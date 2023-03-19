/*
 * console_cmd.h
 *
 * Created: 2020/06/21 5:25:52
 *  Author: kiyot
 */ 


#ifndef CONSOLE_CMD_H_
#define CONSOLE_CMD_H_


typedef void (*console_cmd)(char **argv, int argn);
typedef struct console_cmd_def_t
{
	char *name;
	char *help;
	console_cmd cmd;
} console_cmd_def;

#define NUM_CMDS	(32)
extern console_cmd_def console_cmd_set[NUM_CMDS];

#endif /* CONSOLE_CMD_H_ */