/*
 * samd51_uart.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <sam.h>

#include <samd51_error.h>
#include <samd51_uart.h>

typedef struct REG_SERCOM_UART_t
{
	volatile uint32_t CTRLA;
	volatile uint32_t CTRLB;
	volatile uint32_t CTRLC;
	volatile uint16_t BAUD;
	volatile uint8_t  PXPL;
	volatile uint8_t _rserved0[5];
	volatile uint8_t INTENCLR;
	volatile uint8_t _reserved1;
	volatile uint8_t INTENSET;
	volatile uint8_t _reserved2;
	volatile uint8_t INTFLAG;
	volatile uint8_t _reserved3;
	volatile uint16_t STATUS;
	volatile uint32_t SYNCBUSY;
	volatile uint8_t RXERRCNT;
	volatile uint8_t _reserved4;
	volatile uint16_t LENGTH;
	volatile uint8_t _reserved5[4];
	volatile uint32_t DATA;
	volatile uint8_t _reserved6[4];
	volatile uint8_t DBGCTRL;
	volatile uint8_t _reserved7;
} REG_SERCOM_UART;

#define SAMD51_SERCOM_UART0_BASE			(0x40003000UL)
#define SAMD51_SERCOM_UART1_BASE			(0x40003400UL)
#define SAMD51_SERCOM_UART2_BASE			(0x41012000UL)
#define SAMD51_SERCOM_UART3_BASE			(0x41014000UL)
#define SAMD51_SERCOM_UART4_BASE			(0x43000000UL)
#define SAMD51_SERCOM_UART5_BASE			(0x43000400UL)
#define SAMD51_SERCOM_UART6_BASE			(0x43000800UL)
#define SAMD51_SERCOM_UART7_BASE			(0x43000C00UL)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static volatile REG_SERCOM_UART *_getRegUart(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_UART *reg = NULL;
	
	switch (sercom) {
	case SAMD51_SERCOM0:
		reg = (volatile REG_SERCOM_UART *)SAMD51_SERCOM_UART0_BASE;
		break;
	case SAMD51_SERCOM1:
		reg = (volatile REG_SERCOM_UART *)SAMD51_SERCOM_UART1_BASE;
		break;
	case SAMD51_SERCOM2:
		reg = (volatile REG_SERCOM_UART *)SAMD51_SERCOM_UART2_BASE;
		break;
	case SAMD51_SERCOM3:
		reg = (volatile REG_SERCOM_UART *)SAMD51_SERCOM_UART3_BASE;
		break;
	case SAMD51_SERCOM4:
		reg = (volatile REG_SERCOM_UART *)SAMD51_SERCOM_UART4_BASE;
		break;
	case SAMD51_SERCOM5:
		reg = (volatile REG_SERCOM_UART *)SAMD51_SERCOM_UART5_BASE;
		break;
	case SAMD51_SERCOM6:
		reg = (volatile REG_SERCOM_UART *)SAMD51_SERCOM_UART6_BASE;
		break;
	case SAMD51_SERCOM7:
		reg = (volatile REG_SERCOM_UART *)SAMD51_SERCOM_UART7_BASE;
		break;
	default:
		break;
	}
	
	return reg;
}

static void _interrupt_handler_txc(SAMD51_SERCOM sercom);
static void _interrupt_handler_rxc(SAMD51_SERCOM sercom);

/*--------------------------------------------------------------------------*/
typedef struct UartContext_t
{
	volatile int in_tx_sequence;
	SAMD51_UART_FIFO *tx_fifo;
	SAMD51_UART_FIFO *rx_fifo;
} UartContext;

