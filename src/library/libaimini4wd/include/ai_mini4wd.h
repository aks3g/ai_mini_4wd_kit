/*
 * ai_mini4wd.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef AI_MINI_4WD_H_
#define AI_MINI_4WD_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AI_MINI_4WD_INIT_FLAG_USE_DEBUG_PRINT				(0x00000001)
#define AI_MINI_4WD_INIT_FLAG_USE_TEST_TYPE_HW				(0x00000002)
#define AI_MINI_4WD_INIT_FLAG_USE_USB_MASS					(0x00000004)
#define AI_MINI_4WD_INIT_FLAG_USE_USB_SERIAL				(0x00000008)

int aiMini4wdInitialize(uint32_t flags);
void aiMini4wdSetErrorStatus(int status);

int aiMini4wdGetBatteryVoltage(float *voltage_mV);

int aiMini4wdDebugPrintf(const char *format, ...);
int aiMini4wdDebugPuts(const char *str, size_t len);

int aiMini4wdRegistryUpdateTachometerThreshold_mv(uint16_t mv);

#ifdef __cplusplus
}
#endif


#endif /* AI_MINI_4WD_H_ */