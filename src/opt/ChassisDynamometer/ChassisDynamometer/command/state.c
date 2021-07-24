/*
 * state.c
 *
 * Created: 2021/06/29 6:20:54
 *  Author: kiyot
 */ 
#include <stddef.h>
#include <string.h>

#include <console.h>

static int _state_function(const int argc, const char **argv)
{
	consolePrintf("State of this board\n");


	return 0;
}


static const char *_state_help(void)
{
	return "state - display the state of this board.";
}

ConsoleCommand g_state_cmd =
{
	.name = "state",
	.func = _state_function,
	.help = _state_help,
	.link = NULL
};
