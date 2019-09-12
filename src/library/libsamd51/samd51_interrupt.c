/*
 * samd51_interrupt.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdint.h>
#include <stddef.h>

#include <sam.h>

#include <samd51_error.h>
#include <samd51_interrupt.h>


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef struct REG_EIC_t
{
	volatile uint8_t CTRLA;
	volatile uint8_t NMICTRL;
	volatile uint16_t NMIFLAG;
	volatile uint32_t SYNCBUSY;
	volatile uint32_t EVCTRL;
	volatile uint32_t INTENCLR;
	volatile uint32_t INTENSET;
	volatile uint32_t INTFLAG;
	volatile uint32_t ASYNCH;
	volatile uint32_t CONFIG0;
	volatile uint32_t CONFIG1;
	volatile uint8_t reserved[12];
	volatile uint32_t DEBOUNCEN;
	volatile uint32_t DPRESCALER;
	volatile uint32_t PINSTATE;
} REG_EIC;


#define SAMD51_EIC_SWRST_bp				(0)
#define SAMD51_EIC_ENABLE_bp			(1)
#define SAMD51_EIC_CKSEL_bp				(4)

#define SAMD51_EIC_NMISENSE_bp			(0)
#define SAMD51_EIC_NMIFILTEN_bp			(3)
#define SAMD51_EIC_NMIASYNCH_bp			(4)

#define SAMD51_EIC_NMI_bp				(0)

#define SAMD51_EIC_SENSE_bp				(0)
#define SAMD51_EIC_FILTEN_bp			(3)


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#define SAMD51_EIC_BASE_ADDRESS			(0x40002800)

static volatile REG_EIC *sEicRegs = NULL;



static SAMD51_EIC_INTERRUPT_HANDLER sCallback[SAMD51_EIC_CHANNEL_MAX] =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_external_interrupt_initialize(int use_clk32k)
{
	sEicRegs = (volatile REG_EIC *)SAMD51_EIC_BASE_ADDRESS;
	
	NVIC_EnableIRQ(EIC_0_IRQn);
	NVIC_EnableIRQ(EIC_1_IRQn);
	NVIC_EnableIRQ(EIC_2_IRQn);
	NVIC_EnableIRQ(EIC_3_IRQn);
	NVIC_EnableIRQ(EIC_4_IRQn);
	NVIC_EnableIRQ(EIC_5_IRQn);
	NVIC_EnableIRQ(EIC_6_IRQn);
	NVIC_EnableIRQ(EIC_7_IRQn);
	NVIC_EnableIRQ(EIC_8_IRQn);
	NVIC_EnableIRQ(EIC_9_IRQn);
	NVIC_EnableIRQ(EIC_10_IRQn);
	NVIC_EnableIRQ(EIC_11_IRQn);
	NVIC_EnableIRQ(EIC_12_IRQn);
	NVIC_EnableIRQ(EIC_13_IRQn);
	NVIC_EnableIRQ(EIC_14_IRQn);
	NVIC_EnableIRQ(EIC_15_IRQn);


	if (use_clk32k) {
		sEicRegs->CTRLA = (1 << SAMD51_EIC_CKSEL_bp);
	}
	else {
		sEicRegs->CTRLA = (0 << SAMD51_EIC_CKSEL_bp);
	}

	sEicRegs->CTRLA |= (1 << SAMD51_EIC_ENABLE_bp);
	
	return AI_OK;
}


/*--------------------------------------------------------------------------*/
void samd51_external_interrupt_finalize(void)
{
	NVIC_DisableIRQ(EIC_0_IRQn);
	NVIC_DisableIRQ(EIC_1_IRQn);
	NVIC_DisableIRQ(EIC_2_IRQn);
	NVIC_DisableIRQ(EIC_3_IRQn);
	NVIC_DisableIRQ(EIC_4_IRQn);
	NVIC_DisableIRQ(EIC_5_IRQn);
	NVIC_DisableIRQ(EIC_6_IRQn);
	NVIC_DisableIRQ(EIC_7_IRQn);
	NVIC_DisableIRQ(EIC_8_IRQn);
	NVIC_DisableIRQ(EIC_9_IRQn);
	NVIC_DisableIRQ(EIC_10_IRQn);
	NVIC_DisableIRQ(EIC_11_IRQn);
	NVIC_DisableIRQ(EIC_12_IRQn);
	NVIC_DisableIRQ(EIC_13_IRQn);
	NVIC_DisableIRQ(EIC_14_IRQn);
	NVIC_DisableIRQ(EIC_15_IRQn);

	sEicRegs->CTRLA = (1 << SAMD51_EIC_SWRST_bp);
	while(sEicRegs->SYNCBUSY & (1 << SAMD51_EIC_SWRST_bp));
}


