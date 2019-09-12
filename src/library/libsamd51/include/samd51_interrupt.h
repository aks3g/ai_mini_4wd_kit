/*
 * samd51_interrupt.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SAMD51_INTERRUPT_H_
#define SAMD51_INTERRUPT_H_


typedef enum SAMD51_EIC_CHANNEL_t
{
	SAMD51_EIC_CHANNEL0 = 0,
	SAMD51_EIC_CHANNEL1,
	SAMD51_EIC_CHANNEL2,
	SAMD51_EIC_CHANNEL3,
	SAMD51_EIC_CHANNEL4,
	SAMD51_EIC_CHANNEL5,
	SAMD51_EIC_CHANNEL6,
	SAMD51_EIC_CHANNEL7,
	SAMD51_EIC_CHANNEL8,
	SAMD51_EIC_CHANNEL9,
	SAMD51_EIC_CHANNEL10,
	SAMD51_EIC_CHANNEL11,
	SAMD51_EIC_CHANNEL12,
	SAMD51_EIC_CHANNEL13,
	SAMD51_EIC_CHANNEL14,
	SAMD51_EIC_CHANNEL15,
	SAMD51_EIC_CHANNEL_MAX,
} SAMD51_EIC_CHANNEL;

typedef enum SAMD51_EIC_SENSE_TYPE_t
{
	SAMD51_EIC_SENSE_NONE = 0,
	SAMD51_EIC_SENSE_RISE,
	SAMD51_EIC_SENSE_FALL,
	SAMD51_EIC_SENSE_BOTH,
	SAMD51_EIC_SENSE_HIGH,
	SAMD51_EIC_SENSE_LOW,
	SAMD51_EIC_SENSE_MAX
} SAMD51_EIC_SENSE_TYPE;

typedef void (*SAMD51_EIC_INTERRUPT_HANDLER)(void);

int samd51_external_interrupt_initialize(int use_clk32k);
void samd51_external_interrupt_finalize(void);

int samd51_external_interrupt_setup(SAMD51_EIC_CHANNEL ch, SAMD51_EIC_SENSE_TYPE type, int use_filter, SAMD51_EIC_INTERRUPT_HANDLER cb);

void samd51_external_interrupt_enable(SAMD51_EIC_CHANNEL ch, int enable);

uint16_t samd51_external_interrupt_get_pinstate(void);

#endif /* SAMD51_INTERRUPT_H_ */