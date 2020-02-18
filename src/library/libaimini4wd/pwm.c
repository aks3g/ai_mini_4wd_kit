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
	
	samd51_tc_initialize_as_pwm(SAMD51_TC4, 48*1000*1000, 50, 0);
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
	
	float error_n_1;
	float error_n_2;

	int last_duty;	
} AiMIni4wdPidControlContext;

static AiMIni4wdPidControlContext sPidCtx =
{
	0.05, 0.01, 0.0,
	0.0,
	0.0, 0.0,
	0
};

/*--------------------------------------------------------------------------*/
int aiMini4wdMotorDriverSetRpm(int rpm)
{
	sDriveMode = DRIVE_MODE_RPM;

	sPidCtx.target_rpm  = rpm;
	sPidCtx.error_n_1 = 0.0;
	sPidCtx.error_n_2 = 0.0;

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

/*--------------------------------------------------------------------------*/
int aiMini4wdMotorDriverUpdateRpm(float rpm)
{
	if (sDriveMode != DRIVE_MODE_RPM) {
		return AI_OK;
	}
	
	
	float error = sPidCtx.target_rpm - rpm; 
	
	int delta =  (error - sPidCtx.error_n_1) * sPidCtx.Kp + 
				 (error * sPidCtx.Ki) + 
				((error - sPidCtx.error_n_1) - (sPidCtx.error_n_1 - sPidCtx.error_n_2)) * sPidCtx.Kd;	
	
//	aiMini4wdLitePrintf("%f -> %f. %d + %d\r\n", sPidCtx.target_rpm ,rpm, sPidCtx.last_duty, delta);

	sPidCtx.last_duty = sPidCtx.last_duty + delta;
	sPidCtx.error_n_2 = sPidCtx.error_n_1;
	sPidCtx.error_n_1 = error;
	
	if (sPidCtx.last_duty > 255) {
		sPidCtx.last_duty = 255;
	}
	else if (sPidCtx.last_duty < -255) {
		sPidCtx.last_duty = -255;
	}

	_setDuty(sPidCtx.last_duty);
	
	return AI_OK;
}
