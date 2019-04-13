/*
 * hids.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdint.h>
#include <stddef.h>

#include <samd51_error.h>
#include <samd51_gpio.h>
#include <samd51_interrupt.h>

#include "ai_mini4wd_hid.h"


int aiMini4wdToggleLedPattern(uint32_t leds)
{
	if (leds & 0x01) {
		samd51_gpio_output_toggle(SAMD51_GPIO_B07);
	}
	if (leds & 0x02) {
		samd51_gpio_output_toggle(SAMD51_GPIO_B08);
	}
	if (leds & 0x04) {
		samd51_gpio_output_toggle(SAMD51_GPIO_B09);
	}
	if (leds & 0x08) {
		samd51_gpio_output_toggle(SAMD51_GPIO_A04);
	}

	return 0;
}

int aiMini4wdSetLedPattern(uint32_t leds)
{
	if (leds & 0x01) {
		samd51_gpio_output(SAMD51_GPIO_B07, 0);
	}
	if (leds & 0x02) {
		samd51_gpio_output(SAMD51_GPIO_B08, 0);
	}
	if (leds & 0x04) {
		samd51_gpio_output(SAMD51_GPIO_B09, 0);
	}
	if (leds & 0x08) {
		samd51_gpio_output(SAMD51_GPIO_A04, 0);
	}
	return 0;
}


int aiMini4wdClearLedPattern(uint32_t leds)
{
	if (leds & 0x01) {
		samd51_gpio_output(SAMD51_GPIO_B07, 1);
	}
	if (leds & 0x02) {
		samd51_gpio_output(SAMD51_GPIO_B08, 1);
	}
	if (leds & 0x04) {
		samd51_gpio_output(SAMD51_GPIO_B09, 1);
	}
	if (leds & 0x08) {
		samd51_gpio_output(SAMD51_GPIO_A04, 1);
	}

	return 0;
}


int aiMini4wdPrintLedPattern(uint32_t leds)
{
	samd51_gpio_output(SAMD51_GPIO_B07, (leds & 0x01) ? 0 : 1);
	samd51_gpio_output(SAMD51_GPIO_B08, (leds & 0x02) ? 0 : 1);
	samd51_gpio_output(SAMD51_GPIO_B09, (leds & 0x04) ? 0 : 1);
	samd51_gpio_output(SAMD51_GPIO_A04, (leds & 0x08) ? 0 : 1);

	return 0;
}

void aiMini4wdSetStatusLed(int on)
{
	if (on) {
		samd51_gpio_output(SAMD51_GPIO_B06, 0);
	}
	else {
		samd51_gpio_output(SAMD51_GPIO_B06, 1);
	}
}


#define AI_MINI4WD_SWITCH_STABLE_THRESHOLD					(5)
typedef struct AiMIni4wdSwitchContext_t
{
	int status;
	int stable_cnt;
	AiMini4wdSwitchCallback changed_cb;
} AiMIni4wdSwitchContext;

static AiMIni4wdSwitchContext sSwitchCtx[2];

int aiMini4wdInitializeSwitch(void)
{
	sSwitchCtx[0].status = samd51_gpio_input(SAMD51_GPIO_A21);
	sSwitchCtx[1].status = samd51_gpio_input(SAMD51_GPIO_A22);
	
	sSwitchCtx[0].stable_cnt = 0;
	sSwitchCtx[1].stable_cnt = 0;

	sSwitchCtx[0].changed_cb = NULL;
	sSwitchCtx[1].changed_cb = NULL;

	return AI_OK;
}

int aiMini4wdUpdateSwitchStatus(void)
{
	if(sSwitchCtx[0].status != samd51_gpio_input(SAMD51_GPIO_A21)) {
		sSwitchCtx[0].stable_cnt++;
		if (sSwitchCtx[0].stable_cnt > AI_MINI4WD_SWITCH_STABLE_THRESHOLD) {
			sSwitchCtx[0].status = 	samd51_gpio_input(SAMD51_GPIO_A21);
			if (sSwitchCtx[0].changed_cb) {
				sSwitchCtx[0].changed_cb(sSwitchCtx[0].status == 0);
			}
		}
	}
	else{
		sSwitchCtx[0].stable_cnt = 0;
	}

	if(sSwitchCtx[1].status != samd51_gpio_input(SAMD51_GPIO_A22)) {
		sSwitchCtx[1].stable_cnt++;
		if (sSwitchCtx[1].stable_cnt > AI_MINI4WD_SWITCH_STABLE_THRESHOLD) {
			sSwitchCtx[1].status = 	samd51_gpio_input(SAMD51_GPIO_A22);
			if (sSwitchCtx[1].changed_cb) {
				sSwitchCtx[1].changed_cb(sSwitchCtx[1].status == 0);
			}
		}
	}
	else{
		sSwitchCtx[1].stable_cnt = 0;
	}

	return AI_OK;
}

int aiMini4wdGetSwitchStatus(AiMini4wdSwitch sw) 
{
	return (sSwitchCtx[sw].status == 0) ? 1 : 0;
}

int aiMini4wdRegisterSwitchCb(AiMini4wdSwitch sw, AiMini4wdSwitchCallback cb)
{
	if (sw != cAiMini4wdSwitch0 && sw != cAiMini4wdSwitch1) {
		return AI_ERROR_INVALID;
	}
	
	sSwitchCtx[sw].changed_cb = cb;
	
	return AI_OK;
}

int aiMini4wdRegisterExtInterrupt(AiMini4wdOnExtIntCallback cb)
{
	int ret = samd51_external_interrupt_setup(SAMD51_EIC_CHANNEL0, SAMD51_EIC_SENSE_RISE, 0, cb);
	if (ret != AI_OK) {
		return ret;
	}
	samd51_external_interrupt_enable(SAMD51_EIC_CHANNEL0, 1);

	return AI_OK;
}