static UartContext sUartCtx[5] //TODO
=
{
	{0, NULL, NULL},
	{0, NULL, NULL},
	{0, NULL, NULL},
	{0, NULL, NULL},
	{0, NULL, NULL}	
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_uart_fifo_setup(SAMD51_UART_FIFO *fifo, uint8_t *buf, size_t len)
{
	if ((fifo == NULL) || (buf == NULL)) {
		return AI_ERROR_NULL;
	}
	else if (len == 0) {
		return AI_ERROR_NOBUF;
	}

	fifo->buf = buf;
	fifo->len = len;
	fifo->read_ptr  = 0;
	fifo->write_ptr = 0;

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int samd51_uart_initialize(SAMD51_SERCOM sercom, uint32_t baudrate, SAMD51_SERCOM_PAD rx, SAMD51_SERCOM_PAD tx, SAMD51_UART_FIFO *tx_fifo, SAMD51_UART_FIFO *rx_fifo)
{
	if (tx != SAMD51_SERCOM_PAD0) {
		return AI_ERROR_INVALID;
	}
	volatile REG_SERCOM_UART *reg_uart = _getRegUart(sercom);
	if (reg_uart == NULL) {
		return AI_ERROR_NODEV;
	}
	
	if ((tx_fifo == NULL) || (rx_fifo == NULL) ){
		return AI_ERROR_NULL;
	}

	//J FIFO を登録する
	sUartCtx[sercom].tx_fifo  = tx_fifo;
	sUartCtx[sercom].rx_fifo  = rx_fifo;


	/*Interrupt Enable*/
	samd51_sercom_set_interrupt_handler(sercom, 1, _interrupt_handler_txc);
	samd51_sercom_set_interrupt_handler(sercom, 2, _interrupt_handler_rxc);


	/* CTRLA : 34.8.1 Control A */
	const uint32_t DORD = (1 << 30);	// LSB First
	const uint32_t CMODE = (0 << 28);	// Asynchronous mode
	const uint32_t FORM = (0 << 24);	// USART frame
	const uint32_t SAMPA = (0 << 22);	// over sampling setting
	uint32_t rxpo = (uint32_t)rx << 20;
	uint32_t txpo = (0x0 << 16);
	const uint32_t SAMPR = (0 << 13); // 16x over sampling with arithmetic baud rate generation.
	const uint32_t MODE = (1 << 2);
	const uint32_t ENABLE = (1 << 1);

	uint32_t ctrla = DORD | CMODE | FORM | SAMPA | rxpo | txpo | SAMPR | MODE;
	
	reg_uart->CTRLA = ctrla;
	
	
	/* CTRLB : 34.8.2 Control B */
	const uint32_t RXEN = (1 << 17);
	const uint32_t TXEN = (1 << 16);

	uint32_t ctrlb = RXEN | TXEN;
	
	/* CTRLC : 34.8.3 Control C */
	// Nothing to do

	/* BAUD */
	float refclk = 48000000; ///TODO Clockモジュールから持ってくる

	volatile uint32_t baud = 65536 * (1.0 - (16.0 * (float)baudrate / (float)refclk));

	reg_uart->BAUD  = baud;
	reg_uart->CTRLB = ctrlb;

	//J Enable UART
	reg_uart->CTRLA |= ENABLE;

	//J Set RXC and TXC flag
	reg_uart->INTENSET = (1 << 2) | (1 << 1);

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
void samd51_uart_finalize(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_UART *reg_uart = _getRegUart(sercom);
	if (reg_uart == NULL) {
		return;
	}

	samd51_sercom_reset_intterrupt(sercom);

	reg_uart->CTRLA = 1;
	while(reg_uart->SYNCBUSY & 1);
}


/*--------------------------------------------------------------------------*/
int samd51_uart_tx(SAMD51_SERCOM sercom, uint8_t *buf, size_t len)
{
	if (buf == NULL) {
		return AI_ERROR_NULL;
	}

	//J FIFOにデータを積む
	SAMD51_UART_FIFO *fifo = sUartCtx[sercom].tx_fifo;
	while (len--) {
		samd51_uart_fifo_enqueue(fifo, *buf++);
	}
	
	//J 通信が始まっていなければ始める
	volatile REG_SERCOM_UART *reg_uart = _getRegUart(sercom);
	reg_uart->INTENCLR = (1 << 1);
	if (sUartCtx[sercom].in_tx_sequence == 0) {
		sUartCtx[sercom].in_tx_sequence = 1;

		uint8_t data = samd51_uart_fifo_dequeue(fifo);
		reg_uart->INTENSET = (1 << 1);
		
		reg_uart->DATA = data;
	}
	else {
		reg_uart->INTENSET = (1 << 1);	
	}
	
	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int samd51_uart_try_rx(SAMD51_SERCOM sercom, uint8_t *buf)
{
	if (buf == NULL) {
		return AI_ERROR_NULL;
	}
	
	SAMD51_UART_FIFO *fifo = sUartCtx[sercom].rx_fifo;
	if (samd51_uart_fifo_is_empty(fifo)) {
		return AI_ERROR_NOBUF;
	}
	*buf = samd51_uart_fifo_dequeue(fifo);

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int samd51_uart_rx(SAMD51_SERCOM sercom, uint8_t *buf, size_t len)
{
	if (buf == NULL) {
		return AI_ERROR_NULL;
	}

	SAMD51_UART_FIFO *fifo = sUartCtx[sercom].rx_fifo;
	while (len--) {
		volatile int ret = 0;
		while ((ret = samd51_uart_fifo_is_empty(fifo)));

		*buf++ = samd51_uart_fifo_dequeue(fifo);
	}

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int samd51_uart_putc(SAMD51_SERCOM sercom, const char c)
{
	return samd51_uart_tx(sercom, (uint8_t *)&c, sizeof(c));
}

/*--------------------------------------------------------------------------*/
int samd51_uart_puts(SAMD51_SERCOM sercom, const char *str)
{
	if (str == NULL) {
		return AI_ERROR_NULL;
	}	
	
	while (*str != '\0') {
		int ret = samd51_uart_putc(sercom, *str);
		if (ret != AI_OK) {
			return ret;
		}
		
		str++;
	}
	
	return AI_OK;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _interrupt_handler_txc(SAMD51_SERCOM sercom)
{
	SAMD51_UART_FIFO *fifo = sUartCtx[sercom].tx_fifo;
	if (fifo == NULL) {
		return;
	}
	
	volatile REG_SERCOM_UART *reg_uart = _getRegUart(sercom);

	if (samd51_uart_fifo_is_empty(fifo)) {
		reg_uart->INTFLAG |= (1<<1); //Clear TXC flag
		sUartCtx[sercom].in_tx_sequence = 0;
		return;
	}

	uint8_t data = samd51_uart_fifo_dequeue(fifo);
	reg_uart->DATA = data;
	
	return;
}


/*--------------------------------------------------------------------------*/
static void _interrupt_handler_rxc(SAMD51_SERCOM sercom)
{
	SAMD51_UART_FIFO *fifo = sUartCtx[sercom].rx_fifo;
	if (fifo == NULL) {
		return;
	}
	
	volatile REG_SERCOM_UART *reg_uart = _getRegUart(sercom);

	uint8_t data = reg_uart->DATA;

	if (!samd51_uart_fifo_is_full(fifo)) {
		samd51_uart_fifo_enqueue(fifo, data);
	}
	
	return;
}

/*--------------------------------------------------------------------------*/
int samd51_uart_fifo_is_empty(SAMD51_UART_FIFO *fifo)
{
	if (fifo == NULL) {
		return 1; //J 
	}

	return (fifo->write_ptr == fifo->read_ptr);
}

/*--------------------------------------------------------------------------*/
int samd51_uart_fifo_is_full(SAMD51_UART_FIFO *fifo)
{
	if (fifo == NULL) {
		return 1; //J
	}

	uint32_t mask = (fifo->len - 1);
	uint32_t used_size = ((fifo->write_ptr + fifo->len) - fifo->read_ptr) & mask;

	return (used_size == mask);
}

/*--------------------------------------------------------------------------*/
#if 0
uint32_t samd51_uart_fifo_data_length(SAMD51_UART_FIFO *fifo)
{
	if (fifo == NULL) {
		return 1; //J
	}

	uint32_t mask = (fifo->len - 1);
	uint32_t used_size = ((fifo->write_ptr + fifo->len) - fifo->read_ptr) & mask;

	return used_size;	
}
#endif

/*--------------------------------------------------------------------------*/
uint8_t samd51_uart_fifo_dequeue(SAMD51_UART_FIFO *fifo)
{
	if (fifo == NULL) {
		return 0;
	}

	if (fifo->write_ptr == fifo->read_ptr) {
		return 0;
	}

	uint8_t data = fifo->buf[fifo->read_ptr];

	uint32_t mask = (fifo->len - 1);
	fifo->read_ptr = (fifo->read_ptr + 1) & mask;

	return data;
}

/*--------------------------------------------------------------------------*/
void samd51_uart_fifo_enqueue(SAMD51_UART_FIFO *fifo, uint8_t data)
{
	if (fifo == NULL) {
		return;
	}

	uint32_t mask = (fifo->len - 1);
	uint32_t used_size = ((fifo->write_ptr + fifo->len) - fifo->read_ptr) & mask;

	if (used_size == mask) {
		return;
	}
	
	fifo->buf[fifo->write_ptr] = data;
	fifo->write_ptr = (fifo->write_ptr + 1) & mask;

	return;	
}
