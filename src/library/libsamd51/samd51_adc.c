/*
 * samd51_adc.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 

#include <stdint.h>
#include <stddef.h>

#include <sam.h>

#include <samd51_error.h>
#include <samd51_adc.h>

#pragma pack(1)
typedef struct REG_ADC_t
{
	volatile uint16_t CTRLA;
	volatile uint8_t  EVCTRL;
	volatile uint8_t  DBGCTRL;
	volatile uint16_t INPUTCTRL;
	volatile uint16_t  CTRLB;
	volatile uint8_t REFCTRL;
	volatile uint8_t reserved;
	volatile uint8_t AVGCTRL;
	volatile uint8_t SAMPCTRL;
	volatile uint16_t WINLT;
	volatile uint16_t WINUT;
	volatile uint16_t GAINCORR;
	volatile uint16_t OFFSETCORR;
	volatile uint8_t  SWTRIG;
	volatile uint8_t  rserved1[23];
	volatile uint8_t  INTENCLR;
	volatile uint8_t  INTENSET;
	volatile uint8_t  INTFLAG;
	volatile uint8_t  STATUS;
	volatile uint32_t SYNCBUSY;
	volatile uint32_t DSEQDATA;
	volatile uint32_t DSEQCTRL;
	volatile uint32_t DSEQSTAT;
	volatile uint16_t RESULT;
	volatile uint8_t  reserved2[2];
	volatile uint16_t RESS;
	volatile uint8_t  rserved3[2];
	volatile uint16_t CALIB;
	
} REG_ADC;


// Control A bit pos
#define SAMD51_ADC_CTRLA_R2R				(15)
#define SAMD51_ADC_CTRLA_PRESCALER			(8)
#define SAMD51_ADC_CTRLA_ONDEMAND			(7)
#define SAMD51_ADC_CTRLA_RUNSTDBY			(6)
#define SAMD51_ADC_CTRLA_SLAVEEN			(5)
#define SAMD51_ADC_CTRLA_DUALSEL			(3)
#define SAMD51_ADC_CTRLA_ENABLE				(1)
#define SAMD51_ADC_CTRLA_SWRST				(0)

// Event Control
#define SAMD51_ADC_EVCTRL_WINMONEO			(5)
#define SAMD51_ADC_EVCTRL_RESRDYEO			(4)
#define SAMD51_ADC_EVCTRL_STARTINV			(3)
#define SAMD51_ADC_EVCTRL_FLUSHINV			(2)
#define SAMD51_ADC_EVCTRL_STARTEI			(1)
#define SAMD51_ADC_EVCTRL_FLUSHEI			(0)

// Input Control
#define SAMD51_ADC_INPUTCTRL_DSEQSTOP		(15)
#define SAMD51_ADC_INPUTCTRL_MUXNEG			(8)
#define SAMD51_ADC_INPUTCTRL_DIFFMODE		(7)
#define SAMD51_ADC_INPUTCTRL_MUXPOS			(0)

// Control B
#define SAMD51_ADC_CTRLB_WINSS				(11)
#define SAMD51_ADC_CTRLB_WINMODE			(8)
#define SAMD51_ADC_CTRLB_RESSEL				(3)
#define SAMD51_ADC_CTRLB_CORREN				(2)
#define SAMD51_ADC_CTRLB_FREERUN			(1)
#define SAMD51_ADC_CTRLB_KEFTADJ			(0)

// Reference Control
#define SAMD51_ADC_REFCTRL_REFCOMP			(7)
#define SAMD51_ADC_REFCTRL_REFSEL			(0)

// Average Control
#define SAMD51_ADC_AVGCTRL_ADJRES			(4)
#define SAMD51_ADC_AVGCTRL_SAMPLENUM		(0)

// Sampling Time Control
#define SAMD51_ADC_SAMPCTRL_OFFCOMP			(7)
#define SAMD51_ADC_SAMPCTRL_SAMPLEN			(0)

// Software Trigger
#define SAMD51_ADC_SWTRIG_START				(1)
#define SAMD51_ADC_SWTRIG_FLUSH				(0)

// Interrupt Enable Clear
#define SAMD51_ADC_INTENCLR_WINMON			(2)
#define SAMD51_ADC_INTENCLR_OVERRUN			(1)
#define SAMD51_ADC_INTENCLR_RESRDY			(0)

// Interrupt Enable Set
#define SAMD51_ADC_INTENSET_WINMON			(2)
#define SAMD51_ADC_INTENSET_OVERRUN			(1)
#define SAMD51_ADC_INTENSET_RESRDY			(0)

// Interrupt Flag Status and Clear
#define SAMD51_ADC_INTFLAG_WINMON			(2)
#define SAMD51_ADC_INTFLAG_OVERRUN			(1)
#define SAMD51_ADC_INTFLAG_RESRDY			(0)

// Status
#define SAMD51_ADC_STATUS_WCC				(2)
#define SAMD51_ADC_STATUS_BUSY				(0)

// Synchronization Busy
#define SAMD51_ADC_SYNCBUSY_RBSSW			(31)
#define SAMD51_ADC_SYNCBUSY_SWTRIG			(11)
#define SAMD51_ADC_SYNCBUSY_OFFSETCORR		(10)
#define SAMD51_ADC_SYNCBUSY_GAINCORR		(9)
#define SAMD51_ADC_SYNCBUSY_WINUT			(8)
#define SAMD51_ADC_SYNCBUSY_WINLT			(7)
#define SAMD51_ADC_SYNCBUSY_SAMPLCTRL		(6)
#define SAMD51_ADC_SYNCBUSY_AVGCTRL			(5)
#define SAMD51_ADC_SYNCBUSY_REFCTRL			(4)
#define SAMD51_ADC_SYNCBUSY_CTRLB			(3)
#define SAMD51_ADC_SYNCBUSY_INPUTCTRL		(2)
#define SAMD51_ADC_SYNCBUSY_ENABLE			(1)
#define SAMD51_ADC_SYNCBUSY_SWRST			(0)



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#define SAMD51_ADC0_BASE		0x43001C00L
#define SAMD51_ADC1_BASE		0x43002000L

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static REG_ADC *reg_adc0 = (REG_ADC *)SAMD51_ADC0_BASE;
static REG_ADC *reg_adc1 = (REG_ADC *)SAMD51_ADC1_BASE;

static SAMD51_ADC_CONVERSION_DONE_CB sConversionDoneCb[2] = {NULL, NULL};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static REG_ADC *_get_regset(uint32_t ch) {
	if (ch == 0) {
		return reg_adc0;
	}
	else if (ch == 1) {
		return reg_adc1;
	}
	else {
		return NULL;
	}
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_adc_setup(uint32_t ch, SAMD51_ADC_MODE mode, SAMD51_ADC_BIT_RESOLUTION res, SAMD51_ADC_REFERENCE ref, SAMD51_ADC_POST_PROCESS_OPT *opt)
{
	volatile REG_ADC *reg = _get_regset(ch);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}
	
	if (ch == 0 && ref == SAMD51_ADC_REF_EXTERNAL_REFC) {
		return AI_ERROR_INVALID;
	}
	
	NVIC_EnableIRQ(ADC0_1_IRQn + (2 * ch));

	// Disable ADC
	reg->CTRLA = 0;
	while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_ENABLE));

	reg->CTRLA =	(0x0007 << SAMD51_ADC_CTRLA_PRESCALER);
	while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_ENABLE));

	
	// 45.8.5 Control B
	reg->CTRLB =	(((uint16_t)res) << SAMD51_ADC_CTRLB_RESSEL) |
					(((uint16_t)mode) << SAMD51_ADC_CTRLB_FREERUN) ;
	while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_CTRLB));
	
	// 45.8.6 Reference Control
	reg->REFCTRL = (uint8_t)ref;
	while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_REFCTRL));
	
	// 45.8.7 Average Control
	if (opt != NULL) {
		reg->AVGCTRL = (opt->average_div_power << SAMD51_ADC_AVGCTRL_ADJRES) | (opt->average_cnt << SAMD51_ADC_AVGCTRL_SAMPLENUM);
		while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_AVGCTRL));
	}

	// 45.8.8 Sampling Time Control
//	reg->SAMPCTRL = 0;
	while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_SAMPLCTRL));
	
	// 45.8.1 Control A
	reg->CTRLA =	reg->CTRLA |
					(0x0001 << SAMD51_ADC_CTRLA_ENABLE);
	while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_ENABLE));
	
	return AI_OK;
}

/*--------------------------------------------------------------------------*/
void samd51_adc_finalize(uint32_t ch)
{
	volatile REG_ADC *reg = _get_regset(ch);
	if (reg == NULL) {
		return;
	}

	NVIC_DisableIRQ(ADC0_1_IRQn + (2 * ch));

	reg->CTRLA = (1 << SAMD51_ADC_CTRLA_SWRST);
	while(reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_SWRST));
}

