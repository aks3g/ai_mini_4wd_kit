/*
 * rtc.c
 *
 * Created: 2023/03/25 8:16:27
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <stddef.h>

#include <samd51_error.h>
#include <samd51_sercom.h>
#include <samd51_i2c.h>

#include "include/ai_mini4wd.h"
#include "include/internal/rtc.h"

#include "include/ai_mini4wd_timer.h"

#include "include/internal/max31329.h"

uint32_t sRtcTick = 0;

int aiMini4wdInitializeRtc(SAMD51_SERCOM sercom)
{
	return max31329_probe(sercom, &sRtcTick);	
}

uint32_t aiMini4wdRtcGetTimer(void)
{
	return sRtcTick;
}

void aiMini4wdRtcSetTimer(uint32_t epoc)
{
	sRtcTick = epoc;
	struct tm *t = localtime((time_t *)&epoc);
	
	int ret = max31329_set_time(t);
	(void)ret;

	return;
}

void aiMini4wdRtcGetLocaltime(struct AiMini4wdTm *ltime)
{
	if (ltime == NULL) return;
	
	struct tm *t = localtime((time_t *)&sRtcTick);
	
	ltime->tm_hour  = t->tm_hour;
	ltime->tm_min   = t->tm_min;
	ltime->tm_sec   = t->tm_sec;
	ltime->tm_year  = t->tm_year;
	ltime->tm_mon   = t->tm_mon;
	ltime->tm_mday  = t->tm_mday;
	ltime->tm_wday  = t->tm_wday;
	ltime->tm_yday  = t->tm_yday;
	ltime->tm_isdst = t->tm_isdst;

	return;
}


void aiMIni4wdRtcCycle1sec(void)
{
	sRtcTick++;
}