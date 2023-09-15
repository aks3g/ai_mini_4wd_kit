/*
 * iam20680ht.c
 *
 * Created: 2023/03/20 4:38:28
 *  Author: kiyot
 */ 
#include <stdint.h>

#include <samd51_error.h>
#include <samd51_i2c.h>
#include <samd51_gpio.h>

#include "../include/ai_mini4wd.h"
#include "../include/ai_mini4wd_sensor.h"
#include "../include/internal/iam20680ht.h"
#include "../include/internal/odometer.h"
#include "../include/internal/sensor.h"

#define IAM20680_ADR								(0x68)

#define IAM20680HT_SELF_TEST_X_GYRO					(0x00)
#define IAM20680HT_SELF_TEST_Y_GYRO					(0x01)
#define IAM20680HT_SELF_TEST_Z_GYRO					(0x02)
#define IAM20680HT_SELF_TEST_X_ACCEL				(0x0D)
#define IAM20680HT_SELF_TEST_Y_ACCEL				(0x0E)
#define IAM20680HT_SELF_TEST_Z_ACCEL				(0x0F)

#define IAM20680HT_XG_OFFS_USRH						(0x13)
#define IAM20680HT_XG_OFFS_USRL						(0x14)
#define IAM20680HT_YG_OFFS_USRH						(0x15)
#define IAM20680HT_YG_OFFS_USRL						(0x16)
#define IAM20680HT_ZG_OFFS_USRH						(0x17)
#define IAM20680HT_ZG_OFFS_USRL						(0x18)

#define IAM20680HT_SMPLRT_DIV						(0x19)
#define IAM20680HT_CONFIG							(0x1A)
#define IAM20680HT_GYRO_CONFIG						(0x1B)
#define IAM20680HT_ACCEL_CONFIG						(0x1C)
#define IAM20680HT_ACCEL_CONFIG2					(0x1D)
#define IAM20680HT_LP_MODE_CFG						(0x1E)
#define IAM20680HT_ACCEL_WOM_THR					(0x1F)

#define IAM20680HT_FIFO_EN							(0x23)
#define IAM20680HT_FSYNC_INT						(0x36)
#define IAM20680HT_INT_PIN_CFG						(0x37)
#define IAM20680HT_INT_ENABLE						(0x38)
#define IAM20680HT_INT_STATUS						(0x3A)

#define IAM20680HT_ACCEL_XOUT_H						(0x3B)
#define IAM20680HT_ACCEL_XOUT_L						(0x3C)
#define IAM20680HT_ACCEL_YOUT_H						(0x3D)
#define IAM20680HT_ACCEL_YOUT_L						(0x3E)
#define IAM20680HT_ACCEL_ZOUT_H						(0x3F)
#define IAM20680HT_ACCEL_ZOUT_L						(0x40)

#define IAM20680HT_TEMP_OUT_H						(0x41)
#define IAM20680HT_TEMP_OUT_L						(0x42)

#define IAM20680HT_GYRO_XOUT_H						(0x43)
#define IAM20680HT_GYRO_XOUT_L						(0x44)
#define IAM20680HT_GYRO_YOUT_H						(0x45)
#define IAM20680HT_GYRO_YOUT_L						(0x46)
#define IAM20680HT_GYRO_ZOUT_H						(0x47)
#define IAM20680HT_GYRO_ZOUT_L						(0x48)

#define IAM20680HT_SIGNAL_PATH_RESET				(0x68)
#define IAM20680HT_ACCEL_INTEL_CTRL					(0x69)
#define IAM20680HT_USER_CTRL						(0x6A)
#define IAM20680HT_PWR_MGMT_1						(0x6B)
#define IAM20680HT_PWR_MGMT_2						(0x6C)

#define IAM20680HT_FIFO_COUNTH						(0x72)
#define IAM20680HT_FIFO_COUNTL						(0x73)
#define IAM20680HT_FIFO_R_W							(0x74)
#define IAM20680HT_WHO_AM_I							(0x75)

#define IAM20680HT_XA_OFFSET_H						(0x77)
#define IAM20680HT_XA_OFFSET_L						(0x78)
#define IAM20680HT_YA_OFFSET_H						(0x7A)
#define IAM20680HT_YA_OFFSET_L						(0x7B)
#define IAM20680HT_ZA_OFFSET_H						(0x7D)
#define IAM20680HT_ZA_OFFSET_L						(0x7E)

#define IAM20680_CONFIG_DLPF_CFG_POS				(0)

