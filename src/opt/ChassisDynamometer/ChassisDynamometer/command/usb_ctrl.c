/*
 * usb_ctrl.c
 *
 * Created: 2021/08/04 15:49:48
 *  Author: kiyot
 */ 

#include <stddef.h>
#include <string.h>
#include <stdint.h>

#include <console.h>

#include <samd51_error.h>
#include <samd51_gpio.h>
static int sPowerEn = 1;

static int _usb_ctrl_function(const int argc, const char **argv)
{
	if (argc >= 1) {
		if (0 == strcmp(argv[0], "on")){
			sPowerEn = 1;
			samd51_gpio_output(SAMD51_GPIO_A21, 0);
		}
		else if (0 == strcmp(argv[0], "off")){
			sPowerEn = 0;
			samd51_gpio_output(SAMD51_GPIO_A21, 1);
		}
		else {
			consolePrintf("Invalid argument : %s\n", argv[0]);
			return;
		}

		consolePrintf("Change USB %s\n", (sPowerEn != 0) ? "ENABLE" : "DISABLE");
	}
	else {
		consolePrintf("CURRENT USB STATE = %s\n", (sPowerEn != 0) ? "ENABLE" : "DISABLE");
	}

	return 0;
}


static const char *_usb_ctrl_help(void)
{
	return "usb - Control USB Vbus on/off.";
}

ConsoleCommand g_usb_ctrl_cmd =
{
	.name = "usb",
	.func = _usb_ctrl_function,
	.help = _usb_ctrl_help,
	.link = NULL
};
