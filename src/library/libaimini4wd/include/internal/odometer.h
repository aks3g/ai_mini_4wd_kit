/*
 * odometer.h
 *
 * Created: 2021/03/01 11:50:44
 *  Author: kiyot
 */ 


#ifndef ODOMETER_H_
#define ODOMETER_H_

#pragma pack(1)
typedef struct OdometerData_t
{
	int32_t delta_x_count;
	int32_t delta_y_count;
	uint8_t  quality;
} OdometerData;
#pragma pack()

extern float gMMpCnt;

int odometer_probe(void);
int odometer_grab(void);

OdometerData *odometer_get_latest_data(void);
int odometer_is_busy(void);

#endif /* ODOMETER_H_ */