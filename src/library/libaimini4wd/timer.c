/*
 * timer.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdint.h>
#include <stddef.h>

#include <samd51_error.h>
#include <samd51_clock.h>
#include <samd51_timer.h>
#include <samd51_adc.h>

#include "include/ai_mini4wd_timer.h"
#include "include/internal/timer.h"
#include "include/internal/clock.h"
#include "include/internal/pwm.h"
#include "include/internal/hids.h"

#include "include/ai_mini4wd_sensor.h"

extern void aiMini4wdUpdateErrorStatusIndication(void);

static AiMini4wdTimerCallback _timer_cb_10ms =  NULL;
static AiMini4wdTimerCallback _timer_cb_100ms =  NULL;

static uint32_t sGlobalTick = 0;


static void _tc0_cb(void);

int aiMini4WdInitializeTimer(void)
{
	samd51_mclk_enable(SAMD51_APBA_TCn0, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_TC0_TC1, LIB_MINI_4WD_CLK_GEN_NUMBER_48MHZ);
	
	//J 10ms 毎のタイマー
	samd51_tc_initialize_as_timer(SAMD51_TC0, 48*1000*1000, 10 * 1000, _tc0_cb);

	return 0;
}

int aiMini4wdTimerRegister10msCallback(AiMini4wdTimerCallback cb)
{
	_timer_cb_10ms = cb;
	return 0;
}

int aiMini4WdTimerRegister100msCallback(AiMini4wdTimerCallback cb)
{
	_timer_cb_100ms = cb;
	return 0;
}

uint32_t aiMini4WdTimerGetSystemtick(void)
{
	return sGlobalTick;
}

static int16_t sBatteryVoltageAdc  = 0;
static int16_t sMotorCurrentAdc  = 0;
static void _updateBatteryVoltage(int status, int16_t val)
{
	sBatteryVoltageAdc = val;
} 

static void _updateMotorCurrent(int status, int16_t val)
{
	sMotorCurrentAdc = val;
}

int aiMini4wdGetBatteryVoltage(float *voltage_mV)
{
	if (voltage_mV == NULL) {
		return AI_ERROR_NULL;
	}

	//J 3.3Vリファレンスで12ビット
	*voltage_mV = 3300.0f * (sBatteryVoltageAdc / 4095.0f);
	
	return AI_OK;
}

int aiMini4wdMotorDriverGetDriveCurrent(float *current_mA)
{
	if (current_mA == NULL) {
		return AI_ERROR_NULL;
	}

	//J 3.3Vリファレンスで12ビット
	*current_mA = 2000.0f * (3300.0f * (sMotorCurrentAdc / 4095.0f) / 500.0f);
	
	return AI_OK;
}



static volatile uint32_t sAdcSwitch = 0;
static void _tc0_cb(void)
{
	sGlobalTick += 10;
	if (_timer_cb_10ms) {
		_timer_cb_10ms();
	}

	//J Update ADC
	if (sAdcSwitch) {
		(void)samd51_adc_convert(0, SAMD51_ADC_SINGLE_END, SAMD51_ADC_POS_AIN14, SAMD51_ADC_NEG_GND, _updateBatteryVoltage);
	}
	else {
		(void)samd51_adc_convert(0, SAMD51_ADC_SINGLE_END, SAMD51_ADC_POS_AIN15, SAMD51_ADC_NEG_GND, _updateMotorCurrent);
	}

	sAdcSwitch = 1 - sAdcSwitch;



	if ((sGlobalTick & 50) == 0) {
		aiMini4wdMotorDriverUpdateRpm(aiMini4wdSensorGetCurrentRpm());
	}

	if ((sGlobalTick % 100) == 0) {
		aiMini4wdUpdateErrorStatusIndication();
		if (_timer_cb_100ms) {
			_timer_cb_100ms();
		}
	}

	(void)aiMini4wdUpdateSwitchStatus();

	return;
}
