/*
 * sensor.c
 *
 * Created: 2021/07/24 13:22:28
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <console.h>
#include "drivers/usb_cdc.h"
#include "drivers/dynamo.h"
#include "drivers/ads131m08.h"

static volatile int sUpdated = 0;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _updated(void)
{
	sUpdated = 1;
}

/*--------------------------------------------------------------------------*/
static int _sensor_function(const int argc, const char **argv)
{	
	dynamoRegisterCapturedCallback(_updated);

    while (1) {
	    char c = '\0';
	    //Break with Ctrl+D
	    if (0 == usbCdc_try_rx((uint8_t *)&c) && c == 4) break;

	    if (sUpdated) {
		    sUpdated = 0;
			consolePrintf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", 
						dynamoGetReverseEmf_mV(0),
						dynamoGetReverseEmf_mV(1),
						dynamoGetReverseEmf_mV(2),
						dynamoGetReverseEmf_mV(3),
						dynamoGetCurrent_mA(0),
						dynamoGetCurrent_mA(1),
						dynamoGetCurrent_mA(2),
						dynamoGetCurrent_mA(3)
						);
		}
	}
	dynamoRegisterCapturedCallback(NULL);

	return 0;
}

static const char *_sensor_help(void)
{
	return "sensor : Output sensor value.";
}

ConsoleCommand g_sensor_cmd =
{
	.name = "sensor",
	.func = _sensor_function,
	.help = _sensor_help,
	.link = NULL
};
