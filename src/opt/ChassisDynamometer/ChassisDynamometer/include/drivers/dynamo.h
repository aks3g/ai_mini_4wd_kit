/*
 * dynamo.h
 *
 * Created: 2021/06/30 5:31:57
 *  Author: kiyot
 */ 


#ifndef DYNAMO_H_
#define DYNAMO_H_

#include <stdint.h>

int dynamoInitialize(void);

int dynamoSetDuty(uint32_t ch, int16_t val);
int dynamoSetCurrent(uint32_t ch, int16_t current_mA);
int dynamoSetTorque(uint32_t ch, int16_t torque);

int32_t dynamoGetCurrent_mA(uint32_t ch);
int32_t dynamoGetReverseEmf_mV(uint32_t ch);
float dynamoGetRpm(uint32_t ch);
float dynamoGetVelocity_kmph(uint32_t ch);

typedef void (*DynamoCallback)(void);
int dynamoRegisterCapturedCallback(DynamoCallback cb);

#endif /* DYNAMO_H_ */