/*--------------------------------------------------------------------------*/
int samd51_adc_convert(uint32_t ch, SAMD51_ADC_INPUT_TYPE input, SAMD51_ADC_POS_INPUT pos, SAMD51_ADC_NEG_INPUT neg, SAMD51_ADC_CONVERSION_DONE_CB cb)
{
	volatile REG_ADC *reg = _get_regset(ch);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	// 45.8.4 Input Control
	if (input == SAMD51_ADC_DIFFERNTIAL) {
		reg->INPUTCTRL =	(((uint16_t)pos) << SAMD51_ADC_INPUTCTRL_MUXPOS) |
							(((uint16_t)neg) << SAMD51_ADC_INPUTCTRL_MUXNEG) |
							(0x0001 << SAMD51_ADC_INPUTCTRL_DIFFMODE);
	}
	else {
		reg->INPUTCTRL =	(((uint16_t)pos) << SAMD51_ADC_INPUTCTRL_MUXPOS);
	}
	while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_INPUTCTRL));
	
	// 45.8.15 Interrupt Enable Set
	if (cb != NULL) {
		reg->INTENSET = 1 << SAMD51_ADC_INTENSET_RESRDY;
		sConversionDoneCb[ch] = cb;
	}
	else {
		reg->INTENCLR = 0x07;
	}
	
	// 45.8.13 Software Trigger	
	reg->SWTRIG = (1 << SAMD51_ADC_SWTRIG_START);
	while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_SWTRIG));

	if (cb == NULL) {
		while ((reg->INTFLAG & (1 << SAMD51_ADC_INTFLAG_RESRDY)) == 0);
		
		reg->INTFLAG = reg->INTFLAG;
	}

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_adc_get_result(uint32_t ch, int16_t *result)
{
	if (result == NULL) {
		return AI_ERROR_NULL;
	}

	REG_ADC *reg = _get_regset(ch);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	*result = reg->RESULT;

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int samd51_adc_abort(uint32_t ch)
{
	REG_ADC *reg = _get_regset(ch);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}
		
	// Disable ADC
	reg->CTRLA = 0;
	while (reg->SYNCBUSY & (1 << SAMD51_ADC_SYNCBUSY_ENABLE));
	
	return AI_OK;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ADC0_0_Handler(void)
{
	uint8_t flag = reg_adc0->INTFLAG;

	if (flag & (1 << SAMD51_ADC_INTFLAG_WINMON)) {
		if (sConversionDoneCb[0] != NULL) {
			sConversionDoneCb[0](AI_ERROR_ADC_HIT_WINDOW_MONITOR, 0);
		}
	}
	if (flag & (1 << SAMD51_ADC_INTFLAG_OVERRUN)) {
		if (sConversionDoneCb[0] != NULL) {
			sConversionDoneCb[0](AI_ERROR_ADC_OVER_RUN, 0);
		}
	}

	reg_adc0->INTFLAG = flag;
}

/*--------------------------------------------------------------------------*/
void ADC0_1_Handler(void)
{
	uint8_t flag = reg_adc0->INTFLAG;

	if (flag & (1 << SAMD51_ADC_INTFLAG_RESRDY)) {
		if (sConversionDoneCb[0] != NULL) {
			sConversionDoneCb[0](AI_OK, reg_adc0->RESULT);
		}
	}

	reg_adc0->INTFLAG = flag;	
}

/*--------------------------------------------------------------------------*/
void ADC1_0_Handler(void)
{
	uint8_t flag = reg_adc1->INTFLAG;

	reg_adc1->INTFLAG = flag;
}

/*--------------------------------------------------------------------------*/
void ADC1_1_Handler(void)
{
	uint8_t flag = reg_adc1->INTFLAG;

	if (flag & (1 << SAMD51_ADC_INTFLAG_RESRDY)) {
		if (sConversionDoneCb[1] != NULL) {
			sConversionDoneCb[1](AI_OK, reg_adc0->RESULT);
		}
	}

	reg_adc1->INTFLAG = flag;
}
