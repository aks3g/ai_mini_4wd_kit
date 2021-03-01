/*
 * sensor.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <samd51_error.h>
#include <samd51_gpio.h>
#include <samd51_i2c.h>
#include <samd51_timer.h>
#include <samd51_dac.h>
#include <samd51_ac.h>
#include <samd51_adc.h>
#include <samd51_interrupt.h>
#include <samd51_sercom.h>
#include <samd51_clock.h>

#include "include/internal/lsm6ds3h.h"
#include "include/internal/odometer.h"
#include "include/internal/sensor.h"
#include "include/internal/clock.h"
#include "include/internal/timer.h"
#include "include/internal/registry.h"

#include "include/ai_mini4wd_sensor.h"
#include "include/ai_mini4wd_motor_driver.h"
#include "include/ai_mini4wd_hid.h"
#include "include/ai_mini4wd_timer.h"

static AiMini4wdSensorData sCurrentData;
static AiMini4wdCapturedSensorDataCb sSensorCapturedCb = NULL;
static AiMini4wdOnStartCallback sOnStartCb = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _int0_cb(void)
{
	lsm6ds3h_on_int1();
}

/*--------------------------------------------------------------------------*/
static void _int1_cb(void)
{
	lsm6ds3h_on_int2();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//J 新しいパルスが来るか、オーバーフローしたらリロードかける？
//J ピニオンギアとクラウンギアのギア比は 8 : 20
//J Timer のTickは 48MHz / 64 = 750000Hz
static float sMedianBuf[5] = {0,0,0,0,0};
static float sSorted[5] = {0,0,0,0,0};
static int _float_cmp(const void *a, const void *b)
{
	return *(float *)a - *(float *)b;
}
	
static void _update_rpm(uint16_t count)
{
	float next_rpm = 0;
	if (count != 0) {
		float pulse_time = (float)count * (1.0/750000.0);
		float rpm = 60 * (1.0/pulse_time);

		//J タイヤ軸でのRPM
		next_rpm =  rpm * 8.0f / 20.0f;
	}
	else {
		next_rpm = 0;
	}
	
	sMedianBuf[0] = sMedianBuf[1];
	sMedianBuf[1] = sMedianBuf[2];
	sMedianBuf[2] = sMedianBuf[3];
	sMedianBuf[3] = sMedianBuf[4];
	sMedianBuf[4] = next_rpm;
	
	memcpy(sSorted, sMedianBuf, sizeof(sMedianBuf));
	qsort(sSorted, sizeof(sSorted)/sizeof(float), sizeof(float), _float_cmp);

	sCurrentData.rpm = sSorted[2];
	
	return;	
}

static void _ac_callback(void)
{
	uint16_t counter = samd51_tc_start_onshot(SAMD51_TC2);
	if (sOnStartCb) {
		sOnStartCb();
	}

	_update_rpm(counter);
}

/*--------------------------------------------------------------------------*/
static void _pulse_width_overflow(void)
{
	uint16_t counter = samd51_tc_start_onshot(SAMD51_TC2);

	_update_rpm(counter);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static int _initialize_i2c(void)
{
	samd51_mclk_enable(SAMD51_APBB_SERCOM3, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_SERCOM3_CORE, LIB_MINI_4WD_CLK_GEN_NUMBER_48MHZ);

	samd51_i2c_initialize(SAMD51_SERCOM3, 400000);

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
static int _initialize_ext_i2c(void)
{
	samd51_mclk_enable(SAMD51_APBB_SERCOM2, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_SERCOM2_CORE, LIB_MINI_4WD_CLK_GEN_NUMBER_48MHZ);

	samd51_i2c_initialize(SAMD51_SERCOM2, 400000);

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
static int _initialize_sensor_interrupt(void)
{
	samd51_mclk_enable(SAMD51_APBA_EIC, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_EIC, LIB_MINI_4WD_CLK_GEN_NUMBER_48MHZ);
	samd51_external_interrupt_initialize(0);

	samd51_external_interrupt_setup(SAMD51_EIC_CHANNEL2, SAMD51_EIC_SENSE_RISE, 0, _int0_cb);
	samd51_external_interrupt_setup(SAMD51_EIC_CHANNEL3, SAMD51_EIC_SENSE_RISE, 0, _int1_cb);

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
static int _initialize_tachometer(void)
{
	//J フォトリフレクタからの出力をアナログ比較機に突っ込んでパルス幅をTCで測ります

	//J OneShot Timer
	samd51_mclk_enable(SAMD51_APBB_TCn2, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_TC2_TC3, LIB_MINI_4WD_CLK_GEN_NUMBER_48MHZ);
	samd51_tc_initialize_as_oneshot(SAMD51_TC2, 48*1000*1000, SAMD51_TC_PRESCALE_DIV64, _pulse_width_overflow);

	//J Enable DAC
	samd51_mclk_enable(SAMD51_APBD_DAC, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_DAC, LIB_MINI_4WD_CLK_GEN_NUMBER_1MHZ);

	samd51_dac_initialize(0, SAMD51_DAC_REF_BUFFERED_EXTERNAL_VOLTAGE);
	AiMini4wdRegistry *regstry = aiMini4wdRegistryGet();
	if (regstry != NULL) {
		aiMini4wdSensorSetTachometerThreshold(regstry->sdk_data.field.tachometer_threshold, 0);
	} else {
		aiMini4wdSensorSetTachometerThreshold(1200, 0);
	}
	


	//J Enable AC
	samd51_mclk_enable(SAMD51_APBC_AC, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_AC, LIB_MINI_4WD_CLK_GEN_NUMBER_48MHZ);
	samd51_ac_initialize(0, SAMD51_AC_POS_PIN1, SAMD51_AC_NEG_DAC0, SAMD51_AC_INT_RISING, SAMD51_AC_OUTPUT_SYNC, SAMD51_AC_FILTER_NO, SAMD52_AC_HYST50mV, 0, _ac_callback);

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int aiMini4wdSensorsInitialize(void)
{
	//J Internal Driver
	_initialize_i2c();

	//J Initialize IMU
	lsm6ds3h_probe();

	//J Internal Driver
	_initialize_sensor_interrupt();

	//J EICレジスタのピン状態を確認する
	uint16_t pinstate = 0;
	do {
		samd51_external_interrupt_get_pinstate();
	} while ((pinstate & 0x0C) == 0x0C);

	lsm6ds3h_grab_oneshot();

	_initialize_tachometer();


	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int aiMini4wdSensorsInitializeOdometer(void)
{
	_initialize_ext_i2c();
	
	odometer_probe();
	
	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int aiMini4wdSensorGrabLastData(AiMini4wdSensorData *sensor_data)
{
	memcpy (sensor_data, &sCurrentData, sizeof(AiMini4wdSensorData));
	return AI_OK;
}

/*--------------------------------------------------------------------------*/
float tireSize = 20; //mm
float aiMini4wdSensorGetCurrentRpm(void)
{
	return sCurrentData.rpm;
}

/*--------------------------------------------------------------------------*/
int aiMini4wdSensorSetTireSize(float mm)
{
	tireSize = mm;
	
	return AI_OK;
}

/*--------------------------------------------------------------------------*/
float aiMini4wdSensorGetSpeed(void)
{
	return (sCurrentData.rpm * tireSize * M_PI) * 60.0 / 1000000.0;	
}

/*--------------------------------------------------------------------------*/
int aiMini4wdSensorRegisterCapturedCallback(AiMini4wdCapturedSensorDataCb cb)
{
	sSensorCapturedCb = cb;

	return AI_OK;	
}

/*--------------------------------------------------------------------------*/
int aiMini4wdUpdateSensorData(AiMini4wdImuRawData *imu)
{
	sCurrentData.imu.accel_f[0] = (float)imu->imu.accel[0] * LSM6DS3H_ACCEL_LSB;
	sCurrentData.imu.accel_f[1] = (float)imu->imu.accel[1] * LSM6DS3H_ACCEL_LSB;
	sCurrentData.imu.accel_f[2] = (float)imu->imu.accel[2] * LSM6DS3H_ACCEL_LSB;

	sCurrentData.imu.gyro_f[0] = (float)imu->imu.gyro[0] * LSM6DS3H_ANGUL_LSB;
	sCurrentData.imu.gyro_f[1] = (float)imu->imu.gyro[1] * LSM6DS3H_ANGUL_LSB;
	sCurrentData.imu.gyro_f[2] = (float)imu->imu.gyro[2] * LSM6DS3H_ANGUL_LSB;

	if (sSensorCapturedCb) {
		sSensorCapturedCb(&sCurrentData);
	}

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int aiMini4wdUpdateOdometerData(OdometerData *odometer)
{
	sCurrentData.odometry.delta_x_mm = odometer->delta_x_count * gMMpCnt;
	sCurrentData.odometry.delta_y_mm = odometer->delta_y_count * gMMpCnt;

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int aiMini4wdRegisterOnStartCb(AiMini4wdOnStartCallback cb)
{
	sOnStartCb = cb;

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
volatile static int sAdcConversionDone = 0;
volatile static uint16_t sAdcResult = 0;
void _adcConversionDone(int status, int16_t result)
{
	sAdcConversionDone = 1;
	sAdcResult = result;
}

int _cmp_uint16(const void *p1, const void *p2)
{
	uint16_t v1 = *((uint16_t *)p1);
	uint16_t v2 = *((uint16_t *)p2);
	
	if (v1 < v2) {
		return -1;
	}
	else if (v1 > v2) {
		return 1;
	}
	else {
		return 0;
	}
	
}

/*--------------------------------------------------------------------------*/
int aiMini4wdSensorCalibrateTachoMeter(uint16_t *threshold_mv, uint16_t *work_buf, size_t length)
{
	int ret = 0;

	if (threshold_mv == NULL) {
		return AI_ERROR_NULL;
	}

	//J ADCをLockする
	aiMini4wdCurrentVoltageMonitorControl(0);

	//J モーターをある程度の時間回す
	aiMini4wdMotorDriverDrive(150);
	
	volatile uint32_t tick = aiMini4WdTimerGetSystemtick();
	while ((tick+500) > aiMini4WdTimerGetSystemtick());
	
	//J ADCで全力サンプリング
	for (int i=0 ; i<length ; ++i) {
		sAdcConversionDone= 0;
		ret = samd51_adc_convert(0, SAMD51_ADC_SINGLE_END, SAMD51_ADC_POS_AIN5, SAMD51_ADC_NEG_GND, _adcConversionDone);
		if (ret != AI_OK) {
			return ret;
		}
		while (sAdcConversionDone == 0);

		work_buf[i] = sAdcResult;
	}

	//J モーターを止める
	aiMini4wdMotorDriverDrive(0);

	//J ADCをUnlockする
	aiMini4wdCurrentVoltageMonitorControl(1);

	//J ソートする
	qsort(work_buf, length, sizeof(work_buf[0]), _cmp_uint16);

	//J 閾値を決定
	//J 小さい側と大きい側の5％を捨てて、その中での最大、最小の値
	//J の中間をとる
	uint16_t limited_max = work_buf[(int)(length * 0.95)];
	uint16_t limited_min = work_buf[(int)(length * 0.05)];

	*threshold_mv = (uint16_t)(3300.0f * ((limited_max + limited_min)/2.0f) / 4096.0f);

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int aiMini4wdSensorSetTachometerThreshold(uint16_t threshold_mv, int save)
{
	int ret = AI_OK;
	uint16_t dac_out = (uint16_t)((float)threshold_mv * (4096.0f/3300.0f));
	if (dac_out > 0x0fff) {
		return AI_ERROR_OUT_OF_RANGE;
	}

	//J Flashに保存する
	if (save) {
		AiMini4wdRegistry *regstry = aiMini4wdRegistryGet();
		if (regstry != NULL) {
			regstry->sdk_data.field.tachometer_threshold = threshold_mv;
			ret = aiMini4wdRegistryUpdate();
		}
	}

	samd51_dac_output(0, dac_out);
	
	return ret;
}