/*
 * debug_trace.c
 *
 * Created: 2023/05/07 5:46:15
 *  Author: kiyot
 */
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

#include "ai_mini4wd_trace.h"

#define TRACE_NUM		((0x2000 - 8) / 8)
typedef struct {
	uint32_t enabled;
	uint32_t idx;
	struct {
		uint32_t tick;
		uint32_t log;
	} trace [TRACE_NUM];
} DEBUG_TRACE;

DEBUG_TRACE sDebugTrace __attribute__ ((section (".bkupram"))) ;

void aiMini4wdDebugTracePush(uint32_t tick, uint32_t log)
{
	if (sDebugTrace.enabled == 0) return;
	
	if (sDebugTrace.idx >= TRACE_NUM) sDebugTrace.idx=0;
	
	sDebugTrace.trace[sDebugTrace.idx].log  = log;
	sDebugTrace.trace[sDebugTrace.idx].tick = tick;
	
	sDebugTrace.idx = (sDebugTrace.idx + 1) % TRACE_NUM;
	
	return;
}

void aiMini4wdDebugTraceClear(void)
{
	memset(&sDebugTrace, 0xff, sizeof(sDebugTrace));
	sDebugTrace.enabled = 0;
	sDebugTrace.idx = 0;
}

void aiMini4wdDebugTraceControl(uint32_t enable)
{
	sDebugTrace.enabled = enable;
}

void aiMini4wdDebugTracePop(AiMini4wdDebugTraceGet getter)
{
	uint32_t saved_enable = sDebugTrace.enabled;
	sDebugTrace.enabled = 0;
	
	for (uint32_t i=0 ; i < TRACE_NUM ; i++) {
		getter(i, sDebugTrace.trace[i].tick, sDebugTrace.trace[i].log);
	}
	
	sDebugTrace.enabled = saved_enable;
}
