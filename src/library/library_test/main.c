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

int main(void)
{
	int ret =aiMini4wdInitialize(AI_MINI_4WD_INIT_FLAG_USE_DEBUG_PRINT);
	if (ret != AI_OK) {
		while(1);
	}

	uint32_t threshold_mv = 0;
	uint16_t buf[1024];
	aiMini4wdSensorCalibrateTachoMeter(&threshold_mv, buf, sizeof(buf)/sizeof(buf[0]));

	while(1);

	return 0;
}
