/*
 * samd51_sercom.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdint.h>
#include <stddef.h>

#include <sam.h>

#include <samd51_error.h>
#include <samd51_sercom.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static SAMD51_SERCOM_INTERRUPT_CB sCallbacks[5][4]
=
{
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL}
};


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_sercom_set_interrupt_handler(SAMD51_SERCOM sercom, uint32_t index, SAMD51_SERCOM_INTERRUPT_CB cb)
{
	if (index > 3) {
		return AI_ERROR_INVALID;
	}

	switch (sercom) {
	case SAMD51_SERCOM0:
		NVIC_EnableIRQ(SERCOM0_0_IRQn + index);
		sCallbacks[sercom][index] = cb;
		break;

	case SAMD51_SERCOM1:
		NVIC_EnableIRQ(SERCOM1_0_IRQn + index);
		sCallbacks[sercom][index] = cb;
		break;

	case SAMD51_SERCOM2:
		NVIC_EnableIRQ(SERCOM2_0_IRQn + index);
		sCallbacks[sercom][index] = cb;
		break;

	case SAMD51_SERCOM3:
		NVIC_EnableIRQ(SERCOM3_0_IRQn + index);
		sCallbacks[sercom][index] = cb;
		break;

	case SAMD51_SERCOM4:
		NVIC_EnableIRQ(SERCOM4_0_IRQn + index);
		sCallbacks[sercom][index] = cb;
		break;

	default:
		return AI_ERROR_INVALID;
	}

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
void samd51_sercom_reset_intterrupt(SAMD51_SERCOM sercom)
{
	switch (sercom) {
	case SAMD51_SERCOM0:
		NVIC_DisableIRQ(SERCOM0_0_IRQn);
		NVIC_DisableIRQ(SERCOM0_1_IRQn);
		NVIC_DisableIRQ(SERCOM0_2_IRQn);
		break;

	case SAMD51_SERCOM1:
		NVIC_DisableIRQ(SERCOM1_0_IRQn);
		NVIC_DisableIRQ(SERCOM1_1_IRQn);
		NVIC_DisableIRQ(SERCOM1_2_IRQn);
		break;

	case SAMD51_SERCOM2:
		NVIC_DisableIRQ(SERCOM2_0_IRQn);
		NVIC_DisableIRQ(SERCOM2_1_IRQn);
		NVIC_DisableIRQ(SERCOM2_2_IRQn);
		break;

	case SAMD51_SERCOM3:
		NVIC_DisableIRQ(SERCOM3_0_IRQn);
		NVIC_DisableIRQ(SERCOM3_1_IRQn);
		NVIC_DisableIRQ(SERCOM3_2_IRQn);
		break;

	case SAMD51_SERCOM4:
		NVIC_DisableIRQ(SERCOM4_0_IRQn);
		NVIC_DisableIRQ(SERCOM4_1_IRQn);
		NVIC_DisableIRQ(SERCOM4_2_IRQn);
		break;

	default:
		break;
	}
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SERCOM0_0_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM0][0] != NULL) {
		sCallbacks[SAMD51_SERCOM0][0](SAMD51_SERCOM0);
	}
	return;
}

void SERCOM0_1_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM0][1] != NULL) {
		sCallbacks[SAMD51_SERCOM0][1](SAMD51_SERCOM0);
	}
	return;
}

void SERCOM0_2_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM0][2] != NULL) {
		sCallbacks[SAMD51_SERCOM0][2](SAMD51_SERCOM0);
	}
	return;
}

void SERCOM0_3_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM0][3] != NULL) {
		sCallbacks[SAMD51_SERCOM0][3](SAMD51_SERCOM0);
	}
	return;
}

void SERCOM1_0_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM1][0] != NULL) {
		sCallbacks[SAMD51_SERCOM1][0](SAMD51_SERCOM1);
	}
	return;
}

void SERCOM1_1_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM1][1] != NULL) {
		sCallbacks[SAMD51_SERCOM1][1](SAMD51_SERCOM1);
	}
	return;
}

void SERCOM1_2_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM1][2] != NULL) {
		sCallbacks[SAMD51_SERCOM1][2](SAMD51_SERCOM1);
	}
	return;
}

void SERCOM1_3_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM1][3] != NULL) {
		sCallbacks[SAMD51_SERCOM1][3](SAMD51_SERCOM1);
	}
	return;
}

void SERCOM2_0_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM2][0] != NULL) {
		sCallbacks[SAMD51_SERCOM2][0](SAMD51_SERCOM2);
	}
	return;
}

void SERCOM2_1_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM2][1] != NULL) {
		sCallbacks[SAMD51_SERCOM2][1](SAMD51_SERCOM2);
	}
	return;
}

void SERCOM2_2_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM2][2] != NULL) {
		sCallbacks[SAMD51_SERCOM2][2](SAMD51_SERCOM2);
	}
	return;
}

void SERCOM2_3_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM2][3] != NULL) {
		sCallbacks[SAMD51_SERCOM2][3](SAMD51_SERCOM2);
	}
	return;
}

void SERCOM3_0_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM3][0] != NULL) {
		sCallbacks[SAMD51_SERCOM3][0](SAMD51_SERCOM3);
	}
	return;
}

void SERCOM3_1_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM3][1] != NULL) {
		sCallbacks[SAMD51_SERCOM3][1](SAMD51_SERCOM3);
	}
	return;
}

void SERCOM3_2_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM3][2] != NULL) {
		sCallbacks[SAMD51_SERCOM3][2](SAMD51_SERCOM3);
	}
	return;
}

void SERCOM3_3_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM3][3] != NULL) {
		sCallbacks[SAMD51_SERCOM3][3](SAMD51_SERCOM3);
	}
	return;
}

void SERCOM4_0_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM4][0] != NULL) {
		sCallbacks[SAMD51_SERCOM4][0](SAMD51_SERCOM4);
	}
	return;
}

void SERCOM4_1_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM4][1] != NULL) {
		sCallbacks[SAMD51_SERCOM4][1](SAMD51_SERCOM4);
	}
	return;
}

void SERCOM4_2_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM4][2] != NULL) {
		sCallbacks[SAMD51_SERCOM4][2](SAMD51_SERCOM4);
	}
	return;
}

void SERCOM4_3_Handler(void)
{
	if (sCallbacks[SAMD51_SERCOM4][3] != NULL) {
		sCallbacks[SAMD51_SERCOM4][3](SAMD51_SERCOM4);
	}
	return;
}

