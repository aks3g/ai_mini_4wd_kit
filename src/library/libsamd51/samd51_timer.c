/*
 * samd51_timer.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <sam.h>

#include <stdint.h>
#include <stddef.h>

#include <samd51_error.h>
#include <samd51_timer.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef struct REG_TC_t{
	volatile uint32_t CTRLA;		// 0x00
	volatile uint8_t  CTRLBCLR;		// 0x04
	volatile uint8_t  CTRLBSET;		// 0x05
	volatile uint16_t EVCTRL;		// 0x06
	volatile uint8_t  INTENCLR;		// 0x08
	volatile uint8_t  INTENSET;		// 0x09
	volatile uint8_t  INTFLAG;		// 0x0A
	volatile uint8_t  STATUS;		// 0x0B
	volatile uint8_t  WAVE;			// 0x0C
	volatile uint8_t  DRVCTRL;		// 0x0D
	volatile uint8_t  _reserved0;	// 0x0E
	volatile uint8_t  DBGCTRL;		// 0x0F
	volatile uint32_t SYNCBUSY;		// 0x10
	volatile uint16_t COUNT;		// 0x14
	volatile uint8_t  _reserved1[6];// 0x16
	volatile uint16_t CC0;			// 0x1C
	volatile uint16_t CC1;			// 0x1E
	volatile uint8_t  _reserved2[16];// 0x20
	volatile uint32_t CCBUF0;		// 0x30
	volatile uint32_t CCBUF1;		// 0x31
} REG_TC;

typedef struct REG_TC8_t{
	volatile uint32_t CTRLA;		// 0x00
	volatile uint8_t  CTRLBCLR;		// 0x04
	volatile uint8_t  CTRLBSET;		// 0x05
	volatile uint16_t EVCTRL;		// 0x06
	volatile uint8_t  INTENCLR;		// 0x08
	volatile uint8_t  INTENSET;		// 0x09
	volatile uint8_t  INTFLAG;		// 0x0A
	volatile uint8_t  STATUS;		// 0x0B
	volatile uint8_t  WAVE;			// 0x0C
	volatile uint8_t  DRVCTRL;		// 0x0D
	volatile uint8_t  _reserved0;	// 0x0E
	volatile uint8_t  DBGCTRL;		// 0x0F
	volatile uint32_t SYNCBUSY;		// 0x10
	volatile uint8_t  COUNT;		// 0x14
	volatile uint8_t  _reserved1[6];// 0x16
	volatile uint8_t  PER;			// 0x1B
	volatile uint8_t  CC0;			// 0x1C
	volatile uint8_t  CC1;			// 0x1D
	volatile uint8_t  _reserved2[16];// 0x20
	volatile uint8_t  PREBUF;		// 0x30
	volatile uint8_t  CCBUF0;		// 0x30
	volatile uint8_t  CCBUF1;		// 0x31
} REG_TC8;


#define REG_TC0_BASE			(0x40003800UL)
#define REG_TC1_BASE			(0x40003C00UL)
#define REG_TC2_BASE			(0x4101A000UL)
#define REG_TC3_BASE			(0x4101C000UL)
#define REG_TC4_BASE			(0x42001400UL)
#define REG_TC5_BASE			(0x42001800UL)
#define REG_TC6_BASE			(0x43001400UL)
#define REG_TC7_BASE			(0x43001800UL)

enum TC_CMD {
	SAMD51_TC_CMD_NONE			= 0x0 << 5,
	SAMD51_TC_CMD_RETRIGGER		= 0x1 << 5,
	SAMD51_TC_CMD_STOP			= 0x2 << 5,
	SAMD51_TC_CMD_UPDATE		= 0x3 << 5,
	SAMD51_TC_CMD_READSYNC		= 0x4 << 5
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static SAMD51_TIMER_COUNTER_CB sTc0Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTc1Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTc2Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTc3Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTc4Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTc5Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTc6Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTc7Callback = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _set_callback(SAMD51_TC tc, SAMD51_TIMER_COUNTER_CB cb)
{
	switch (tc)
	{
	case SAMD51_TC0:
		sTc0Callback = cb;
		break;
	case SAMD51_TC1:
		sTc1Callback = cb;
		break;
	case SAMD51_TC2:
		sTc2Callback = cb;
		break;
	case SAMD51_TC3:
		sTc3Callback = cb;
		break;
	case SAMD51_TC4:
		sTc4Callback = cb;
		break;
	case SAMD51_TC5:
		sTc5Callback = cb;
		break;
	case SAMD51_TC6:
		sTc6Callback = cb;
		break;
	case SAMD51_TC7:
		sTc7Callback = cb;
		break;
	default:
		break;
	}

	return;
}
static volatile REG_TC *_getRegTc(SAMD51_TC tc)
{
	volatile REG_TC *base = NULL;

	switch (tc)
	{
	case SAMD51_TC0:
		base = (volatile REG_TC *)REG_TC0_BASE;
		break;
	case SAMD51_TC1:
		base = (volatile REG_TC *)REG_TC1_BASE;
		break;
	case SAMD51_TC2:
		base = (volatile REG_TC *)REG_TC2_BASE;
		break;
	case SAMD51_TC3:
		base = (volatile REG_TC *)REG_TC3_BASE;
		break;
	case SAMD51_TC4:
		base = (volatile REG_TC *)REG_TC4_BASE;
		break;
	case SAMD51_TC5:
		base = (volatile REG_TC *)REG_TC5_BASE;
		break;
	case SAMD51_TC6:
		base = (volatile REG_TC *)REG_TC6_BASE;
		break;
	case SAMD51_TC7:
		base = (volatile REG_TC *)REG_TC7_BASE;
		break;
	default:
		break;
	}
	
	return base;
}




/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_tc_initialize_as_timer(SAMD51_TC tc, uint32_t peripheral_clock, uint32_t tick_us, SAMD51_TIMER_COUNTER_CB cb)
{
	volatile REG_TC volatile *reg = _getRegTc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	//J MC0との一致割り込みを入れて使います。
	//J 16bitモードで動かします。
	uint32_t prescaler[8] = {1, 2, 4, 8, 16, 64, 256, 1024};
	volatile uint32_t i=0;
	volatile uint16_t top = 0;
	float tick = (float)tick_us / 1000000.0;
	for (i=0 ; i<8 ; ++i) {
		float tau = 1.0 / ((float)peripheral_clock /(float)prescaler[i]);
		float top_f = tick / tau;
		// t x top_f = tick_us;
		//float top_f = ((float)tick_us * (float)peripheral_clock) / ((float)prescaler[i] * 1000000.0);
		if (top_f <= (float)(65535)) {
			top = (uint16_t)top_f;
			break;
		}
	}
	
	if (top == 0) {
		return AI_ERROR_INVALID;
	}

	NVIC_EnableIRQ(TC0_IRQn + (int)tc);

	//J Set callback
	_set_callback(tc, cb);

	// 48.7.3.14
	reg->CCBUF0 = top;

	// 48.7.1.4 Event Control
	uint32_t mceo0 = (1 << 12);
	uint32_t evact = (0 << 0);
	
	reg->EVCTRL = mceo0 | evact; 
	
	// 48.7.1.1 Control A
	uint32_t prescaler_bm = (i << 8);
	uint32_t mode = (0 << 2);
	uint32_t enable = (1 << 1);
	
	reg->CTRLA = prescaler_bm | mode | enable;

	//
	reg->INTENSET = (1 << 4);

	// 48.7.1.3
	reg->CTRLBSET = (SAMD51_TC_CMD_RETRIGGER);

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_tc_initialize_as_pwm(SAMD51_TC tc, uint32_t pheripheral_clock, uint32_t period_ms, uint32_t initial_duty)
{
	volatile REG_TC volatile *reg = _getRegTc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	//WAVE.WAVEGEN
	reg->WAVE  = (0x02 << 0); // Normal PWM

	//CTRLA.MODE
	//CTRLA.PRESCALER
	uint32_t prescaler_bm = (6 << 8);
	uint32_t mode = (1 << 2);
	uint32_t enable = (1 << 1);
	
	reg->CTRLA = prescaler_bm | mode | enable;
	
	return AI_OK;
}


/*--------------------------------------------------------------------------*/
void samd51_tc_finalize(SAMD51_TC tc)
{
	volatile REG_TC8 volatile *reg = (REG_TC8 *)_getRegTc(tc);
	if (reg == NULL) {
		return;
	}

	NVIC_DisableIRQ(TC0_IRQn + (int)tc);

	reg->CTRLA = 1; //SWRST
	while (reg->SYNCBUSY & 1);
}

