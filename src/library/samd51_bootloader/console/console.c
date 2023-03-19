/*
 * console.c
 *
 * Created: 2020/06/20 22:25:20
 *  Author: kiyot
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ai_mini4wd.h>

#include "console_cmd.h"


char linebuf[32] = {0};
int  wptr = 0;

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

static char *skip_white_space(char *str)
{
	while (*str == ' ') str++;

	return str;
}



static void _console_cmd_exec(char *line)
{
	int   argn = 0;
	char *argv[8];
	char *cmd = line;
	char *args = replace_next_space_to_null(cmd);
	args = skip_white_space(args);

	memset (argv, 0, sizeof(argv));

	while (args != NULL) {
		argv[argn++] = args;
		args = replace_next_space_to_null(args);
		args = skip_white_space(args);
	}

	for (int i=0 ; i<NUM_CMDS ; ++i) {
		if (console_cmd_set[i].name != NULL && 0 == strcmp(cmd, console_cmd_set[i].name)) {
			console_cmd_set[i].cmd(argv, argn);
			return;
		}		
	}

	aiMini4wdDebugPrintf ("%s : Command was not found.\n", cmd);

	return;
}



void console_update(char c)
{
	if (c == 0x08) {
		if (wptr > 0) {
			aiMini4wdDebugPutc(8);
			aiMini4wdDebugPutc(' ');
			aiMini4wdDebugPutc(8);
			linebuf[wptr] = 0;
			wptr--;
		}

		return;
	}
	else {
		aiMini4wdDebugPutc(c);	
	}

	if (c == '\n') {
		if (wptr) _console_cmd_exec(linebuf);

		aiMini4wdDebugPutc('>');
		aiMini4wdDebugPutc(' ');
		memset (linebuf, 0, sizeof(linebuf));
		wptr = 0;
	}
	else if (wptr < sizeof(linebuf)) {
		linebuf[wptr] = c;
		wptr++;
	}

	return;
}
