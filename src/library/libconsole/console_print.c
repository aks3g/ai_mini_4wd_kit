/*
 * console_print.c
 *
 * Created: 2021/06/27 9:06:25
 *  Author: kiyot
 */ 
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

#include "include/internal/console_internal.h"
#include "include/console.h"

int consolePrintf(const char *format, ...)
{
	static char buf[512];
	va_list ap;

	if (gConsoleCtx.putcfunc == NULL) {
		return CONSOLE_ERROR_NOPUTFUNC;
	}

	va_start( ap, format );
	int len = vsnprintf(buf, sizeof(buf), format, ap );
	va_end( ap );

	if (sizeof(buf) == len) {
		len = -1;
	}

	if (len > 0) {
		consolePuts((const char *)buf);
	}

	return len;
}


int consolePuts(const char *str)
{
	if (gConsoleCtx.putcfunc == NULL) {
		return CONSOLE_ERROR_NOPUTFUNC;
	}

	size_t len = strlen(str);
	for (int i=0 ; i<len ; ++i, str++) {
		gConsoleCtx.putcfunc(*str);
	}
	
	return CONSOLE_ERROR_OK;
}


int consolePutc(const char c)
{
	if (gConsoleCtx.putcfunc == NULL) {
		return CONSOLE_ERROR_NOPUTFUNC;
	}
	
	gConsoleCtx.putcfunc(c);

	return CONSOLE_ERROR_OK;
}