#define IAM20680HT_GYRO_FS_SEL_POS					(3)
#define IAM20680HT_GYRO_FS_250DSP					(0)
#define IAM20680HT_GYRO_FS_500DSP					(1)
#define IAM20680HT_GYRO_FS_1000DSP					(2)
#define IAM20680HT_GYRO_FS_2000DSP					(3)
#define IAM20680HT_GYRO_FCHOICE_POS					(0)


#define IAM20680HT_ACCEL_FS_SEL_POS					(3)
#define IAM20680HT_ACCEL_FS_2G						(0)
#define IAM20680HT_ACCEL_FS_4G						(1)
#define IAM20680HT_ACCEL_FS_8G						(2)
#define IAM20680HT_ACCEL_FS_16G						(3)

#define IAM20680HT_ACCEL_DEC2_CFGS_POS				(4)
#define IAM20680HT_ACCEL_AVERAGE_4SAMPLES			(0)
#define IAM20680HT_ACCEL_AVERAGE_8SAMPLES			(1)
#define IAM20680HT_ACCEL_AVERAGE_16SAMPLES			(2)
#define IAM20680HT_ACCEL_AVERAGE_32SAMPLES			(3)

#define IAM20680HT_ACCEL_FCHOICE_B_POS				(3)
#define IAM20680HT_ACCEL_A_DLPF_CFG_POS				(0)

#define IAM20680HT_LP_MODE_CFG_G_AVGCFG_POS			(4)

#define IAM20680HT_INT_CFG_INT_LEVEL_POS			(7)
#define IAM20680HT_INT_CFG_INT_OPEN_POS				(6)
#define IAM20680HT_INT_CFG_LATCH_INT_EN_POS			(5)
#define IAM20680HT_INT_CFG_INT_RD_CLEAR_POS			(4)
#define IAM20680HT_INT_CFG_FSYNC_INT_LEVEL_POS		(3)
#define IAM20680HT_INT_CFG_FSYNC_INT_MODE_EN_POS	(2)

#define IAM20680HT_INT_ENABLE_WOM_INT_EN_POS		(5)
#define IAM20680HT_INT_ENABLE_FIFO_OFLOW_EN_POS		(4)
#define IAM20680HT_INT_ENABLE_GDRIVE_INT_EN_POS		(2)
#define IAM20680HT_INT_ENABLE_DATA_RDY_INT_EN_POS	(0)


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static void _checkDataReadyAndRead(void);
static void _capture_done_cb(int status);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static volatile uint8_t sAccelReady = 0;
static volatile uint8_t sGyroReady  = 0;
static volatile uint8_t sInhibit = 0;


static AiMini4wdImuRawData sRxBuf;
static uint8_t sTxbuf[1];



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
int iam20680ht_probe(void)
{
	uint8_t txbuf[2];
	uint8_t rxbuf[1];
	int ret = 0;
	volatile int wait = 0;

	// initialzie clock
#if 1
	txbuf[0] = IAM20680HT_PWR_MGMT_1;
	txbuf[1] = 0x81;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}
	do {
		wait = 120000;
		while(wait--);
		ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);
	}while(ret == AI_ERROR_I2C_NACK || (rxbuf[0] & 0x80) != 0);
#endif

	// read Who am i Register
	txbuf[0] = IAM20680HT_WHO_AM_I;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);
	if (ret != 0) {
		return ret;
	}
	else if (rxbuf[0] != 0xA9) {
//		return AI_ERROR_NODEV;
	}


	// Setup Sensor rate = 50Hz
	txbuf[0] = IAM20680HT_SMPLRT_DIV;
	txbuf[1] = (1000 / 50) - 1;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);

	// Config
	txbuf[0] = IAM20680HT_CONFIG;
	txbuf[1] = (2 << IAM20680_CONFIG_DLPF_CFG_POS);
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);

	// Gyro Config
	txbuf[0] = IAM20680HT_GYRO_CONFIG;
	txbuf[1] = (IAM20680HT_GYRO_FS_2000DSP << IAM20680HT_GYRO_FS_SEL_POS) | (0 << IAM20680HT_GYRO_FCHOICE_POS);
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);

	// Low Power Mode
	txbuf[0] = IAM20680HT_LP_MODE_CFG;
	txbuf[1] = (5 << IAM20680HT_LP_MODE_CFG_G_AVGCFG_POS);
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);

	// Accel Config 1
	txbuf[0] = IAM20680HT_ACCEL_CONFIG;
	txbuf[1] = (IAM20680HT_ACCEL_FS_16G << IAM20680HT_ACCEL_FS_SEL_POS) | (2 << IAM20680HT_ACCEL_A_DLPF_CFG_POS);
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);

	// Accel Config 2
	txbuf[0] = IAM20680HT_ACCEL_CONFIG2;
	txbuf[1] = (2<<IAM20680HT_ACCEL_A_DLPF_CFG_POS) | (IAM20680HT_ACCEL_AVERAGE_8SAMPLES << IAM20680HT_ACCEL_FS_SEL_POS);
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);

	// Interrupt config
	txbuf[0] = IAM20680HT_INT_PIN_CFG;
	txbuf[1] = (1<<IAM20680HT_INT_CFG_INT_LEVEL_POS) | (0<<IAM20680HT_INT_CFG_LATCH_INT_EN_POS) | (1<<IAM20680HT_INT_CFG_INT_RD_CLEAR_POS);
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);

	// Interrupt Enable
	txbuf[0] = IAM20680HT_INT_ENABLE;
	txbuf[1] = (1<<IAM20680HT_INT_ENABLE_DATA_RDY_INT_EN_POS);
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, txbuf, 1, rxbuf, 1, NULL);
	
	
//	samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, sTxbuf, 1, sRxBuf.bytes, 14, NULL);
	
	// Check Interrupt pin
