/*
 * console_cmd.h
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
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

#define NUM_CMDS	(10)
extern console_cmd_def console_cmd_set[NUM_CMDS];

#endif /* CONSOLE_CMD_H_ */