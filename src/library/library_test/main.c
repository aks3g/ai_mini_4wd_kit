/*
 * library_test.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <ai_mini4wd.h>
#include <ai_mini4wd_error.h>
#include <ai_mini4wd_sensor.h>
#include <ai_mini4wd_timer.h>

static volatile int sFlag = 0;
static void _100ms_cb(void){
	sFlag = 1;
}

int main(void)
{
	int ret =aiMini4wdInitialize(AI_MINI_4WD_INIT_FLAG_USE_DEBUG_PRINT | AI_MINI_4WD_INIT_FLAG_USE_USB_SERIAL | AI_MINI_4WD_INIT_FLAG_USE_TEST_TYPE_HW);
	if (ret != AI_OK) {
		while(1);
	}

	aiMini4WdTimerRegister100msCallback(_100ms_cb);

	while(1) {
		if (sFlag) {
			sFlag = 0;
			aiMini4wdDebugPrintf("Library Test\n");
		}
	}

	return 0;
}