/*--------------------------------------------------------------------------*/
int samd51_external_interrupt_setup(SAMD51_EIC_CHANNEL ch, SAMD51_EIC_SENSE_TYPE type, int use_filter, SAMD51_EIC_INTERRUPT_HANDLER cb)
{
	if ((ch >= SAMD51_EIC_CHANNEL_MAX) || (type >= SAMD51_EIC_SENSE_MAX)) {
		return AI_ERROR_INVALID;
	}

	sEicRegs->CTRLA &= ~(1 << SAMD51_EIC_ENABLE_bp);
	while(sEicRegs->SYNCBUSY & (1 << SAMD51_EIC_ENABLE_bp));

	uint32_t config = type;
	config |= (use_filter) ? (1 << SAMD51_EIC_FILTEN_bp) : (0 << SAMD51_EIC_FILTEN_bp);

	//J Debaunce‚ª–³‚¢‚Æ1‰ñ–Ú‚ÌEdge‚µ‚©Detecto—ˆ‚È‚¢@‚ ‚Ù‚©
	sEicRegs->DEBOUNCEN |= (1 << ch);

	if (ch < 8) {
		int index = ch;	

		sEicRegs->CONFIG0 &= ~(0xf    << (index * 4));
		sEicRegs->CONFIG0 |=  (config << (index * 4));
	}
	else {
		int index = ch - 8;
		
		sEicRegs->CONFIG1 &= ~(0xf    << (index * 4));
		sEicRegs->CONFIG1 |=  (config << (index * 4));
	}
	
	//J Interrupt Enable
	sEicRegs->INTENSET |= (1 << ch);
	
	//J Callback‚Ì“o˜^
	sCallback[ch] = cb;

	sEicRegs->CTRLA |= (1 << SAMD51_EIC_ENABLE_bp);
	while(sEicRegs->SYNCBUSY & (1 << SAMD51_EIC_ENABLE_bp));

	return AI_OK;;
}

/*--------------------------------------------------------------------------*/
void samd51_external_interrupt_enable(SAMD51_EIC_CHANNEL ch, int enable)
{
	if (enable) {
		sEicRegs->INTENSET |= (1 << ch);
	}
	else {
		sEicRegs->INTENCLR |= (1 << ch);
	}

	return;
}

/*--------------------------------------------------------------------------*/
uint16_t samd51_external_interrupt_get_pinstate(void)
{
	return sEicRegs->PINSTATE;	
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EIC_0_Handler(void)
{
	int index = 0;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_1_Handler(void)
{
	int index = 1;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_2_Handler(void)
{
	int index = 2;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_3_Handler(void)
{
	int index = 3;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_4_Handler(void)
{
	int index = 4;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_5_Handler(void)
{
	int index = 5;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_6_Handler(void)
{
	int index = 6;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}
/*--------------------------------------------------------------------------*/
void EIC_7_Handler(void)
{
	int index = 7;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_8_Handler(void)
{
	int index = 8;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_9_Handler(void)
{
	int index = 9;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_10_Handler(void)
{
	int index = 10;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_11_Handler(void)
{
	int index = 11;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_12_Handler(void)
{
	int index = 12;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_13_Handler(void)
{
	int index = 13;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_14_Handler(void)
{
	int index = 14;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}

/*--------------------------------------------------------------------------*/
void EIC_15_Handler(void)
{
	int index = 15;
	
	sEicRegs->INTFLAG = (1 << index);
	if (sCallback[index] != NULL) {
		sCallback[index]();
	}
}
