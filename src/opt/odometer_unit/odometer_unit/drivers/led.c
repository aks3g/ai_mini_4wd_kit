/*
 * led.c
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */
#include <stdint.h>

#include <gpio_driver.h>

#include "drivers/led.h"

void led_set(LED_IDX idx, uint8_t on)
{
	if (idx == LED0) {
		gpio_output(GPIO_PORTD, GPIO_PIN4, (on ? 0 : 1));
	}
	
	if (idx == LED1) {
		gpio_output(GPIO_PORTD, GPIO_PIN5, (on ? 0 : 1));
	}

	return;
}

