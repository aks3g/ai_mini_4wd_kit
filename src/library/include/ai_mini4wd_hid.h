/*
 * ai_mini4wd_hid.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef AI_MINI4WD_HID_H_
#define AI_MINI4WD_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

//J LED Pattern
int aiMini4wdToggleLedPattern(uint32_t leds);
int aiMini4wdSetLedPattern(uint32_t leds);
int aiMini4wdClearLedPattern(uint32_t leds);
int aiMini4wdPrintLedPattern(uint32_t leds);

//J Switch関連
typedef enum AiMini4wdSwitch_t {
	cAiMini4wdSwitch0 = 0,
	cAiMini4wdSwitch1 = 1,
} AiMini4wdSwitch;

int aiMini4wdGetSwitchStatus(AiMini4wdSwitch sw);

typedef void (*AiMini4wdSwitchCallback)(int stat);
int aiMini4wdRegisterSwitchCb(AiMini4wdSwitch sw, AiMini4wdSwitchCallback cb);

//J タイヤを少し回したときに実行するコールバックの登録
typedef void (*AiMini4wdOnStartCallback)(void);
int aiMini4wdRegisterOnStartCb(AiMini4wdOnStartCallback cb);

typedef void (*AiMini4wdOnExtIntCallback)(void);
int aiMini4wdRegisterExtInterrupt(AiMini4wdOnExtIntCallback cb);

#ifdef __cplusplus
}
#endif

#endif /* AI_MINI4WD_HID_H_ */