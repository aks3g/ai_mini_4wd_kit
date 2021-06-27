/*
 * console_internal.h
 *
 * Created: 2021/06/28 5:28:39
 *  Author: kiyot
 */ 
#include "../console.h"

#ifndef CONSOLE_INTERNAL_H_
#define CONSOLE_INTERNAL_H_


typedef struct ConsoleContext_t
{
	ConsoleCommand *cmdlist;

	ConsolePutcFunc putcfunc;
	char line[32];
	int wptr;

} ConsoleContext;

extern ConsoleContext gConsoleCtx;


#endif /* CONSOLE_INTERNAL_H_ */