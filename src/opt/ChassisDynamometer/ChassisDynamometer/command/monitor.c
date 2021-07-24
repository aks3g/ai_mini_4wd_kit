/*
 * monitor.c
 *
 * Created: 2021/07/06 14:46:37
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

#define CONSOLE_WIDTH			80
#define CONSOLE_HEIGHT			24

#define MAX_SYMS			(60)
#define ORANGE_ZONE_IDX		(30)
#define RED_ZONE_IDX		(50)
static void _print_bar(uint32_t raw, uint32_t col, int32_t value, int32_t max)
{
	consolePrintf("\e[%d;%dH", raw, col);
	
	int num_sym = (MAX_SYMS * value) / max;
	if (num_sym > MAX_SYMS) {
		num_sym = MAX_SYMS;
	}

	consolePuts("\e[42m");
	for (int i=0 ; i<num_sym ; ++i) {
		if (i == ORANGE_ZONE_IDX) 	consolePuts("\e[43m");
		if (i == RED_ZONE_IDX) 		consolePuts("\e[41m");
		consolePutc(' ');
	}
	consolePuts("\e[40m\e[0K");
}

static void _print_val(uint32_t raw, uint32_t col, int32_t value)
{
	consolePrintf("\e[%d;%dH        ", raw, col);
	consolePrintf("\e[%d;%dH", raw, col);

	consolePrintf("%8d", value);

}

static void _print_valf(uint32_t raw, uint32_t col, float value)
{
	consolePrintf("\e[%d;%dH        ", raw, col);
	consolePrintf("\e[%d;%dH", raw, col);

	consolePrintf("%8.1f", value);

}

static void _move_curs_default(void)
{
	consolePrintf("\e[24;0H");
}

static void _print_frame(void)
{
	consolePuts("\e[2J");
	consolePuts(
		"--- Dynamo 0------------------------------------------------------------------\n"
		" v=        [km/h]|\n"
		"Er=        [mV]  |\n"
		" I=        [mA]  |\n"
		" T=        [Nm]  |\n"
		"--- Dynamo 1------------------------------------------------------------------\n"
		" v=        [km/h]|\n"
		"Er=        [mV]  |\n"
		" I=        [mA]  |\n"
		" T=        [Nm]  |\n"
		"--- Dynamo 2------------------------------------------------------------------\n"
		" v=        [km/h]|\n"
		"Er=        [mV]  |\n"
		" I=        [mA]  |\n"
		" T=        [Nm]  |\n"
		"--- Dynamo 3------------------------------------------------------------------\n"
		" v=        [km/h]|\n"
		"Er=        [mV]  |\n"
		" I=        [mA]  |\n"
		" T=        [Nm]  |\n"
		"Battery Voltage =     [mV]\n"
		"\n"
		"(Exit : Ctrl+D, Reflesh Screen: c)\n"
	);
	
	return;
}

volatile int sUpdated = 0;
void _updated(void)
{
	sUpdated = 1;
}

static int _monitor_function(const int argc, const char **argv)
{
	dynamoRegisterCapturedCallback(_updated);

	_print_frame();

    while (1) {
	    char c = '\0';
		//Break with Ctrl+D
	    if (0 == usbCdc_try_rx((uint8_t *)&c) && c == 4) break;

		if (sUpdated) {
			sUpdated = 0;

			float v[4];
			for (int i=0 ; i<4 ; ++i){
				v[i] = dynamoGetVelocity_kmph(i);
			}

			_print_valf( 2, 3, v[0]);
			_print_valf( 7, 3, v[1]);
			_print_valf(12, 3, v[2]);
			_print_valf(17, 3, v[3]);

			_print_bar( 2, 19, v[0], 40);
			_print_bar( 7, 19, v[1], 40);
			_print_bar(12, 19, v[2], 40);
			_print_bar(17, 19, v[3], 40);

			_print_val( 3, 3, dynamoGetReverseEmf_mV(0));
			_print_val( 8, 3, dynamoGetReverseEmf_mV(1));
			_print_val(13, 3, dynamoGetReverseEmf_mV(2));
			_print_val(18, 3, dynamoGetReverseEmf_mV(3));

			_print_val( 4, 3, dynamoGetCurrent_mA(0));
			_print_val( 9, 3, dynamoGetCurrent_mA(1));
			_print_val(14, 3, dynamoGetCurrent_mA(2));
			_print_val(19, 3, dynamoGetCurrent_mA(3));
			
			_print_bar( 3, 19, dynamoGetReverseEmf_mV(0), 500);
			_print_bar( 8, 19, dynamoGetReverseEmf_mV(1), 500);
			_print_bar(13, 19, dynamoGetReverseEmf_mV(2), 500);
			_print_bar(18, 19, dynamoGetReverseEmf_mV(3), 500);

			_print_bar( 4, 19, dynamoGetCurrent_mA(0), 2000);
			_print_bar( 9, 19, dynamoGetCurrent_mA(1), 2000);
			_print_bar(14, 19, dynamoGetCurrent_mA(2), 2000);
			_print_bar(19, 19, dynamoGetCurrent_mA(3), 2000);
			
			_move_curs_default();
		}
    }
	consolePuts("\e[2J");

	return 0;
}

static const char *_monitor_help(void)
{
	return "monitor - Monitor station.";
}

ConsoleCommand g_monitor_cmd =
{
	.name = "monitor",
	.func = _monitor_function,
	.help = _monitor_help,
	.link = NULL
};

