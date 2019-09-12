/*
 * samd51_dac.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */
#include <stdint.h>
#include <stddef.h>

#include <sam.h>

#include <samd51_error.h>
#include <samd51_dac.h>


typedef struct REG_DAC_t
{
	volatile uint8_t  CTRLA;
	volatile uint8_t  CTRLB;
	volatile uint8_t  EVCTRL;
	volatile uint8_t  _reserved1[1];
	volatile uint8_t  INTENCLR;
	volatile uint8_t  INTENSET;
	volatile uint8_t  IntFLAG;
	volatile uint8_t  STATUS;
	volatile uint32_t SYNBUSY;
	volatile uint16_t DACCTRL0;
	volatile uint16_t DACCTRL1;
	volatile uint16_t DATA0;
	volatile uint16_t DATA1;
	volatile uint16_t DATABUF0;
	volatile uint16_t DATABUF1;
	volatile uint8_t  DBGCTRL;
	volatile uint8_t  _reserved2[3];
	volatile uint16_t RESULT0;
	volatile uint16_t RESULT1;
} REG_DAC;


/* CTRLA */
#define SAMD51_DAC_SWRST_pos					(0)
#define SAMD51_DAC_ENABLE_pos					(1)

/* CTRLB */
#define SAMD51_DAC_DIFF_pos						(0)
#define SAMD51_DAC_REFSEL_pos					(1)

/* EVCTRL */
#define SAMD51_DAC_STARTEI0_pos					(0)
#define SAMD51_DAC_STARTEI1_pos					(1)
#define SAMD51_DAC_EMPTYEO0_pos					(2)
#define SAMD51_DAC_EMPTYEO1_pos					(3)
#define SAMD51_DAC_INVEI0_pos					(4)
#define SAMD51_DAC_INVEI1_pos					(5)
#define SAMD51_DAC_RESRDYEO0_pos				(6)
#define SAMD51_DAC_RESRDYEO1_pos				(7)

/* INTFLAGS */
#define SAMD51_DAC_UNDERRUN0_pos				(0)
#define SAMD51_DAC_UNDERRUN1_pos				(1)
#define SAMD51_DAC_EMPTY0_pos					(2)
#define SAMD51_DAC_EMPTY1_pos					(3)
#define SAMD51_DAC_RESRDY0_pos					(4)
#define SAMD51_DAC_RESRDY1_pos					(5)
#define SAMD51_DAC_OVERRUN0_pos					(6)
#define SAMD51_DAC_OVERRUN1_pos					(7)

/* STATUS */
#define SAMD51_DAC_READY0_pos					(0)
#define SAMD51_DAC_READY1_pos					(1)
#define SAMD51_DAC_EOC0_pos						(2)
#define SAMD51_DAC_EOC1_pos						(3)

/*DACCTRLx*/
#define SAMD51_DAC_CTRL_LEFTADJ_pos				(0)
#define SAMD51_DAC_CTRL_ENABLE_pos				(1)
#define SAMD51_DAC_CTRL_CCTRL_pos				(2)
#define SAMD51_DAC_CTRL_FEXT_pos				(5)
#define SAMD51_DAC_CTRL_RUNSTBY_pos				(6)
#define SAMD51_DAC_CTRL_DITHER_pos				(7)
#define SAMD51_DAC_CTRL_REFRESH_pos				(8)
#define SAMD51_DAC_CTRL_OSR_pos					(13)



#define OSR_1									(0 << SAMD51_DAC_CTRL_OSR_pos)
#define OSR_2									(1 << SAMD51_DAC_CTRL_OSR_pos)
#define OSR_4									(2 << SAMD51_DAC_CTRL_OSR_pos)
#define OSR_8									(3 << SAMD51_DAC_CTRL_OSR_pos)
#define OSR_16									(4 << SAMD51_DAC_CTRL_OSR_pos)
#define OSR_32									(5 << SAMD51_DAC_CTRL_OSR_pos)

#define DITHERING_DISABLED						(0 << SAMD51_DAC_CTRL_DITHER_pos)
#define DITHERING_ENABLED						(1 << SAMD51_DAC_CTRL_DITHER_pos)

#define RUN_IN_STANDBY_DISABLED					(0 << SAMD51_DAC_CTRL_RUNSTBY_pos)
#define RUN_IN_STANDBY_ENABLED					(1 << SAMD51_DAC_CTRL_RUNSTBY_pos)

#define EXTERNAL_FILTER_DISABLED				(0 << SAMD51_DAC_CTRL_FEXT_pos)
#define EXTERNAL_FILTER_ENABLED					(1 << SAMD51_DAC_CTRL_FEXT_pos)

#define CC100K									(0 << SAMD51_DAC_CTRL_CCTRL_pos)
#define CC1M									(1 << SAMD51_DAC_CTRL_CCTRL_pos)
#define CC12M									(2 << SAMD51_DAC_CTRL_CCTRL_pos)

#define ENABLE									(1 << SAMD51_DAC_CTRL_ENABLE_pos)

#define SAMD51_DAC_BASE							(0x43002400)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static volatile REG_DAC	*reg_dac = (volatile REG_DAC *)(SAMD51_DAC_BASE);


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_dac_initialize(uint32_t ch, SAMD51_DAC_REFERENCE ref)
{
	if (ch >= 2) {
		return AI_ERROR_NODEV;
	}
	
	reg_dac->CTRLA = 0;

	// Control
	uint32_t ctrl = OSR_1 | (5 << SAMD51_DAC_CTRL_REFRESH_pos) | DITHERING_DISABLED | RUN_IN_STANDBY_ENABLED | EXTERNAL_FILTER_DISABLED | CC12M | ENABLE;
	if (ch == 0) {
		reg_dac->DACCTRL0 = ctrl;
	}
	else {
		reg_dac->DACCTRL1 = ctrl;
	}
	
	// CTRLB
	reg_dac->CTRLB = ((int)ref << SAMD51_DAC_REFSEL_pos);

	//J Enable 
	reg_dac->CTRLA = (1 << SAMD51_DAC_ENABLE_pos);
	while (reg_dac->SYNBUSY & (1 << SAMD51_DAC_ENABLE_pos));


	if (ch == 0) {
		while ((reg_dac->STATUS & (1<<SAMD51_DAC_READY0_pos)) == 0);
	}
	else {
		while ((reg_dac->STATUS & (1<<SAMD51_DAC_READY1_pos)) == 0);
	}

	samd51_dac_output(ch, 0);

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
void samd51_dac_finalize(void)
{
	reg_dac->CTRLA = (1 << SAMD51_DAC_SWRST_pos);
	while (reg_dac->SYNBUSY & (1 << SAMD51_DAC_SWRST_pos));
}

/*--------------------------------------------------------------------------*/
int samd51_dac_output(uint32_t ch, uint16_t val) 
{
	if (ch >= 2) {
		return AI_ERROR_NODEV;
	}

	if (ch == 0) {
		reg_dac->DATA0 = val;
		while ((reg_dac->STATUS & (1<<SAMD51_DAC_EOC0_pos)) == 0);
	}
	else{
		reg_dac->DATA1 = val;
		while ((reg_dac->STATUS & (1<<SAMD51_DAC_EOC1_pos)) == 0);
	}
	
	return AI_OK;
}