/*
 * led_indicator.c
 *
 * Created: 2021/06/11 9:43:41
 * Author : kiyot
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

static void gpio_init(void)
{
	initialize_gpio(GPIO_PORTA, GPIO_PIN0, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTA, GPIO_PIN1, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTA, GPIO_PIN2, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTA, GPIO_PIN3, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTA, GPIO_PIN4, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTA, GPIO_PIN5, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTA, GPIO_PIN6, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTA, GPIO_PIN7, GPIO_OUT, GPIO_PUSH_PULL);

	initialize_gpio(GPIO_PORTC, GPIO_PIN5, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTC, GPIO_PIN6, GPIO_OUT, GPIO_PUSH_PULL);
	initialize_gpio(GPIO_PORTC, GPIO_PIN7, GPIO_OUT, GPIO_PUSH_PULL);

	gpio_output(GPIO_PORTA, GPIO_PIN0, 0);
	gpio_output(GPIO_PORTA, GPIO_PIN1, 0);
	gpio_output(GPIO_PORTA, GPIO_PIN2, 0);
	gpio_output(GPIO_PORTA, GPIO_PIN3, 0);
	gpio_output(GPIO_PORTA, GPIO_PIN4, 0);
	gpio_output(GPIO_PORTA, GPIO_PIN5, 0);
	gpio_output(GPIO_PORTA, GPIO_PIN6, 0);
	gpio_output(GPIO_PORTA, GPIO_PIN7, 0);

	gpio_output(GPIO_PORTC, GPIO_PIN5, 0);
	gpio_output(GPIO_PORTC, GPIO_PIN6, 0);
	gpio_output(GPIO_PORTC, GPIO_PIN7, 0);

	return;
}

static void _display_led(uint16_t num, uint8_t sep)
{
	if (sep != 0) {
		gpio_output(GPIO_PORTA, GPIO_PIN0, 1);
		gpio_output(GPIO_PORTC, GPIO_PIN5, 1);
	}
	else {
		gpio_output(GPIO_PORTA, GPIO_PIN0, 0);
		gpio_output(GPIO_PORTC, GPIO_PIN5, 0);
	}

	if (num & 0x0001) 
		gpio_output(GPIO_PORTA, GPIO_PIN1, 1);
	else
		gpio_output(GPIO_PORTA, GPIO_PIN1, 0);

	if (num & 0x0002)
		gpio_output(GPIO_PORTA, GPIO_PIN2, 1);
	else
		gpio_output(GPIO_PORTA, GPIO_PIN2, 0);
		
	if (num & 0x0004) 
		gpio_output(GPIO_PORTA, GPIO_PIN3, 1);
	else
		gpio_output(GPIO_PORTA, GPIO_PIN3, 0);

	if (num & 0x0008) 
		gpio_output(GPIO_PORTA, GPIO_PIN4, 1);
	else
		gpio_output(GPIO_PORTA, GPIO_PIN4, 0);

	if (num & 0x0010) 
		gpio_output(GPIO_PORTA, GPIO_PIN5, 1);
	else
		gpio_output(GPIO_PORTA, GPIO_PIN5, 0);

	if (num & 0x0020) 
		gpio_output(GPIO_PORTA, GPIO_PIN6, 1);
	else
		gpio_output(GPIO_PORTA, GPIO_PIN6, 0);

	if (num & 0x0040) 
		gpio_output(GPIO_PORTA, GPIO_PIN7, 1);
	else
		gpio_output(GPIO_PORTA, GPIO_PIN7, 0);

	if (num & 0x0080) 
		gpio_output(GPIO_PORTC, GPIO_PIN7, 1);
	else
		gpio_output(GPIO_PORTC, GPIO_PIN7, 0);

	if (num & 0x0100) 
		gpio_output(GPIO_PORTC, GPIO_PIN6, 1);
	else
		gpio_output(GPIO_PORTC, GPIO_PIN6, 0);

}


static void _sTimerCB(void)
{
}

static volatile uint8_t ndata = 0;
static uint8_t _i2cStartCb(uint8_t mode)
{
	if (mode == I2C_MASTER_READ) {
	}
	else if (mode == I2C_MASTER_WRITE) {
		ndata = 0;
	}

	return 0;
}

static uint8_t _i2cStopCb(void)
{
	return 0;
}

static volatile uint8_t sUpdateReq = 0;
static volatile uint16_t sValue = 0;
static uint8_t _masterRxCb(uint8_t ack)
{
	uint8_t data = 0;

	//	return sAddress;
	return data;
}

static uint8_t _masterTxCb(uint8_t data)
{
	if (ndata == 0) {
		sValue = (sValue & 0xff00) | data;
	}
	else if (ndata == 1){
		sValue = (sValue & 0x00ff) | (((uint16_t)data) << 8);
		sUpdateReq = 1;
	}
	
	ndata++;

	return 0;
}

extern uint8_t gI2cSlaveStatus;

int main(void)
{
	uint32_t systemClock = initialize_clock(CORE_SRC_RC32MHZ, NULL, NULL);

	gpio_init();
	
	int ret = initialize_tcc5_as_timerMilSec(10, systemClock);
	if (ret == TIMER_OK) {
		tcc5_timer_registerCallback(_sTimerCB);
	}

	Enable_Int();

	//J I2C Slave I/F‚ðÝ’è
	I2C_INIT_OPT i2c_opt = {
		.ownAddress  = 0x34,
		.startCB     = _i2cStartCb,
		.stopCB      = _i2cStopCb,
		.masterRxCB  = _masterRxCb,
		.masterTxCB  = _masterTxCb
	};
	initialize_i2c(400000, systemClock, I2C_SLAVE, &i2c_opt);

	while (1) {
		if (sUpdateReq) {
			_display_led(sValue & 0x7fff, (sValue & 0x8000) != 0 ? 1 : 0);
		}
	}
}

