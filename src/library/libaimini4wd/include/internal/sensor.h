/*
 * sensor.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SENSOR_H_
#define SENSOR_H_

typedef union AiMini4wdImuRawData_t
{
	uint8_t bytes[12];
	struct {
		int16_t gyro[3];
		int16_t accel[3];
	} imu;
} AiMini4wdImuRawData;

int aiMini4wdSensorsInitialize(void);

int aiMini4wdUpdateSensorData(AiMini4wdImuRawData *imu);

#endif /* SENSOR_H_ */