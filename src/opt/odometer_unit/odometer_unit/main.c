/*
 * odometer_unit.c
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <utils.h>
#include <core_driver.h>
#include <timer_driver.h>
#include <uart_driver.h>
#include <spi_driver.h>
#include <i2c_driver.h>
#include <gpio_driver.h>

#include "drivers/led.h"
#include "drivers/console.h"
#include "drivers/adns9800.h"
#include "odometer_reg.h"


#define READY_Hi()			gpio_output(GPIO_PORTC, GPIO_PIN4, 1);
#define READY_Lo()			gpio_output(GPIO_PORTC, GPIO_PIN4, 0);

static void gpio_init(void)
{
	initialize_gpio(GPIO_PORTC, GPIO_PIN2, GPIO_OUT, GPIO_PUSH_PULL);
	gpio_output(GPIO_PORTC, GPIO_PIN4, 0);

	initialize_gpio(GPIO_PORTC, GPIO_PIN4, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTC, GPIO_PIN5, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTC, GPIO_PIN6, GPIO_IN,  GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTC, GPIO_PIN7, GPIO_OUT, GPIO_PUSH_PULL);

	gpio_output(GPIO_PORTC, GPIO_PIN4, 1);
	gpio_output(GPIO_PORTC, GPIO_PIN5, 1);
	gpio_output(GPIO_PORTC, GPIO_PIN7, 1);

	initialize_gpio(GPIO_PORTD, GPIO_PIN4, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTD, GPIO_PIN5, GPIO_OUT, GPIO_PUSH_PULL);
	gpio_output(GPIO_PORTD, GPIO_PIN4, 1);
	gpio_output(GPIO_PORTD, GPIO_PIN5, 1);


	return;
}

static int new_putchar(char c, FILE *stream)
{
	(void)stream;
	uart_tx((uint8_t *)&c, 1);
	return 0;
}

static volatile uint8_t sReadReq = 0;
static void _sTimerCB(void)
{
	sReadReq = 1;
}



int main(void)
{ 
 	uint32_t systemClock = initialize_clock(CORE_SRC_RC32MHZ, NULL, NULL);

	gpio_init();
 
	READY_Lo();
 
 	initialize_uart(USART0_ON_PORTD_0, 115200, systemClock);

	//J stdoutを付け替える
	static FILE new_stdout = FDEV_SETUP_STREAM(new_putchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &new_stdout;

	int ret = initialize_tcc5_as_timerMilSec(10, systemClock);
	if (ret == TIMER_OK) {
		tcc5_timer_registerCallback(_sTimerCB);
	}

	//J 3Wireモードで動かして、CSは自力で動かす
	initialize_spi_master(SPI_TYPE_4WIRE, SPI_TRANSFER_MODE_3, SPI_DATA_ORDER_MSB_FIRST, SPI_CLK_DIV_16);

	Enable_Int();

	reg_initialize();

/*
	I2C_INIT_OPT i2c_opt = {
		.ownAddress,
		.startCB,
		.stopCB,
		.masterRxCB,
		.masterTxCB
	};
	initialize_i2c(400000, systemClock, I2C_SLAVE, &i2c_opt);
*/

	printf("\n");
	printf("\n");
	printf("--------------------------------------------------\n");
	printf("  Odometer for Ai mini4wd Kit. \n");
	printf("  Build Data %s %s\n", __DATE__, __TIME__);
	printf("--------------------------------------------------");

	adns9800_initialize();
	led_set(LED0, 1);
	READY_Hi();

	console_update('\n');

	uint8_t cnt = 0;	
    while (1) {
		while (uart_get_rxlen() > 0) {
			char c = 0;
			uint32_t len = 0;
			uart_rx_n((uint8_t *)&c, 1, &len);
			console_update(c);
		}
		
		if (sReadReq) {
			sReadReq = 0;
			adns9800_update();

			cnt++;
			if (cnt == 50) {
				cnt = 0;
				adns9800_debug_print();
			}
		}
		
    }
}

