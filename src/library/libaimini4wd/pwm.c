/*
 * pwm.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 

#include <stdint.h>
#include <stddef.h>

#define	REV2		(1)

#include <samd51_error.h>
#include <samd51_clock.h>
#include <samd51_timer.h>
#include <samd51_gpio.h>

#include "include/ai_mini4wd_motor_driver.h"
#include "include/internal/pwm.h"
#include "include/internal/clock.h"
#include "include/ai_mini4wd.h"

extern uint32_t gAiMini4wdInitFlags;

static uint32_t sMinusCountMax = 5;

static SAMD51_GPIO_PORT sPwm1Port = SAMD51_GPIO_B14;
static SAMD51_GPIO_PORT sPwm2Port = SAMD51_GPIO_B15;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef enum AiMIni4wdDriveMode_t
{
	DRIVE_MODE_DUTY,
	DRIVE_MODE_RPM
} AiMIni4wdDriveMode;

static AiMIni4wdDriveMode sDriveMode = DRIVE_MODE_DUTY;
static int sCurrentDuty = 0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static int _setDuty(int duty);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int aiMini4WdInitializePwm(void)
{
	if (gAiMini4wdInitFlags & AI_MINI_4WD_INIT_FLAG_USE_TEST_TYPE_HW) {
		sPwm1Port = SAMD51_GPIO_B15;
		sPwm2Port = SAMD51_GPIO_B14;
	}

	samd51_mclk_enable(SAMD51_APBC_TCn4, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_TC4_TC5, LIB_MINI_4WD_CLK_GEN_NUMBER_48MHZ);
	
	samd51_tc_initialize_as_pwm(SAMD51_TC4, SAMD51_TC_PRESCALE_DIV256);
	aiMini4wdMotorDriverDrive(0);

	return 0;
}

/*--------------------------------------------------------------------------*/
int aiMini4wdMotorDriverDrive(int duty) 
{
	sDriveMode = DRIVE_MODE_DUTY;
	
	return _setDuty(duty);
}

/*--------------------------------------------------------------------------*/
int aiMini4wdMotorDriverGetDuty(void)
{
  return sCurrentDuty;
}

/*--------------------------------------------------------------------------*/
int aiMini4wdMotorDriverSetMinusCountMax(uint32_t max_count)
{
	sMinusCountMax = max_count;
	
	return 0;
}

/*--------------------------------------------------------------------------*/
static int _setDuty(int duty)
{
	if (sCurrentDuty == duty) {
		return 0;
	}
	
	if (duty == 0) {
		samd51_gpio_configure(SAMD51_GPIO_B12, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);
		samd51_gpio_configure(SAMD51_GPIO_B13, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);
		samd51_gpio_configure(SAMD51_GPIO_B14, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);
		samd51_gpio_configure(SAMD51_GPIO_B15, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);

		samd51_gpio_output(SAMD51_GPIO_B12, 0);
		samd51_gpio_output(SAMD51_GPIO_B13, 0);
		samd51_gpio_output(SAMD51_GPIO_B14, 0);
		samd51_gpio_output(SAMD51_GPIO_B15, 0);
	}
	else if(duty > 0) {
		samd51_gpio_output(SAMD51_GPIO_B12, 0);
		samd51_gpio_output(SAMD51_GPIO_B13, 0);
		samd51_gpio_configure(SAMD51_GPIO_B12, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);
		samd51_gpio_configure(SAMD51_GPIO_B13, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_E);

		samd51_gpio_output(sPwm1Port, 1);
		samd51_gpio_output(sPwm2Port, 0);
		samd51_tc_set_pwm(SAMD51_TC4, 1, duty);
	}
	else if(duty < 0) {
		samd51_gpio_output(SAMD51_GPIO_B12, 0);
		samd51_gpio_output(SAMD51_GPIO_B13, 0);
		samd51_gpio_configure(SAMD51_GPIO_B12, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_E);
		samd51_gpio_configure(SAMD51_GPIO_B13, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);

		samd51_gpio_output(sPwm1Port, 0);
		samd51_gpio_output(sPwm2Port, 1);
		samd51_tc_set_pwm(SAMD51_TC4, 0, -duty);
	}
	
	sCurrentDuty = duty;
	
	return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef struct AiMini4wdPidControlContext_t
{
	float Kp;
	float Ki;
	float Kd;
	
	float target_rpm;
	
	float e_sum;
	float e_1;

} AiMIni4wdPidControlContext;

static AiMIni4wdPidControlContext sPidCtx =
{
	.Kp = 0.05, 
	.Ki = 0.01, 
	.Kd = 0.0,

	.target_rpm = 0.0,

	.e_sum = 0.0,
	.e_1 = 0.0,
};

/*--------------------------------------------------------------------------*/
int aiMini4wdMotorDriverSetRpm(int rpm)
{
	sDriveMode = DRIVE_MODE_RPM;

	if (sPidCtx.target_rpm != rpm) {
		sPidCtx.target_rpm  = rpm;
		sPidCtx.e_sum = 0.0;
		sPidCtx.e_1 = 0.0;
	}

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int aiMini4wdMotorDriverSetPidGain(float Kp, float Ki, float Kd)
{
	sPidCtx.Kp = Kp;
	sPidCtx.Ki = Ki;
	sPidCtx.Kd = Kd;	
	
	return AI_OK;
}


float gDebugError = 0;
float gDebugErrorSum = 0;
/*--------------------------------------------------------------------------*/
int aiMini4wdMotorDriverUpdateRpm(float rpm)
{
	static uint32_t sMinusCnt = 0;
	static float last_rpm = 0;

	if (sDriveMode != DRIVE_MODE_RPM) {
		return AI_OK;
	}
	
	if (rpm > 7000) {
		rpm = last_rpm;
	}
	else {
		last_rpm = rpm;
	}

	float error = sPidCtx.target_rpm - rpm; 
	int u =  (error * sPidCtx.Kp) + 
			 (sPidCtx.e_sum * sPidCtx.Ki) + 
			 ((error - sPidCtx.e_1) * sPidCtx.Kd);

	gDebugError = sPidCtx.e_1;
	gDebugErrorSum = sPidCtx.e_sum;

	sPidCtx.e_1 = error;
	sPidCtx.e_sum += error;

	// Dutyが正であれば、最大値を切る
	if (u >= 0) {
		sMinusCnt = 0;
		if (u > 255) {
			u = 255;
		}
	}
	else {
		sMinusCnt++;
		if (u < -255) {
			u = -255;
		}

		// Dutyを負の状態で長時間キープさせない
		if (sMinusCnt > sMinusCountMax) {
			u = 0;			
		}
	}

	_setDuty(u);
	
	return AI_OK;
}
