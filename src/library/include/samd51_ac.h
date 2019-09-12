/*
 * samd51_ac.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SAMD51_AC_H_
#define SAMD51_AC_H_

typedef enum SAMD51_AC_POS_INPUT_t
{
	SAMD51_AC_POS_PIN0 = 0,
	SAMD51_AC_POS_PIN1 = 1,
	SAMD51_AC_POS_PIN2 = 2,
	SAMD51_AC_POS_PIN3 = 3,
	SAMD51_AC_POS_VSCALE = 4
} SAMD51_AC_POS_INPUT;

typedef enum SAMD51_AC_NEG_INPUT_t
{
	SAMD51_AC_NEG_PIN0 = 0,
	SAMD51_AC_NEG_PIN1,
	SAMD51_AC_NEG_PIN2,
	SAMD51_AC_NEG_PIN3,
	SAMD51_AC_NEG_GND,
	SAMD51_AC_NEG_VSCALE,
	SAMD51_AC_NEG_BANDGAP,
	SAMD51_AC_NEG_DAC0
} SAMD51_AC_NEG_INPUT;

typedef enum SAMD51_AC_OUTPUT_t
{
	SAMD51_AC_OUTPUT_OFF,
	SAMD51_AC_OUTPUT_ASYNC,
	SAMD51_AC_OUTPUT_SYNC	
} SAMD51_AC_OUTPUT;

typedef enum SAMD51_AC_FILTER_t
{
	SAMD51_AC_FILTER_NO,
	SAMD51_AC_FILTER_MAJ3,
	SAMD51_AC_FILTER_MAJ5
} SAMD51_AC_FILTER;

typedef enum SAMD51_AC_HYST_t
{
	SAMD52_AC_HYST50mV,
	SAMD52_AC_HYST100mV,
	SAMD52_AC_HYST150mV
} SAMD51_AC_HYST;

typedef enum SAMD51_AC_INTERRUPT_SEL_t
{
	SAMD51_AC_INT_TOGGLE,
	SAMD51_AC_INT_RISING,
	SAMD51_AC_INT_FALLING,
	SAMD51_AC_INT_EOC
} SAMD51_AC_INTERRUPT_SEL;

typedef void (*SAMD51_AC_INTERRUPT_CALLBACK)(void);


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
						);
void samd51_ac_finalize(void);

#endif /* SAMD51_AC_H_ */