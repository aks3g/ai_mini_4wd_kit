/*
 * ai_mini4wd_timer.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef AI_MINI4WD_TIMER_H_
#define AI_MINI4WD_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*AiMini4wdTimerCallback)(void);

	
int aiMini4wdTimerRegister10msCallback(AiMini4wdTimerCallback cb);
int aiMini4WdTimerRegister100msCallback(AiMini4wdTimerCallback cb);

uint32_t aiMini4WdTimerGetSystemtick(void);


#ifdef __cplusplus
}
#endif

#endif /* AI_MINI4WD_TIMER_H_ */