/*
 * ChassisDynamometer.c
 *
 * Created: 2021/06/18 19:17:07
 * Author : kiyot
 */ 


#include "sam.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

#include <samd51_error.h>
#include <samd51_gpio.h>
#include <samd51_sercom.h>
#include <samd51_clock.h>
#include <samd51_timer.h>
#include <samd51_adc.h>
#include <samd51_interrupt.h>

#include "console.h"

#include "drivers/usb_cdc.h"
#include "drivers/usb_dev.h"
#include "drivers/dynamo.h"
#include "drivers/ads131m08.h"
#include "command_list.h"

#define GPIO_LED0		SAMD51_GPIO_B04
#define GPIO_LED1		SAMD51_GPIO_B05
#define GPIO_LED2		SAMD51_GPIO_B06
#define GPIO_LED3		SAMD51_GPIO_B07
#define GPIO_LED4		SAMD51_GPIO_B08
#define GPIO_LED5		SAMD51_GPIO_B09
#define GPIO_LED6		SAMD51_GPIO_A04
#define GPIO_LED7		SAMD51_GPIO_A05

static void _putc(const char c)
{
	(void)usbCdc_putc(c);
}

static void _configurate_gpio(void)
{
	// ADC <- VBAT
	samd51_gpio_configure(SAMD51_GPIO_A02, SAMD51_GPIO_IN, SAMD51_GPIO_NO_PULL, SAMD51_GPIO_MUX_FUNC_B);
	// AERF
	samd51_gpio_configure(SAMD51_GPIO_A03, SAMD51_GPIO_IN, SAMD51_GPIO_NO_PULL, SAMD51_GPIO_MUX_FUNC_B);

	// LED
	samd51_gpio_configure(SAMD51_GPIO_B04, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//LED0
	samd51_gpio_configure(SAMD51_GPIO_B05, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//LED1
	samd51_gpio_configure(SAMD51_GPIO_B06, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//LED2
	samd51_gpio_configure(SAMD51_GPIO_B07, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//LED3
	samd51_gpio_configure(SAMD51_GPIO_B08, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//LED4
	samd51_gpio_configure(SAMD51_GPIO_B09, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//LED5
	samd51_gpio_configure(SAMD51_GPIO_A04, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//LED6
	samd51_gpio_configure(SAMD51_GPIO_A05, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//LED7

	// ADS131M08
	samd51_gpio_configure(SAMD51_GPIO_A06, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//RESET#
	samd51_gpio_configure(SAMD51_GPIO_A07, SAMD51_GPIO_IN,  SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_A);	//DRDY#
	samd51_gpio_configure(SAMD51_GPIO_A08, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_C);	//MOSI
	samd51_gpio_configure(SAMD51_GPIO_A09, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_C);	//SCLK
	samd51_gpio_configure(SAMD51_GPIO_A10, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//CS#
	samd51_gpio_configure(SAMD51_GPIO_A11, SAMD51_GPIO_IN,  SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_C);	//MISO

	samd51_gpio_output(SAMD51_GPIO_A06, 0);


	//Driver
	samd51_gpio_configure(SAMD51_GPIO_B10, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_E);	//PWM0-0
	samd51_gpio_configure(SAMD51_GPIO_B11, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_E);	//PWM0-1
	samd51_gpio_configure(SAMD51_GPIO_B12, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//DIR0
	samd51_gpio_configure(SAMD51_GPIO_B13, SAMD51_GPIO_OUT, SAMD51_GPIO_NO_PULL,     SAMD51_GPIO_MUX_FUNC_A);	//INTR0

	samd51_gpio_configure(SAMD51_GPIO_B14, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//DIR1
	samd51_gpio_configure(SAMD51_GPIO_B15, SAMD51_GPIO_OUT, SAMD51_GPIO_NO_PULL,     SAMD51_GPIO_MUX_FUNC_A);	//INTR1
	samd51_gpio_configure(SAMD51_GPIO_A12, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_E);	//PWM1-0
	samd51_gpio_configure(SAMD51_GPIO_A13, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_E);	//PWM1-1
	
	samd51_gpio_configure(SAMD51_GPIO_A14, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_E);	//PWM2-0
	samd51_gpio_configure(SAMD51_GPIO_A15, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_E);	//PWM2-1
	samd51_gpio_configure(SAMD51_GPIO_A16, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//DIR2
	samd51_gpio_configure(SAMD51_GPIO_A17, SAMD51_GPIO_OUT, SAMD51_GPIO_NO_PULL,     SAMD51_GPIO_MUX_FUNC_A);	//INTR2
	
	samd51_gpio_configure(SAMD51_GPIO_A18, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//DIR3
	samd51_gpio_configure(SAMD51_GPIO_A19, SAMD51_GPIO_OUT, SAMD51_GPIO_NO_PULL,     SAMD51_GPIO_MUX_FUNC_A);	//INTR3
	samd51_gpio_configure(SAMD51_GPIO_B16, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_G);	//PWM3-0
	samd51_gpio_configure(SAMD51_GPIO_B17, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_G);	//PWM3-1


	// USB-CTRL
	samd51_gpio_configure(SAMD51_GPIO_A20, SAMD51_GPIO_IN,  SAMD51_GPIO_NO_PULL,     SAMD51_GPIO_MUX_DEFAULT);	//USB2_POWER_EN
	samd51_gpio_configure(SAMD51_GPIO_A21, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);	//USB2_POWER_EM_MCU
	samd51_gpio_configure(SAMD51_GPIO_A22, SAMD51_GPIO_IN,  SAMD51_GPIO_NO_PULL,     SAMD51_GPIO_MUX_DEFAULT);	//USB2_FAULT
	
	//USB
	samd51_gpio_configure(SAMD51_GPIO_A24, SAMD51_GPIO_INOUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_H);
	samd51_gpio_configure(SAMD51_GPIO_A25, SAMD51_GPIO_INOUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_H);
	

	return;
}

#define CD_CLK_GEN_NUMBER_MAIN			(0)
#define CD_CLK_GEN_NUMBER_1MHZ			(1)
#define CD_CLK_GEN_NUMBER_48MHZ			(2)
#define CD_CLK_GEN_NUMBER_32kHZ			(3)

void _configurate_clock_tree(void)
{
	//J 1MHzのクロックソースを作る
	samd51_gclk_configure_generator(CD_CLK_GEN_NUMBER_1MHZ, SAMD51_GCLK_SRC_DFLL, 48, 0, SAMD51_GCLK_DIV_NORMAL);

	//J 32kHz backup clock
	samd51_gclk_configure_generator(CD_CLK_GEN_NUMBER_32kHZ, SAMD51_GCLK_SRC_OSCULP32K, 0, 0, SAMD51_GCLK_DIV_NORMAL);

	//J 48MHzのクロック源をMAINとは別に用意
	samd51_gclk_configure_generator(CD_CLK_GEN_NUMBER_48MHZ, SAMD51_GCLK_SRC_DFLL, 0, 0, SAMD51_GCLK_DIV_NORMAL);
	//J 48MHz 基準クロックをクロジェネ0番から移動させる
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_OSCCTRL_DFLL48, CD_CLK_GEN_NUMBER_48MHZ);

	//J DFLL用のクロックを準備
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_OSCCTRL_FDPLL_32K, CD_CLK_GEN_NUMBER_32kHZ);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_OSCCTRL_FDPLL0, CD_CLK_GEN_NUMBER_1MHZ);

	//J DFLLを120MHzで起動
	samd51_mclk_enable(SAMD51_APBA_OSCCTRL, 1);
	SAMD51_OSC_OPT osc_opt;
	{
		osc_opt.dco_filter = 0;
		osc_opt.filter = 0;
		osc_opt.filter_en = 1;
	}
	samd51_oscillator_dpll_enable(0, SAMD51_OSC_REF_GCLK, 1000000, 120000000, &osc_opt);

	//J 120MHzになったFDPLLをGCLK0のソースに設定する（死にそう）
	samd51_gclk_configure_generator(CD_CLK_GEN_NUMBER_MAIN, SAMD51_GCLK_SRC_DPLL0, 0, 0, SAMD51_GCLK_DIV_NORMAL);


	//J USBを使うのでクロックを供給しておく
	samd51_mclk_enable(SAMD51_AHB_USB, 1);
	samd51_mclk_enable(SAMD51_APBB_USB, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_USB, CD_CLK_GEN_NUMBER_48MHZ);

	//J ADCにクロックを供給
	samd51_mclk_enable(SAMD51_APBD_ADCn0, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_ADC0, CD_CLK_GEN_NUMBER_48MHZ);

	//J SERCOM0にクロックを供給
	samd51_mclk_enable(SAMD51_APBA_SERCOM0, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_SERCOM0_CORE, CD_CLK_GEN_NUMBER_48MHZ);

	// TimerにClockを供給
	samd51_mclk_enable(SAMD51_APBA_TCn0, 1);
	samd51_mclk_enable(SAMD51_APBA_TCn1, 1);
	samd51_mclk_enable(SAMD51_APBB_TCn2, 1);
	samd51_mclk_enable(SAMD51_APBB_TCn3, 1);
	samd51_mclk_enable(SAMD51_APBC_TCn5, 1);
	
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_TC0_TC1, CD_CLK_GEN_NUMBER_1MHZ);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_TC2_TC3, CD_CLK_GEN_NUMBER_1MHZ);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_TC4_TC5, CD_CLK_GEN_NUMBER_1MHZ);

	samd51_mclk_enable(SAMD51_APBB_TCCn0, 1);
	samd51_mclk_enable(SAMD51_APBB_TCCn1, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_TCC0_TCC1, CD_CLK_GEN_NUMBER_1MHZ);

	//J 外部割込み有効化
	samd51_mclk_enable(SAMD51_APBA_EIC, 1);
	samd51_gclk_configure_peripheral_channel(SAMD51_GCLK_EIC, CD_CLK_GEN_NUMBER_48MHZ);

	return;
}

static void _banner(void)
{
	return;
}

int main(void)
{
	/* Initialize the SAM system */
    SystemInit();

	_configurate_gpio();
	_configurate_clock_tree();

	samd51_external_interrupt_initialize(0);

	dynamoInitialize();

	usbCdcRegisterLinkUpCallback(_banner);
	static char reg_buf[128];
	sprintf(reg_buf, "%08X%08X%08X%08X",
		*((unsigned int *)0x008061FC),
		*((unsigned int *)0x00806010),
		*((unsigned int *)0x00806014),
		*((unsigned int *)0x00806018));
	initialize_usb(reg_buf, "Mini4wd Chassis Dynamo");

	ads131m08_initialize(SAMD51_SERCOM0, SAMD51_GPIO_A06, SAMD51_EIC_CHANNEL7);

	consoleInitialize(_putc);
	consoleInstallCommand(&g_ctrl_cmd);
	consoleInstallCommand(&g_state_cmd);
	consoleInstallCommand(&g_led_cmd);
	consoleInstallCommand(&g_adcw_cmd);
	consoleInstallCommand(&g_adcr_cmd);
	consoleInstallCommand(&g_adccap_cmd);
	consoleInstallCommand(&g_monitor_cmd);
	consoleInstallCommand(&g_sensor_cmd);
	consoleInstallCommand(&g_usb_ctrl_cmd);	


    /* Replace with your application code */
    while (1) {
		char c = '\0';
		if (0 == usbCdc_try_rx((uint8_t *)&c))
			consoleUpdate(c);
    }
}
