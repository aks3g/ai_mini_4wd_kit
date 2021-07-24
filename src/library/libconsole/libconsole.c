/*
 * libconsole.c
 *
 * Created: 2021/06/27 8:28:35
 *  Author: kiyot
 */ 
#include <stddef.h>
#include <string.h>

#include "include/internal/console_internal.h"
#include "include/console.h"


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
ConsoleContext gConsoleCtx;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
extern ConsoleCommand g_echo_cmd;
extern ConsoleCommand g_help_cmd;

/*---------------------------------------------------------------------------*/
static char *replace_next_space_to_null(char *str);
static char *skip_white_space(char *str);


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
int consoleInitialize(ConsolePutcFunc putc_func)
{
	memset(&gConsoleCtx, 0, sizeof(gConsoleCtx));
	
	gConsoleCtx.putcfunc = putc_func;
	
	consoleInstallCommand(&g_help_cmd);
	consoleInstallCommand(&g_echo_cmd);

	return CONSOLE_ERROR_OK;
}

/*---------------------------------------------------------------------------*/
int consoleInstallCommand(ConsoleCommand *cmd)
{
	if (cmd == NULL || cmd->name == NULL) {
		return CONSOLE_ERROR_NULL;
	}
	
	cmd->link = gConsoleCtx.cmdlist;
	gConsoleCtx.cmdlist = cmd;

	return CONSOLE_ERROR_OK;
}

/*---------------------------------------------------------------------------*/
void consoleUpdate(char c)
{
	if (c == 0x08) {
		if (gConsoleCtx.wptr > 0) {
			consolePuts("\b \b");
			gConsoleCtx.line[gConsoleCtx.wptr] = 0;
			gConsoleCtx.wptr--;
		}

		return;
	}
	else {
		consolePutc(c);
	}

	if (c == '\n') {
		if (gConsoleCtx.wptr) consoleExecute(gConsoleCtx.line);

		consolePuts("> ");

		memset (gConsoleCtx.line, 0, sizeof(gConsoleCtx.line));
		gConsoleCtx.wptr = 0;
	}
	else if (gConsoleCtx.wptr < sizeof(gConsoleCtx.line)) {
		gConsoleCtx.line[gConsoleCtx.wptr++] = c;
	}

	return;
}


/*---------------------------------------------------------------------------*/
int consoleExecute(char *line)
{
	int   argn = 0;
	char *argv[MAX_ARG_CNT];
	char *cmd = line;
	char *args = replace_next_space_to_null(cmd);
	args = skip_white_space(args);

	memset (argv, 0, sizeof(argv));

	while (args != NULL) {
		argv[argn++] = args;
		args = replace_next_space_to_null(args);
		args = skip_white_space(args);
	}

	int ret = 0;
	ConsoleCommand *ptr = gConsoleCtx.cmdlist;
	while (ptr != NULL) {
		if (ptr->name != NULL && 0 == strcmp(cmd, ptr->name)) {
			ret = ptr->func(argn, (const char **)argv);
			consolePutc('\n');
			break;
		}
		
		ptr = ptr->link;
	}

	if (ptr == NULL) {
		consolePrintf ("%s : Command was not found.\n", cmd);
		ret = CONSOLE_ERROR_COMMAND_NOTFOUND;
	}

	return ret;
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static char *replace_next_space_to_null(char *str)
{
	while (*str != '\0') {
		if (*str == ' ') {
			*str = '\0';
			return (str + 1);
		}
		str++;
	}

	return NULL;
}

/*---------------------------------------------------------------------------*/
static char *skip_white_space(char *str)
{
	if (str == NULL) return str;
	while (*str == ' ') str++;

	return str;
}


