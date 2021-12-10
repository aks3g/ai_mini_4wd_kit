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


typedef struct REG_TCC_t{
	volatile uint32_t CTRLA;		// 0x00
	volatile uint8_t  CTRLBCLR;		// 0x04
	volatile uint8_t  CTRLBSET;		// 0x05
	volatile uint16_t _reserved;	// 0x06
	volatile uint32_t SYNCBUSY;		// 0x08
	volatile uint32_t FCTRLA;		// 0x0C
	volatile uint32_t FCTRLB;		// 0x10
	volatile uint32_t WEXCTRL;		// 0x14
	volatile uint32_t DRVCTRL;		// 0x18
	volatile uint16_t _reserved0;	// 0x1C
	volatile uint8_t  DBGCTRL;		// 0x1E
	volatile uint8_t  _reserved6;	// 0x1f
	volatile uint32_t EVCTRL;		// 0x20
	volatile uint32_t INTCLR;		// 0x24
	volatile uint32_t INTSET;		// 0x28
	volatile uint32_t INTFLAG;		// 0x2C
	volatile uint32_t STATUS;		// 0x30
	volatile uint32_t COUNT;		// 0x34
	volatile uint16_t PATT;			// 0x38
	volatile uint16_t _reserved1;	// 0x3A
	volatile uint32_t WAVE;			// 0x3C
	volatile uint32_t PER;			// 0x40
	volatile uint32_t CC0;			// 0x44
	volatile uint32_t CC1;			// 0x48
	volatile uint32_t CC2;			// 0x4C
	volatile uint32_t CC3;			// 0x50
	volatile uint32_t CC4;			// 0x54
	volatile uint32_t CC5;			// 0x58
	volatile uint32_t _reserved2;	// 0x5C
	volatile uint32_t _reserved3;	// 0x60
	volatile uint16_t PATTBUF;		// 0x64
	volatile uint16_t _reserved4;	// 0x66
	volatile uint32_t _reserved5;	// 0x68
	volatile uint32_t PERBUF;		// 0x6C
	volatile uint32_t CCBUF0;		// 0x70
	volatile uint32_t CCBUF1;		// 0x74
	volatile uint32_t CCBUF2;		// 0x78
	volatile uint32_t CCBUF3;		// 0x7C
	volatile uint32_t CCBUF4;		// 0x80
	volatile uint32_t CCBUF5;		// 0x84
} REG_TCC;

#define REG_TCC0_BASE			(0x41016000UL)
#define REG_TCC1_BASE			(0x41018000UL)
#define REG_TCC2_BASE			(0x42000C00UL)
#define REG_TCC3_BASE			(0x42001000UL)
#define REG_TCC4_BASE			(0x43001000UL)

enum TC_CMD {
	SAMD51_TC_CMD_NONE			= 0x0 << 5,
	SAMD51_TC_CMD_RETRIGGER		= 0x1 << 5,
	SAMD51_TC_CMD_STOP			= 0x2 << 5,
	SAMD51_TC_CMD_UPDATE		= 0x3 << 5,
	SAMD51_TC_CMD_READSYNC		= 0x4 << 5
};

#define SAMD51_TC_MODE_16BIT	0x0
#define SAMD51_TC_MODE_8BIT		0x1
#define SAMD51_TC_MODE_32BIT	0x2


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

static SAMD51_TIMER_COUNTER_CB sTcc0Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTcc1Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTcc2Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTcc3Callback = NULL;
static SAMD51_TIMER_COUNTER_CB sTcc4Callback = NULL;


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
	case SAMD51_TCC0:
		sTcc0Callback = cb;
		break;
	case SAMD51_TCC1:
		sTcc1Callback = cb;
		break;
	case SAMD51_TCC2:
		sTcc2Callback = cb;
		break;
	case SAMD51_TCC3:
		sTcc3Callback = cb;
		break;
	case SAMD51_TCC4:
		sTcc4Callback = cb;
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

