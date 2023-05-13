/*
 * ai_mini4wd.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef AI_MINI_4WD_H_
#define AI_MINI_4WD_H_

//#include <stdint.h>
//#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AI_MINI_4WD_INIT_FLAG_USE_DEBUG_PRINT				(0x00000001)
#define AI_MINI_4WD_INIT_FLAG_USE_TEST_TYPE_HW				(0x00000002)
#define AI_MINI_4WD_INIT_FLAG_USE_USB_MASS					(0x00000004)
#define AI_MINI_4WD_INIT_FLAG_USE_USB_SERIAL				(0x00000008)
#define AI_MINI_4WD_INIT_FLAG_USE_ODOMETER					(0x00000010)
#define AI_MINI_4WD_INIT_FLAG_USE_LED_INDICATOR				(0x00000020)

int aiMini4wdInitialize(uint32_t flags);
void aiMini4wdSetErrorStatus(int status);

int aiMini4wdGetBatteryVoltage(float *voltage_mV);

int aiMini4wdDebugPrintf(const char *format, ...);
int aiMini4wdDebugPuts(const char *str, size_t len);
int aiMini4wdDebugPutc(const char c);

int aiMini4wdDebugTryGetc(void);
int aiMini4wdDebugGetc(void);


int aiMini4wdRegistryUpdateTachometerThreshold_mv(uint16_t mv);
int aiMini4wdOdometerEnabled(void);
int aiMini4wdDebugUartEnabled(void);
int aiMini4wdLedIndicatorEnabled(void);
int aiMini4wdusbEnabled(void);

int aiMini4wdSetLedIndicator(uint16_t value, uint8_t sep);

typedef void (*AiMini4wdOnVbusChanged)(int vbus);
int aiMini4wdRegisterOnVbusChangedCb(AiMini4wdOnVbusChanged cb);

void aiMini4wdReset(uint32_t reset_addr);


#ifdef __cplusplus
}
#endif


#endif /* AI_MINI_4WD_H_ */