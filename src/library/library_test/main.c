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
#include <ai_mini4wd_hid.h>

static volatile int sFlag = 0;
static void _100ms_cb(void){
	sFlag = 1;
}

static volatile int x = 0;
static volatile uint32_t idx = 0;
static volatile int32_t deltax[1024] = {0};
static volatile int32_t deltay[1024] = {0};
static void _sensor_callback(AiMini4wdSensorData *data)
{
	(void)data;
	x = 1 - x;

	if (idx < 1024) {
		deltax[idx  ] = data->odometry.delta_x_mm;
		deltay[idx++] = data->odometry.delta_y_mm;
	}
}


int testFuncA(int x) 
{
	volatile int z = 1;
	
	return x + z;
}


int testFuncB(int x)
{
	volatile int y = 4;
	x = y + x;
	
	return testFuncA(x);
}


int testFuncC(int x)
{
	volatile int y = 4;
	x = y + x;
	
	return testFuncB(x);
}



int main(void)
{
	int x = testFuncC(12);
	
	
	int ret =aiMini4wdInitialize(AI_MINI_4WD_INIT_FLAG_USE_ODOMETER | AI_MINI_4WD_INIT_FLAG_USE_TEST_TYPE_HW);
	if (ret != AI_OK) {
		while(1);
	}

	aiMini4wdSensorSetTachometerThreshold(1800,0);

	aiMini4WdTimerRegister100msCallback(_100ms_cb);
	aiMini4wdSensorRegisterCapturedCallback(_sensor_callback);
	
	//aiMini4wdMotorDriverDrive(128);
	
	while(1) {
		if (sFlag) {
			sFlag = 0;
		}
		
		if (idx >= 1024) {
			x ++;
		}
	}

	return 0;
}
