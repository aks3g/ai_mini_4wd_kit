/*
 * samd51_sercom.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SAMD51_SERCOM_H_
#define SAMD51_SERCOM_H_

typedef enum SAMD51_SERCOM_t
{
	SAMD51_SERCOM0 = 0,
	SAMD51_SERCOM1,
	SAMD51_SERCOM2,
	SAMD51_SERCOM3,
	SAMD51_SERCOM4,
	SAMD51_SERCOM5,
	SAMD51_SERCOM6,
	SAMD51_SERCOM7,
} SAMD51_SERCOM;

typedef void (*SAMD51_SERCOM_INTERRUPT_CB)(SAMD51_SERCOM);

int samd51_sercom_set_interrupt_handler(SAMD51_SERCOM sercom, uint32_t index, SAMD51_SERCOM_INTERRUPT_CB cb);
void samd51_sercom_reset_intterrupt(SAMD51_SERCOM sercom);

#endif /* SAMD51_SERCOM_H_ */