/*
 * ff_rtc.c
 *
 * Created: 2023/03/26 7:39:07
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <stddef.h>
#include <time.h>

#include <samd51_error.h>
#include <samd51_sercom.h>
#include <samd51_i2c.h>

#include "internal/ff.h"
#include "internal/ffconf.h"
#include "internal/rtc.h"

#include "ai_mini4wd.h"
#include "ai_mini4wd_timer.h"

DWORD get_fattime (void)
{
	uint32_t epoc = aiMini4wdRtcGetTimer();
	struct tm *t = localtime((time_t *)&epoc);

	return ((DWORD)((t->tm_year + 1900) - 1980) << 25) | ((DWORD)(t->tm_mon+1)) << 21 | ((DWORD)t->tm_mday) << 16 | ((DWORD)t->tm_hour) << 11 | ((DWORD)t->tm_mon) << 5 | ((DWORD)t->tm_sec)/2;
}
