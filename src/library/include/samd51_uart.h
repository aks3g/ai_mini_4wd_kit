/*
 * samd51_uart.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SAMD51_UART_H_
#define SAMD51_UART_H_

#include <stdint.h>
#include <stddef.h>

#include <samd51_sercom.h>

typedef enum SAMD51_SERCOM_PAD_t
{
	SAMD51_SERCOM_PAD0 = 0,
	SAMD51_SERCOM_PAD1 = 1,
	SAMD51_SERCOM_PAD2 = 2,
	SAMD51_SERCOM_PAD3 = 3
} SAMD51_SERCOM_PAD;

typedef struct SAMD51_UART_FIFO_t
{
	uint32_t write_ptr;
	uint32_t read_ptr;
	
	uint8_t *buf;
	size_t len;
} SAMD51_UART_FIFO;

int samd51_uart_fifo_setup(SAMD51_UART_FIFO *fifo, uint8_t *buf, size_t len);
int samd51_uart_fifo_is_empty(SAMD51_UART_FIFO *fifo);
int samd51_uart_fifo_is_full(SAMD51_UART_FIFO *fifo);
#if 0
uint32_t samd51_uart_fifo_data_length(SAMD51_UART_FIFO *fifo);
#endif
uint8_t samd51_uart_fifo_dequeue(SAMD51_UART_FIFO *fifo);
void    samd51_uart_fifo_enqueue(SAMD51_UART_FIFO *fifo, uint8_t data);

int samd51_uart_initialize(SAMD51_SERCOM sercom, uint32_t baudrate, SAMD51_SERCOM_PAD rx, SAMD51_SERCOM_PAD tx, SAMD51_UART_FIFO *tx_fifo, SAMD51_UART_FIFO *rx_fifo);
void samd51_uart_finalize(SAMD51_SERCOM sercom);

int samd51_uart_tx(SAMD51_SERCOM sercom, uint8_t *buf, size_t len);
int samd51_uart_try_rx(SAMD51_SERCOM sercom, uint8_t *buf);
int samd51_uart_rx(SAMD51_SERCOM sercom, uint8_t *buf, size_t len);

int samd51_uart_putc(SAMD51_SERCOM sercom, const char c);
int samd51_uart_puts(SAMD51_SERCOM sercom, const char *str);


#endif /* SAMD51_UART_H_ */