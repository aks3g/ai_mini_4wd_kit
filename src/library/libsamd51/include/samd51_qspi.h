/*
 * samd51_qspi.h
 *
 * Created: 2023/02/21 4:57:36
 *  Author: kiyot
 */ 


#ifndef SAMD51_QSPI_H_
#define SAMD51_QSPI_H_

typedef enum {
	SAMD51_QSPI_CLOCK_MODE0,
	SAMD51_QSPI_CLOCK_MODE1,
	SAMD51_QSPI_CLOCK_MODE2,
	SAMD51_QSPI_CLOCK_MODE3
} SAMD51_QSPI_CLOCK_MODE;

typedef struct
{
	uint32_t addr;
	union{
		uint32_t reg;
		struct {
			uint8_t instr;
			uint8_t dummy8_15;
			uint8_t opcode;
			uint8_t dummy24_31;
		} st;
	} code;
	union{
		uint32_t reg;
		struct {
			uint8_t width     : 3;
			uint8_t pad3      : 1;
			uint8_t instren   : 1;
			uint8_t addren    : 1;
			uint8_t opcodeen  : 1;
			uint8_t dataen    : 1;
			uint8_t opcodelen : 2;
			uint8_t addrlen   : 1;
			uint8_t pad11     : 1;
			uint8_t tfrtype   : 2;
			uint8_t crmode    : 1;
			uint8_t ddren     : 1;
			uint8_t dummy     : 5;
			uint16_t pad21_31 : 11;
		} st;
	} frame;
} SAMD51_QSPI_INSTRUCTION;


typedef void (*SAMD51_QSPI_TRANSACTION_DONE)(int error);

int samd51_qspi_initialize(uint8_t baud_div, uint8_t delay_before_sck, uint8_t minimum_inactive_CS_delay, uint8_t delay_between_consecutive_transfers, SAMD51_QSPI_CLOCK_MODE mode);

int samd51_qspi_exec_instruction(SAMD51_QSPI_INSTRUCTION *inst, uint8_t *wbuf, size_t wbuf_len, uint8_t *rbuf, size_t rbuf_len, SAMD51_QSPI_TRANSACTION_DONE cb);

int samd51_qspi_exec_instruction_until_bit_clear(SAMD51_QSPI_INSTRUCTION *inst, uint8_t clear_mask, uint32_t timeout);
int samd51_qspi_exec_instruction_until_bit_set(SAMD51_QSPI_INSTRUCTION *inst, uint8_t set_mask, uint32_t timeout);
int samd51_qspi_is_busy(void);


#endif /* SAMD51_QSPI_H_ */