/*--------------------------------------------------------------------------*/
int samd51_tc_set_pwm(SAMD51_TC tc, uint8_t wave_index, uint16_t duty)
{
	volatile REG_TC8 volatile *reg = (REG_TC8 *)_getRegTc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	if (wave_index == 0) {
		reg->CC0 = duty;
	}
	else if (wave_index == 1){
		reg->CC1 = duty;
	}
	else {
		return AI_ERROR_NODEV;
	}

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int samd51_tc_initialize_as_oneshot(SAMD51_TC tc, uint32_t peripheral_clock, SAMD51_TC_PRESCALE div, SAMD51_TIMER_COUNTER_CB cb)
{
	volatile REG_TC volatile *reg = _getRegTc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	reg->CTRLA  = 0;

	NVIC_EnableIRQ(TC0_IRQn + (int)tc);

	//J Set callback
	_set_callback(tc, cb);
	
	reg->CCBUF0 = 0xffff;
	
	// 48.7.1.1 Control A
	uint32_t prescaler_bm = (div << 8);
	uint32_t mode = (0 << 2);
	uint32_t enable = (1 << 1);
	
	reg->CTRLA = prescaler_bm | mode;

	// Only Enable Overflow Interrupt
	reg->INTENSET = (1 << 0);

	reg->CTRLA |= enable;
	while (reg->SYNCBUSY & (1 << 1));

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_tc_start_onshot(SAMD51_TC tc)
{
	volatile REG_TC volatile *reg = _getRegTc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	reg->CTRLBSET = (SAMD51_TC_CMD_READSYNC);
	while (reg->SYNCBUSY & (1 << 2));
	uint32_t count = (uint16_t)reg->COUNT;

	//J Timer を一旦止めて
	reg->CTRLBSET = (SAMD51_TC_CMD_STOP);
	while (reg->SYNCBUSY & (1 << 2));
	
	//J カウントをクリア
	reg->COUNT = 0;
	while (reg->SYNCBUSY & (1 << 4));

	reg->CTRLBSET = SAMD51_TC_CMD_RETRIGGER | (1 << 2);
	while (reg->SYNCBUSY & (1 << 2));

	return (int)count;
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void TC0_Handler(void)
{
	volatile REG_TC volatile *reg = _getRegTc(SAMD51_TC0);
	reg->INTFLAG |= (0x33); //Clear all flags;

	if (sTc0Callback) {
		sTc0Callback();
	}

	reg->CTRLBSET = (SAMD51_TC_CMD_RETRIGGER);

}

/*--------------------------------------------------------------------------*/
void TC1_Handler(void)
{
	if (sTc1Callback) {
		sTc1Callback();
	}
}

/*--------------------------------------------------------------------------*/
void TC2_Handler(void)
{
	volatile REG_TC volatile *reg = _getRegTc(SAMD51_TC2);
	reg->INTFLAG |= (0x33); //Clear all flags;

	if (sTc2Callback) {
		sTc2Callback();
	}
}

/*--------------------------------------------------------------------------*/
void TC3_Handler(void)
{
	if (sTc3Callback) {
		sTc3Callback();
	}
}

/*--------------------------------------------------------------------------*/
void TC4_Handler(void)
{
	if (sTc4Callback) {
		sTc4Callback();
	}
}

/*--------------------------------------------------------------------------*/
void TC5_Handler(void)
{
	if (sTc5Callback) {
		sTc5Callback();
	}
}

/*--------------------------------------------------------------------------*/
void TC6_Handler(void)
{
	if (sTc6Callback) {
		sTc6Callback();
	}
}

/*--------------------------------------------------------------------------*/
void TC7_Handler(void)
{
	if (sTc7Callback) {
		sTc7Callback();
	}
}