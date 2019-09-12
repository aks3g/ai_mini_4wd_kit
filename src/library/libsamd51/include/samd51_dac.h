/*
 * samd51_dac.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SAMD51_DAC_H_
#define SAMD51_DAC_H_

typedef enum SAMD51_DAC_REFERENCE_t
{
	SAMD51_DAC_REF_EXTERNAL_VOLTAGE = 0,
	SAMD51_DAC_REF_VDD_ANA,
	SAMD51_DAC_REF_BUFFERED_EXTERNAL_VOLTAGE,
	SAMD51_DAC_REF_INTERNAL
} SAMD51_DAC_REFERENCE;

int samd51_dac_initialize(uint32_t ch, SAMD51_DAC_REFERENCE ref);
void samd51_dac_finalize(void);
int samd51_dac_output(uint32_t ch, uint16_t val);


#endif /* SAMD51_DAC_H_ */