//	while(0 == samd51_gpio_input(SAMD51_GPIO_A19));
	return ret;
}

/*---------------------------------------------------------------------------*/
int iam20680ht_grab_oneshot(void)
{
	sTxbuf[0] = IAM20680HT_ACCEL_XOUT_H;
	return samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, sTxbuf, 1, sRxBuf.bytes, 1, NULL);
}

/*---------------------------------------------------------------------------*/
int iam20680ht_on_int(void)
{
	if (sInhibit) {
		return -1;
	}

	_checkDataReadyAndRead();
	
	return 0;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static void _checkDataReadyAndRead(void)
{
	int ret = 0;

	sInhibit    = 1;
	sAccelReady = 0;
	sGyroReady  = 0;

	//J Odometerを使用する場合には、ここでデータ読み込みのKickを行う
	if (aiMini4wdOdometerEnabled()) {
		ret = odometer_grab();
	}
	
	sTxbuf[0] = IAM20680HT_ACCEL_XOUT_H;
	ret = -1;

	while ((ret != 0) && (ret != AI_ERROR_I2C_BUSY)) {
		ret = samd51_i2c_txrx(SAMD51_SERCOM3, IAM20680_ADR, sTxbuf, 1, sRxBuf.bytes, 14, _capture_done_cb);
	}

	return;
}

/*---------------------------------------------------------------------------*/
static void _capture_done_cb(int status)
{
	if (status == AI_OK) {
		//J Odometerを使用している場合、データが読み込まれるのを待つ
		if (aiMini4wdOdometerEnabled() && !(odometer_is_busy())) {
			aiMini4wdUpdateOdometerData(odometer_get_latest_data());
		}

		sRxBuf.imu_us.accel[0] = ((sRxBuf.imu_us.accel[0] << 8) & 0xff00) | ((sRxBuf.imu_us.accel[0] >> 8) & 0x00ff);
		sRxBuf.imu_us.accel[1] = ((sRxBuf.imu_us.accel[1] << 8) & 0xff00) | ((sRxBuf.imu_us.accel[1] >> 8) & 0x00ff);
		sRxBuf.imu_us.accel[2] = ((sRxBuf.imu_us.accel[2] << 8) & 0xff00) | ((sRxBuf.imu_us.accel[2] >> 8) & 0x00ff);

		sRxBuf.imu_us.gyro[0] = ((sRxBuf.imu_us.gyro[0] << 8) & 0xff00) | ((sRxBuf.imu_us.gyro[0] >> 8) & 0x00ff);
		sRxBuf.imu_us.gyro[1] = ((sRxBuf.imu_us.gyro[1] << 8) & 0xff00) | ((sRxBuf.imu_us.gyro[1] >> 8) & 0x00ff);
		sRxBuf.imu_us.gyro[2] = ((sRxBuf.imu_us.gyro[2] << 8) & 0xff00) | ((sRxBuf.imu_us.gyro[2] >> 8) & 0x00ff);

		// Keep compatibility 
		uint16_t swap=sRxBuf.imu_us.gyro[0];
		sRxBuf.imu_us.gyro[0]=sRxBuf.imu_us.gyro[1];
		sRxBuf.imu_us.gyro[1]=swap;

		swap=sRxBuf.imu_us.accel[0];
		sRxBuf.imu_us.accel[0]=sRxBuf.imu_us.accel[1];
		sRxBuf.imu_us.accel[1]=swap;


		aiMini4wdUpdateSensorData(&sRxBuf);
	}

	sInhibit = 0;

	return;
}