/*
 * led_driver.c
 *
 * Created: 2021/07/01 8:28:08
 *  Author: kiyot
 */ 


#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

#include <samd51_error.h>
#include <samd51_gpio.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static const SAMD51_GPIO_PORT sGpioPins[] = 
{
	SAMD51_GPIO_B04,
	SAMD51_GPIO_B05,
	SAMD51_GPIO_B06,
	SAMD51_GPIO_B07,
	SAMD51_GPIO_B08,
	SAMD51_GPIO_B09,
	SAMD51_GPIO_A04,
	SAMD51_GPIO_A05,
};


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int ledSetPattern(uint8_t pattern)
{
	for (int i=0 ; i<8 ; ++i) {
		if (pattern & (1<<i)) {
			samd51_gpio_output(sGpioPins[i], 1);
		}
		else {
			samd51_gpio_output(sGpioPins[i], 0);
		}
	}
	
	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int ledTogglePattern(uint8_t pattern)
{
	for (int i=0 ; i<8 ; ++i) {
		if (pattern & (1<<i)) {
			samd51_gpio_output_toggle(sGpioPins[i]);
		}
	}
	
	return AI_OK;
}