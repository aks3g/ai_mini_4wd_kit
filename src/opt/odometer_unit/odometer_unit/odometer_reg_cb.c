/*
 * odometer_reg_cb.c
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdint.h>
#include <string.h>

#include "drivers/led.h"
#include "drivers/adns9800.h"

void on_write_reg_reset(uint8_t val)
{
	adns9800_reset(val);
}

void on_write_reg_srom_version(uint8_t val)
{
	adns9800_switch_srom(val);
}

void on_write_reg_led(uint8_t val)
{
	if (val & 0x01) {
		led_set(LED0, 1);
	}
	else {
		led_set(LED0, 0);
	}
	
	if (val & 0x02) {
		led_set(LED1, 1);
	}
	else {
		led_set(LED1, 0);
	}
}

void on_write_reg_debug_print_enable(uint8_t val)
{
	adns9800_enable_debug_print(val);
}

static uint8_t sCpiLower = 200;
void on_write_cpi_l(uint8_t val)
{
	sCpiLower = val;
}

void on_write_cpi_h(uint8_t val)
{
	uint16_t cpi = ((uint16_t)val) << 8 |  sCpiLower;

	adns9800_set_cpi(cpi);
}