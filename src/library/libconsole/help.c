/*
 * help.c
 *
 * Created: 2021/06/28 6:23:25
 *  Author: kiyot
 */ 
#include <stddef.h>
#include <string.h>

#include "include/internal/console_internal.h"
#include "include/console.h"

static int _help_function(const int argc, const char **argv)
{
	if (argc == 0) {
		consolePuts("List of commands:\n");
		ConsoleCommand *ptr = gConsoleCtx.cmdlist;
		while (ptr) {
			if (ptr->name != NULL) {
				consolePuts("  ");
				consolePuts(ptr->name);
				consolePutc('\n');
			}
			
			ptr = ptr->link;
		}
	}
	else {
		ConsoleCommand *ptr = gConsoleCtx.cmdlist;
		while (ptr) {
			if (ptr->name != NULL && 0 == strncmp(argv[0], ptr->name, strlen(ptr->name))) {
				consolePuts(ptr->help());
				consolePutc('\n');
			}

			ptr = ptr->link;
		}
	}

	return 0;
}


static const char *_help_help(void)
{
	return "help - Show command help\n"
		   "  help : show all installed command.\n"
		   "  help <cmd> : show help of <cmd>";
}


ConsoleCommand g_help_cmd =
{
	.name = "help",
	.func = _help_function,
	.help = _help_help,
	.link = NULL
};
