/*
 * echo.c
 *
 * Created: 2021/06/28 6:23:12
 *  Author: kiyot
 */ 
#include <stddef.h>
#include <string.h>

#include "include/console.h"

static int _echo_function(const int argc, const char **argv)
{
	for (int i=0 ; i<argc ; ++i) {
		consolePuts(argv[i]);
		consolePutc(' ');
	}

	return 0;
}


static const char *_echo_help(void)
{
	return "echo - display a line of text";	
}

ConsoleCommand g_echo_cmd =
{
	.name = "echo",
	.func = _echo_function,
	.help = _echo_help,
	.link = NULL
};
