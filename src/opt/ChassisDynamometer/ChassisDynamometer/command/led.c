/*
 * led.c
 *
 * Created: 2021/07/01 8:26:06
 *  Author: kiyot
 */ 
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <console.h>
#include "drivers/led_driver.h"

static int _led_function(const int argc, const char **argv)
{
	if (argc != 1) {
		consolePuts("Argument Error.\n");
	}
	
	uint8_t val = (uint8_t)strtoul(argv[0], NULL, 16);
	
	ledSetPattern(val);
	
	return 0;
}


static const char *_led_help(void)
{
	return "led - control LEDs on/off\n"
	"  led value(0x00 - 0xff)";
}


ConsoleCommand g_led_cmd =
{
	.name = "led",
	.func = _led_function,
	.help = _led_help,
	.link = NULL
};
