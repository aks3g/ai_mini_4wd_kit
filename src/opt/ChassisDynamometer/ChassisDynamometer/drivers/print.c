/*
 * print.c
 *
 * Created: 2020/09/21 6:37:19
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

static char sCommonLineBuf[512];
int cd_printf(const char *format, ...)
{
	va_list ap;

	va_start( ap, format );
	int len = vsnprintf(sCommonLineBuf, sizeof(sCommonLineBuf), format, ap );
	va_end( ap );

	usbCdc_puts(sCommonLineBuf);
	
	return len;
}
