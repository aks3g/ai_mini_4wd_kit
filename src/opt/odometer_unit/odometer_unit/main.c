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

static volatile uint8_t sAddress = 0;
static volatile uint8_t sAddressSelected = 0; 
static uint8_t _i2cStartCb(uint8_t mode)
{
	if (mode == I2C_MASTER_READ) {
	}
	else if (mode == I2C_MASTER_WRITE) {
		sAddressSelected = 0;
	}

	return 0;
}

static uint8_t _i2cStopCb(void)
{
	return 0;
}

static uint8_t _masterRxCb(uint8_t ack)
{
	uint8_t data = 0;

	// Register Read
	reg_read(sAddress, &data);
	sAddress = (sAddress + 1);

//	return sAddress;
	return data;
}

static uint8_t _masterTxCb(uint8_t data)
{
	if (sAddressSelected == 0) {
		sAddressSelected = 1;
		sAddress = data;

		if (sAddress == REG_DELTAX_0) {
			reg_update_delta();
		}
	}
	else {
		//Register Write
		reg_write(sAddress, data);
		sAddress = (sAddress + 1);
	}

	return 0;
}

extern uint8_t gI2cSlaveStatus;

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

	//J I2C Slave I/Fを設定
	I2C_INIT_OPT i2c_opt = {
		.ownAddress  = 0x33,
		.startCB     = _i2cStartCb,
		.stopCB      = _i2cStopCb,
		.masterRxCB  = _masterRxCb,
		.masterTxCB  = _masterTxCb
	};
	initialize_i2c(400000, systemClock, I2C_SLAVE, &i2c_opt);

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
		}		
    }
}

