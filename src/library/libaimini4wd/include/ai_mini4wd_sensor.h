/*
 * ai_mini4wd_sensor.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 

#ifndef AI_MINI4WD_SENSOR_H_
#define AI_MINI4WD_SENSOR_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AiMini4wdSensorData_t
{
	struct {
		float accel_f[3];
		float gyro_f[3];
	} imu;
	float rpm;
} AiMini4wdSensorData;

typedef void (*AiMini4wdCapturedSensorDataCb)(AiMini4wdSensorData *sensor_data);

int aiMini4wdSensorGrabLastData(AiMini4wdSensorData *sensor_data);
int aiMini4wdSensorRegisterCapturedCallback(AiMini4wdCapturedSensorDataCb cb);

float aiMini4wdSensorGetCurrentRpm(void);
float aiMini4wdSensorGetSpeed(void);
int aiMini4wdSensorSetTireSize(float mm);

int aiMini4wdSensorCalibrateTachoMeter(uint16_t *threshold_mv, uint16_t *work_buf, size_t length);
int aiMini4wdSensorSetTachometerThreshold(uint16_t threshold_mv, int save);

#ifdef __cplusplus
}
#endif

#endif/*AI_MINI4WD_SENSOR_H_*/