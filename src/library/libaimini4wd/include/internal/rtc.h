/*
 * rtc.h
 *
 * Created: 2023/03/25 8:15:47
 *  Author: kiyot
 */ 


#ifndef RTC_H_
#define RTC_H_

#include <samd51_sercom.h>

int aiMini4wdInitializeRtc(SAMD51_SERCOM sercom);
void aiMIni4wdRtcCycle1sec(void);

#endif /* RTC_H_ */