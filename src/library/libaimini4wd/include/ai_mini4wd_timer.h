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

struct AiMini4wdTm
{
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
};
	
int aiMini4wdTimerRegister10msCallback(AiMini4wdTimerCallback cb);
int aiMini4WdTimerRegister100msCallback(AiMini4wdTimerCallback cb);

uint32_t aiMini4WdTimerGetSystemtick(void);

uint32_t aiMini4wdRtcGetTimer(void);
void aiMini4wdRtcGetLocaltime(struct AiMini4wdTm *ltime);
void aiMini4wdRtcSetTimer(uint32_t epoc);


#ifdef __cplusplus
}
#endif

#endif /* AI_MINI4WD_TIMER_H_ */