static volatile REG_TCC *_getRegTcc(SAMD51_TC tc)
{
	volatile REG_TCC *base = NULL;

	switch (tc)
	{
	case SAMD51_TCC0:
		base = (volatile REG_TCC *)REG_TCC0_BASE;
		break;
	case SAMD51_TCC1:
		base = (volatile REG_TCC *)REG_TCC1_BASE;
		break;
	case SAMD51_TCC2:
		base = (volatile REG_TCC *)REG_TCC2_BASE;
		break;
	case SAMD51_TCC3:
		base = (volatile REG_TCC *)REG_TCC3_BASE;
		break;
	case SAMD51_TCC4:
		base = (volatile REG_TCC *)REG_TCC4_BASE;
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
	reg->INTENSET = (1 << 4);

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
	uint32_t mode = (SAMD51_TC_MODE_16BIT << 2);
	uint32_t enable = (1 << 1);
	
	reg->CTRLA = prescaler_bm | mode;
	reg->CTRLA |= enable;
	while (reg->SYNCBUSY & (1 << 1));

	// 48.7.1.3
	reg->CTRLBSET = (SAMD51_TC_CMD_RETRIGGER);
	while (reg->SYNCBUSY & (1 << 2));

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_tc_initialize_as_freerun_counter(SAMD51_TC tc, SAMD51_TC_PRESCALE scale)
{
	volatile REG_TC volatile *reg = _getRegTc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	reg->CCBUF0 = 0xffff;

	NVIC_EnableIRQ(TC0_IRQn + (int)tc);
	reg->INTENSET = 0x01; // OVF Interrupt

	// 48.7.1.1 Control A
	uint32_t prescaler_bm = ((uint16_t)scale << 8);
	uint32_t mode = (SAMD51_TC_MODE_16BIT << 2);
	uint32_t enable = (1 << 1);
	
	reg->CTRLA = prescaler_bm | mode | enable;

	reg->CTRLA |= enable;
	while (reg->SYNCBUSY & (1 << 1));

	// 48.7.1.3
	reg->CTRLBSET = (SAMD51_TC_CMD_RETRIGGER);
	while (reg->SYNCBUSY & (1 << 2));

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int16_t samd51_tc_get_counter(SAMD51_TC tc)
{
	volatile REG_TC volatile *reg = _getRegTc(tc);
	if (reg == NULL) {
		return 0;
	}

	reg->CTRLBSET = (SAMD51_TC_CMD_READSYNC);
	while (reg->SYNCBUSY & (1 << 2));
	uint16_t cnt = reg->COUNT;
	
	return cnt;
}

/*--------------------------------------------------------------------------*/
int samd51_tc_initialize_as_pwm(SAMD51_TC tc, SAMD51_TC_PRESCALE scale)
{
	volatile REG_TC volatile *reg = _getRegTc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	//WAVE.WAVEGEN
	reg->WAVE  = (0x02 << 0); // Normal PWM

	//CTRLA.MODE
	//CTRLA.PRESCALER
//	uint32_t prescaler_bm = (6 << 8);
	uint32_t prescaler_bm = (scale << 8);
	uint32_t mode = (SAMD51_TC_MODE_8BIT << 2);
	uint32_t enable = (1 << 1);
	
	reg->CTRLA = prescaler_bm | mode | enable;
	
	return AI_OK;
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



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_tcc_initialize_as_timer(SAMD51_TC tc, uint32_t peripheral_clock, uint32_t tick_us, SAMD51_TIMER_COUNTER_CB cb)
{
	volatile REG_TCC volatile *reg = _getRegTcc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	return AI_NOT_IMPLEMENTED;
}

/*--------------------------------------------------------------------------*/
int samd51_tcc_initialize_as_freerun_counter(SAMD51_TC tc, SAMD51_TC_PRESCALE scale)
{
	volatile REG_TCC volatile *reg = _getRegTcc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	// 24bit width.
	reg->PERBUF = 0xffffff;

	// Enable IRQ
	switch (tc){
	case SAMD51_TCC0:
		NVIC_EnableIRQ(TCC0_0_IRQn);
		break;
	case SAMD51_TCC1:
		NVIC_EnableIRQ(TCC1_0_IRQn);
		break;
	case SAMD51_TCC2:
		NVIC_EnableIRQ(TCC2_0_IRQn);
		break;
	default:
		return AI_ERROR_NODEV;	
	}
	reg->INTSET = 0x01; // OVF Interrupt

	// 48.7.1.1 Control A
	uint32_t prescaler_bm = ((uint16_t)scale << 8);
	uint32_t enable = (1 << 1);
	
	reg->CTRLA = prescaler_bm;
	reg->CTRLA |= enable;
	while (reg->SYNCBUSY & (1 << 1));

	// 48.7.1.3
	reg->CTRLBSET = (SAMD51_TC_CMD_RETRIGGER);
	while (reg->SYNCBUSY & (1 << 2));

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int samd51_tcc_initialize_as_pwm(SAMD51_TC tc, SAMD51_TC_PRESCALE scale, uint32_t top, uint8_t enabled_ch)
{
	volatile REG_TCC volatile *reg = _getRegTcc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	// Set top value.
	reg->PER = (top);
	while (reg->SYNCBUSY & (1 << 7));

	//WAVE.WAVEGEN
	reg->WAVE  = (0x02 << 0); // Normal PWM
	while (reg->SYNCBUSY & (1 << 6));

	//CTRLA.PRESCALER
	uint32_t prescaler_bm = (scale << 8);
	uint32_t enable = (1 << 1);
	
	reg->CTRLA = ((uint32_t)(enabled_ch&0x3f)<<24) | prescaler_bm | enable;
	while (reg->SYNCBUSY & (1 << 1));

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_tcc_set_pwm(SAMD51_TC tc, uint8_t wave_index, uint32_t duty)
{
	volatile REG_TCC volatile *reg = (REG_TCC *)_getRegTcc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	if (wave_index == 0) {
		reg->CC0 = duty;
		while (reg->SYNCBUSY & (1 << 8));
	}
	else if (wave_index == 1){
		reg->CC1 = duty;
		while (reg->SYNCBUSY & (1 << 9));
	}
	else if (wave_index == 2){
		reg->CC2 = duty;
		while (reg->SYNCBUSY & (1 << 10));
	}
	else if (wave_index == 3){
		reg->CC3 = duty;
		while (reg->SYNCBUSY & (1 << 11));
	}
	else if (wave_index == 4){
		reg->CC4 = duty;
		while (reg->SYNCBUSY & (1 << 12));
	}
	else if (wave_index == 5){
		reg->CC5 = duty;
		while (reg->SYNCBUSY & (1 << 13));
	}
	else {
		return AI_ERROR_NODEV;
	}

	return AI_OK;
}


uint32_t samd51_tcc_get_counter(SAMD51_TC tc)
{
	volatile REG_TCC volatile *reg = (REG_TCC *)_getRegTcc(tc);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	reg->CTRLBSET = (SAMD51_TC_CMD_READSYNC);

	while (reg->SYNCBUSY & (1 << 2));
	uint32_t cnt = reg->COUNT;

	return cnt;
}


/*--------------------------------------------------------------------------*/
void TCC0_0_Handler(void)
{
	volatile REG_TCC volatile *reg = (REG_TCC *)_getRegTcc(SAMD51_TCC0);
	volatile uint32_t flag = reg->INTFLAG;
	reg->INTFLAG = flag; //Clear all flags;

	if (sTcc0Callback) sTcc0Callback();

	return;
}

/*--------------------------------------------------------------------------*/
void TCC0_1_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC0_2_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC0_3_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC0_4_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC0_5_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC0_6_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC1_0_Handler(void)
{
	volatile REG_TCC volatile *reg = (REG_TCC *)_getRegTcc(SAMD51_TCC1);
	volatile uint32_t flag = reg->INTFLAG;
	reg->INTFLAG = flag; //Clear all flags;

	if (sTcc1Callback) sTcc1Callback();
	return;
}

/*--------------------------------------------------------------------------*/
void TCC1_1_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC1_2_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC1_3_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC1_4_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC2_0_Handler(void)
{
	volatile REG_TCC volatile *reg = (REG_TCC *)_getRegTcc(SAMD51_TCC2);
	volatile uint32_t flag = reg->INTFLAG;
	reg->INTFLAG = flag; //Clear all flags;

	if (sTcc2Callback) sTcc2Callback();
	return;
}

/*--------------------------------------------------------------------------*/
void TCC2_1_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC2_2_Handler(void)
{
	return;
}

/*--------------------------------------------------------------------------*/
void TCC2_3_Handler(void)
{
	return;
}
