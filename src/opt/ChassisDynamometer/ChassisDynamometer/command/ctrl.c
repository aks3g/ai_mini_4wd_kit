/*
 * ctrl.c
 *
 * Created: 2021/06/30 9:46:27
 *  Author: kiyot
 */ 
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <console.h>
#include "drivers/dynamo.h"

static int _ctrl_function(const int argc, const char **argv)
{
	if (argc != 3) {
		consolePuts("Argument Error.\n");
	}
	
	int ch  = strtol(argv[0], NULL, 10);
	int val = strtol(argv[2], NULL, 10);
	
	consolePrintf("Set Ch%d duty = %d\n", ch, val);
	
	dynamoSetDuty(ch, val);
	
	return 0;
}


static const char *_ctrl_help(void)
{
	return "ctrl - control the dynamos\n"
			"  ctrl <ch> [d|c|t] value(0 - 255)\n"
			"   <ch>    : channel. select 0 - 3\n"
			"   [d|c|t] : d - with duty, c - with current, t - with torque\n"
			"   value   : control/target value";
}


ConsoleCommand g_ctrl_cmd =
{
	.name = "ctrl",
	.func = _ctrl_function,
	.help = _ctrl_help,
	.link = NULL
};
