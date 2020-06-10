/*
 * timer_driver.h
 *
 * Created: 2013/09/07 14:39:25
 *  Author: sazae7
 */ 
#ifndef TIMER_DRIVER_H_
#define TIMER_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define TIMER_OK								(0)
#define TIMER_CALLBACK_IS_ALREADY_REGISTERD		(-1)
#define TIMER_CANNOT_CONFIGURATE				(-2)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef void (*timer_callback)(void);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
uint8_t initialize_tcc5_as_timerMilSec(uint32_t periodInMiliSec, uint32_t peripheralClock);
uint8_t initialize_tcc5_as_timerMicroSec(uint32_t periodInMicroSec, uint32_t peripheralClock);
uint8_t tcc5_timer_registerCallback(timer_callback cb);
uint32_t tcc5_timer_getCurrentCount(void);
void tcc5_enable(uint8_t set);

uint8_t initialize_tcd5_as_timerMilSec(uint32_t periodInMiliSec, uint32_t peripheralClock);
uint8_t initialize_tcd5_as_timerMicroSec(uint32_t periodInMicroSec, uint32_t peripheralClock);
uint8_t tcd5_timer_registerCallback(timer_callback cb);
void tcd5_enable(uint8_t set);

uint8_t initialize_tcc4_as_timerMilSec(uint32_t periodInMiliSec, uint32_t peripheralClock);
uint8_t initialize_tcc4_as_timerMicroSec(uint32_t periodInMicroSec, uint32_t peripheralClock);
uint8_t tcc4_timer_registerCallback(timer_callback cb);
uint32_t tcc4_timer_getCurrentCount(void);
void tcc4_enable(uint8_t set);

#ifdef __cplusplus
};
#endif

#endif /* TIMER_DRIVER_H_ */