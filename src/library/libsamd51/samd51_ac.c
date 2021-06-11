/*
 * samd51_ac.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdint.h>
#include <stddef.h>

#include <sam.h>

#include <samd51_error.h>
#include <samd51_ac.h>


typedef struct REG_AC_t
{
	volatile uint8_t  CTRLA;
	volatile uint8_t  CTRLB;
	volatile uint16_t EVCTRL;
	volatile uint8_t  INTENCLR;
	volatile uint8_t  INTENSET;
	volatile uint8_t  INTFLAG;
	volatile uint8_t  STATUSA;
	volatile uint8_t  STATUSB;
	volatile uint8_t  DBGCTRL;
	volatile uint8_t  WINCTRL;
	volatile uint8_t  _reserved1[1];
	volatile uint8_t  SCALER0;
	volatile uint8_t  SCALER1;
	volatile uint8_t  _reserved2[2];
	volatile uint32_t COMPCTRL0;
	volatile uint32_t COMPCTRL1;
	volatile uint8_t  _reserved3[8];
	volatile uint32_t SYNCBUSY;
	volatile uint16_t CALIB;
} REG_AC;

/* CTRLA */
#define SAMD51_AC_SWRST_pos						(0)
#define SAMD51_AC_ENABLE						(1)

/* CTRLB */
#define SAMD51_AC_START0_pos					(0)
#define SAMD51_AC_START1_pos					(1)

/* EVCTRL */
#define SAMD51_AC_COMPEO0_pos					(0)
#define SAMD51_AC_COMPEO1_pos					(1)
#define SAMD51_AC_WINEO0_pos					(4)
#define SAMD51_AC_COMPEI0_pos					(8)
#define SAMD51_AC_COMPEI1_pos					(9)
#define SAMD51_AC_INVEI0_pos					(12)
#define SAMD51_AC_INVEI1_pos					(13)

/* INT FLAGS */
#define SAMD51_AC_INT_COMP0_pos					(0)
#define SAMD51_AC_INT_COMP1_pos					(1)
#define SAMD51_AC_INT_WIN0_pos					(4)

/* STATUSA */
#define SAMD51_AC_STATE0_pos					(0)
#define SAMD51_AC_STATE1_pos					(1)
#define SAMD51_AC_WSTATE0_pos					(4)

/* STATUSB */
#define SAMD51_AC_READY0_pos					(0)
#define SAMD51_AC_READY1_pos					(1)

/* WINCTRL */
#define SAMD51_AC_WEN0_pos						(0)
#define SAMD51_AC_WINSEL0_pos					(1)

/* COMCTRLx */
#define SAMD51_AC_COMP_ENABLE_pos				(1)
#define SAMD51_AC_COMP_SINGLE_pos				(2)
#define SAMD51_AC_COMP_INTSEL_pos				(3)
#define SAMD51_AC_COMP_RUNSTDBY_pos				(6)
#define SAMD51_AC_COMP_MUXNEG_pos				(8)
#define SAMD51_AC_COMP_MUXPOS_pos				(12)
#define SAMD51_AC_COMP_SWAP_pos					(15)
#define SAMD51_AC_COMP_SPEED_pos				(16)
#define SAMD51_AC_COMP_HYSEN_pos				(19)
#define SAMD51_AC_COMP_HYST_pos					(20)
#define SAMD51_AC_COMP_FLEN_pos					(24)
#define SAMD51_AC_COMP_OUT_pos					(28)

/* SYNCBUSY */
#define SAMD51_AC_SYNCBUSY_SWRST_pos			(0)
#define SAMD51_AC_SYNCBUSY_ENABLE_pos			(1)
#define SAMD51_AC_SYNCBUSY_WINCTRL_pos			(2)
#define SAMD51_AC_SYNCBUSY_COMPCTRL0_pos		(3)
#define SAMD51_AC_SYNCBUSY_COMPCTRL1_pos		(4)



#define SAMD51_AC_BASE							(0x42002000L)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static REG_AC *reg_ac = (REG_AC *)SAMD51_AC_BASE;
static SAMD51_AC_INTERRUPT_CALLBACK sAcCallback[2] = {NULL, NULL};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_ac_initialize(
						uint32_t ch, 
						SAMD51_AC_POS_INPUT positive_input,
						SAMD51_AC_NEG_INPUT negative_input,
						SAMD51_AC_INTERRUPT_SEL intsel,
						SAMD51_AC_OUTPUT output_type,
						SAMD51_AC_FILTER filter,
						SAMD51_AC_HYST hysteresis,
						int singleshot,
						SAMD51_AC_INTERRUPT_CALLBACK cb
						)
{
	if (ch >= 2) {
		return AI_ERROR_NODEV;
	}
	
	reg_ac->CTRLA = 0;
	
	NVIC_EnableIRQ(AC_IRQn);
	
	uint32_t compctrl = (output_type << SAMD51_AC_COMP_OUT_pos) |
						(filter << SAMD51_AC_COMP_FLEN_pos) |
						(hysteresis << SAMD51_AC_COMP_HYST_pos) |
						(0 << SAMD51_AC_COMP_HYSEN_pos) |
						(3 << SAMD51_AC_COMP_SPEED_pos) |
						(positive_input<< SAMD51_AC_COMP_MUXPOS_pos) |
						(negative_input << SAMD51_AC_COMP_MUXNEG_pos) |
						(intsel << SAMD51_AC_COMP_INTSEL_pos) |
						(singleshot ? (1 << SAMD51_AC_COMP_SINGLE_pos) : 0) |
						(1 << SAMD51_AC_COMP_ENABLE_pos);
						
	if (ch == 0)
	{
		sAcCallback[0] = cb;
		
		reg_ac->INTENSET = (1 << SAMD51_AC_INT_COMP0_pos);
		reg_ac->COMPCTRL0 = compctrl;
		while(reg_ac->SYNCBUSY & (1 << SAMD51_AC_SYNCBUSY_COMPCTRL0_pos));
	}
	else
	{
		sAcCallback[1] = cb;

		reg_ac->INTENSET = (1 << SAMD51_AC_INT_COMP1_pos);
		reg_ac->COMPCTRL1 = compctrl;
		while(reg_ac->SYNCBUSY & (1 << SAMD51_AC_SYNCBUSY_COMPCTRL1_pos));
	}
	
	reg_ac->CTRLA = (1 << SAMD51_AC_ENABLE);
	while(reg_ac->SYNCBUSY & (1 << SAMD51_AC_SYNCBUSY_ENABLE_pos));

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
void samd51_ac_finalize(void) 
{
	NVIC_DisableIRQ(AC_IRQn);

	reg_ac->CTRLA = (1 << SAMD51_AC_SWRST_pos);
	while(reg_ac->SYNCBUSY & (1 << SAMD51_AC_SWRST_pos));
}

/*--------------------------------------------------------------------------*/
void AC_Handler(void) 
{
	uint8_t intflag = reg_ac->INTFLAG;
	reg_ac->INTFLAG = intflag;

	if(intflag & (1 << SAMD51_AC_INT_COMP0_pos)) {
		if (sAcCallback[0]) {
			sAcCallback[0]();
		}		
	}
	else if (intflag & (1 << SAMD51_AC_INT_COMP1_pos)) {
		if (sAcCallback[1]) {
			sAcCallback[1]();
		}
	}
	
}