/*
 * samd51_timer.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SAMD51_TIMER_H_
#define SAMD51_TIMER_H_

typedef enum SAMD51_TC_t
{
	SAMD51_TC0 = 0,
	SAMD51_TC1,
	SAMD51_TC2,
	SAMD51_TC3,
	SAMD51_TC4,
	SAMD51_TC5,
	SAMD51_TC6,
	SAMD51_TC7,
	
	SAMD51_TCC0 = 0x80000001,
	SAMD51_TCC1,
	SAMD51_TCC2,
	SAMD51_TCC3,
	SAMD51_TCC4,
} SAMD51_TC;

typedef enum SAMD51_TC_PRESCALE_t
{
	SAMD51_TC_PRESCALE_DIV1 = 0,
	SAMD51_TC_PRESCALE_DIV2,
	SAMD51_TC_PRESCALE_DIV4,
	SAMD51_TC_PRESCALE_DIV8,
	SAMD51_TC_PRESCALE_DIV16,
	SAMD51_TC_PRESCALE_DIV64,
	SAMD51_TC_PRESCALE_DIV256,
	SAMD51_TC_PRESCALE_DIV1024
} SAMD51_TC_PRESCALE;


typedef void (*SAMD51_TIMER_COUNTER_CB)(void);

int samd51_tc_initialize_as_timer(SAMD51_TC tc, uint32_t peripheral_clock, uint32_t tick_us, SAMD51_TIMER_COUNTER_CB cb);
int samd51_tc_initialize_as_pwm(SAMD51_TC tc, SAMD51_TC_PRESCALE scale);
void samd51_tc_finalize(SAMD51_TC tc);
int samd51_tc_set_pwm(SAMD51_TC tc, uint8_t wave_index, uint16_t duty);

int samd51_tc_initialize_as_oneshot(SAMD51_TC, uint32_t peripheral_clock, SAMD51_TC_PRESCALE div, SAMD51_TIMER_COUNTER_CB cb);
int samd51_tc_start_onshot(SAMD51_TC tc);

int samd51_tc_initialize_as_freerun_counter(SAMD51_TC tc, SAMD51_TC_PRESCALE scale);
int16_t samd51_tc_get_counter(SAMD51_TC tc);


int samd51_tcc_initialize_as_timer(SAMD51_TC tc, uint32_t peripheral_clock, uint32_t tick_us, SAMD51_TIMER_COUNTER_CB cb);
int samd51_tcc_initialize_as_freerun_counter(SAMD51_TC tc, SAMD51_TC_PRESCALE scale);
int samd51_tcc_initialize_as_pwm(SAMD51_TC tc, SAMD51_TC_PRESCALE scale, uint32_t top, uint8_t enabled_ch);
int samd51_tcc_set_pwm(SAMD51_TC tc, uint8_t wave_index, uint32_t duty);
uint32_t samd51_tcc_get_counter(SAMD51_TC tc);

#endif /* SAMD51_TIMER_H_ */