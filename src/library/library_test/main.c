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
#include <ai_mini4wd_hid.h>
#include <ai_mini4wd_timer.h>
#include <ai_mini4wd_sensor.h>
#include <ai_mini4wd_motor_driver.h>
#include <ai_mini4wd_fs.h>

static volatile int sLogEnabled = 1;

static void _sw0_cb(int status) {
	if (status) {
		aiMini4wdSetLedPattern(0x01);
		aiMini4wdMotorDriverDrive(0);
		sLogEnabled = 0;
	}
	else {
		aiMini4wdClearLedPattern(0x01);
	}
}

static void _sw1_cb(int status) {
	if (status) {
		aiMini4wdSetLedPattern(0x02);
	}
	else {
		aiMini4wdClearLedPattern(0x02);
	}	
}

static int sFlag = 1;
static void _onStart_cb(void) {
	aiMini4wdToggleLedPattern(0x04);	

	if (sFlag) {
		aiMini4wdMotorDriverSetRpm(1000);
		sFlag = 0;
	}
}

static void _onExtTrig(void) {
	return;
}

static volatile int sSensorFlag = 0;
static AiMini4wdSensorData sSensorData;
static void _onSensor_cb(AiMini4wdSensorData *sensor_data)
{
	sSensorFlag = 1;
	memcpy(&sSensorData, sensor_data, sizeof(AiMini4wdSensorData));
	
}

static volatile int s100msFlag = 0;
static void _on100ms(void)
{
	s100msFlag = 1;
	return;
}

int main(void)
{
	aiMini4wdInitialize(AI_MINI_4WD_INIT_FLAG_USE_DEBUG_PRINT);

	aiMini4wdRegisterSwitchCb(cAiMini4wdSwitch0, _sw0_cb);
	aiMini4wdRegisterSwitchCb(cAiMini4wdSwitch1, _sw1_cb);

	aiMini4wdRegisterOnStartCb(_onStart_cb);
	aiMini4wdRegisterExtInterrupt(_onExtTrig);

	aiMini4wdSensorRegisterCapturedCallback(_onSensor_cb);

	aiMini4WdTimerRegister100msCallback(_on100ms);

	aiMini4wdDebugPrintf("Test for AI mini 4WD %s %s\r\n", __DATE__, __TIME__);
	aiMini4wdDebugPrintf("System Initialized. \r\n");


	AiMini4wdFile *fp1;
	fp1 = aiMini4wdFsOpen("OUT1.TXT", "w");
	if (fp1 == NULL) {
		aiMini4wdSetErrorStatus(AI_ERROR_NULL);
	}

	//J Fsのパフォーマンスを出す為に一旦サイズを指定してフラッシュ上の領域を繋いでおく
	aiMini4wdFsSeek(fp1, 1024*1024);
	aiMini4wdFsSeek(fp1, 0);

	char line[256];
    while (sLogEnabled) 
    {
		if (sSensorFlag) {
			aiMini4wdToggleLedPattern(0x08);
			sSensorFlag = 0;

			float vbat = 0;
			float mcurrent = 0;
			aiMini4wdGetBatteryVoltage(&vbat);
			aiMini4wdMotorDriverGetDriveCurrent(&mcurrent);

			snprintf(line, sizeof(line), "%.2f\t%.2f\t%.2f\t%.0f\t%.0f\t%.0f\t%.2f\t%.2f\t%.2f\t\r\n", 
					sSensorData.imu.accel_f[0],	// Y 
					sSensorData.imu.accel_f[1], // X
					sSensorData.imu.accel_f[2], // Z
					sSensorData.imu.gyro_f[0],
					sSensorData.imu.gyro_f[0],
					sSensorData.imu.gyro_f[0],
					sSensorData.rpm,
					vbat,
					mcurrent );
			aiMini4wdFsPuts(fp1, line, strlen(line));
		}
    }

	//J 残った文字列をFlushして、サイズを切り詰める
	aiMini4wdFsPutsFlush(fp1);
	aiMini4wdFsTruncate(fp1);
	
	aiMini4wdFsClose(fp1);
	aiMini4wdPrintLedPattern(15);
	while(1);

	return 0;
}
