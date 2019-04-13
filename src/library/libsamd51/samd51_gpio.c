/*
 * samd51_gpio.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <sam.h>

#include <stdint.h>

#include <samd51_error.h>
#include <samd51_gpio.h>


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_gpio_configure(SAMD51_GPIO_PORT port, SAMD51_GPIO_DIR dir, SAMD51_GPIO_PULL pull, SAMD51_GPIO_PORTMUX mux)
{
	uint32_t portnum = (port & 0xffff);
	uint32_t hwsel = (portnum >= 16) ? (1<<31) : 0;
	uint32_t bitmask = 1 << ((uint32_t)port & 0xffff);
	const uint32_t wrpincfg = (1 << 30);
	uint32_t inen = 0;
	uint32_t pmuxen = (mux != SAMD51_GPIO_MUX_DEFAULT) ? (1 << 16) : 0;
	uint32_t wrpmux = (pmuxen) ? (1 << 28) : 0;
	uint32_t pmux = (pmuxen) ? (mux&0xf) << 24 : 0;

	if ((port & 0x000F0000) == 0x000a0000) {
		if (dir == SAMD51_GPIO_IN) {
			REG_PORT_DIRCLR0 = bitmask;
			inen = (1 << 17);			
		}
		else if (dir == SAMD51_GPIO_OUT) {
			REG_PORT_DIRSET0 = bitmask;
		}
		else if (dir == SAMD51_GPIO_INOUT) {
			REG_PORT_DIRSET0 = bitmask;
			inen = (1 << 17);
		}
		else {
			REG_PORT_DIRCLR0 = bitmask;
		}

		volatile uint32_t cfg  =  hwsel | wrpincfg | wrpmux | pmux |  pull | inen | pmuxen | (hwsel ? ((bitmask>>16) & 0xffff) : (bitmask & 0xffff) );
		REG_PORT_WRCONFIG0 = cfg;
	}
	else if ((port & 0x000F0000) == 0x000b0000) {
		if (dir == SAMD51_GPIO_IN) {
			REG_PORT_DIRCLR1 = bitmask;
			inen = (1 << 17);
		}
		else if (dir == SAMD51_GPIO_OUT) {
			REG_PORT_DIRSET1 = bitmask;
		}
		else if (dir == SAMD51_GPIO_INOUT) {
			REG_PORT_DIRSET1 = bitmask;
			inen = (1 << 17);
		}
		else {
			REG_PORT_DIRCLR1 = bitmask;
		}

		volatile uint32_t cfg  =  hwsel | wrpincfg | wrpmux | pmux |  pull | inen | pmuxen | (hwsel ? ((bitmask>>16) & 0xffff) : (bitmask & 0xffff) );
		REG_PORT_WRCONFIG1 = cfg;
	}
	else {
		return AI_ERROR_NODEV;
	}

	return AI_OK;	
}

/*--------------------------------------------------------------------------*/
int samd51_gpio_input(SAMD51_GPIO_PORT port)
{
	uint32_t bitmask = 1 << ((uint32_t)port & 0xffff);

	if ((port & 0x000F0000) == 0x000a0000) {
		return (REG_PORT_IN0 & bitmask) ? 1 : 0;
	}
	else if ((port & 0x000F0000) == 0x000b0000) {
		return (REG_PORT_IN1 & bitmask) ? 1 : 0;
	}
	else {
		return AI_ERROR_NODEV;
	}
}

/*--------------------------------------------------------------------------*/
int samd51_gpio_output(SAMD51_GPIO_PORT port, uint8_t out)
{
	uint32_t bitmask = 1 << ((uint32_t)port & 0xffff);

	if ((port & 0x000F0000) == 0x000a0000) {
		if (out) {
			REG_PORT_OUTSET0 = bitmask;
		}
		else {
			REG_PORT_OUTCLR0 = bitmask;
		}
	}
	else if ((port & 0x000F0000) == 0x000b0000) {
		if (out) {
			REG_PORT_OUTSET1 = bitmask;
		}
		else {
			REG_PORT_OUTCLR1 = bitmask;
		}
	}
	else {
		return AI_ERROR_NODEV;
	}

	
	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_gpio_output_toggle(SAMD51_GPIO_PORT port)
{
	uint32_t bitmask = 1 << ((uint32_t)port & 0xffff);
	
	if ((port & 0x000F0000) == 0x000a0000) {
		REG_PORT_OUTTGL0 = bitmask;
	}
	else if ((port & 0x000F0000) == 0x000b0000) {
		REG_PORT_OUTTGL1 = bitmask;
	}
	else {
		return AI_ERROR_NODEV;
	}
	
	return AI_OK;
}