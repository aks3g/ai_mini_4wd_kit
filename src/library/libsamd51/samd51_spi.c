/*
 * samd51_spi.c
 *
 * Created: 2021/07/02 5:32:42
 *  Author: kiyot
 */ 
#include <string.h>

#include <sam.h>

#include <samd51_error.h>
#include <samd51_spi.h>

typedef struct REG_SERCOM_SPI_t
{
	volatile uint32_t CTRLA;			// 0x00
	volatile uint32_t CTRLB;			// 0x04
	volatile uint32_t CTRLC;			// 0x08
	volatile uint8_t  BAUD;				// 0x0C
	volatile uint8_t  _rserved[3];		// 0x0D
	volatile uint8_t  _rserved0[4];		// 0x10
	volatile uint8_t  INTENCLR;			// 0x14
	volatile uint8_t  _reserved1;		// 0x15
	volatile uint8_t  INTENSET;			// 0x16
	volatile uint8_t  _reserved2;		// 0x17
	volatile uint8_t  INTFLAG;			// 0x18
	volatile uint8_t  _reserved3;		// 0x19
	volatile uint16_t STATUS;			// 0x1A
	volatile uint32_t SYNCBUSY;			// 0x1C
	volatile uint8_t  _reserved4[2];	// 0x20
	volatile uint16_t LENGTH;			// 0x20
	volatile uint32_t ADDR;				// 0x24
	volatile uint32_t DATA;				// 0x28
} REG_SERCOM_SPI;

/* CTRLA */
#define SAMD51_SERCOM_SPI_CTRLA_SWRST_POS				(0)
#define SAMD51_SERCOM_SPI_CTRLA_ENABLE_POS				(1)
#define SAMD51_SERCOM_SPI_CTRLA_MODE_POS				(2)
#define SAMD51_SERCOM_SPI_CTRLA_RUNSTDBY_POS			(7)
#define SAMD51_SERCOM_SPI_CTRLA_IBON_POS				(8)
#define SAMD51_SERCOM_SPI_CTRLA_DOPO_POS				(16)
#define SAMD51_SERCOM_SPI_CTRLA_DIPO_POS				(20)
#define SAMD51_SERCOM_SPI_CTRLA_FORM_POS				(24)
#define SAMD51_SERCOM_SPI_CTRLA_CPHA_POS				(28)
#define SAMD51_SERCOM_SPI_CTRLA_CPOL_POS				(29)
#define SAMD51_SERCOM_SPI_CTRLA_DORD_POS				(30)

/* CTRLB */
#define SAMD51_SERCOM_SPI_CTRLB_CHSIZE_POS				(0)
#define SAMD51_SERCOM_SPI_CTRLB_PLOADEN_POS				(6)
#define SAMD51_SERCOM_SPI_CTRLB_SSDE_POS				(9)
#define SAMD51_SERCOM_SPI_CTRLB_MSSEN_POS				(13)
#define SAMD51_SERCOM_SPI_CTRLB_AMODE_POS				(14)
#define SAMD51_SERCOM_SPI_CTRLB_RXEN_POS				(17)

/* CTRLC */
#define SAMD51_SERCOM_SPI_CTRLC_ICSPACE_POS				(0)
#define SAMD51_SERCOM_SPI_CTRLC_DATA32B_POS				(24)

/* INTENCLR/INTENSET/INTFLAG */
#define SAMD51_SERCOM_SPI_INT_DRE_POS					(0)
#define SAMD51_SERCOM_SPI_INT_TXC_POS					(1)
#define SAMD51_SERCOM_SPI_INT_RXC_POS					(2)
#define SAMD51_SERCOM_SPI_INT_SSL_POS					(3)
#define SAMD51_SERCOM_SPI_INT_ERROR_POS					(7)

/* STATUS */
#define SAMD51_SERCOM_SPI_STATUS_BUFOVF_POS				(2)
#define SAMD51_SERCOM_SPI_STATUS_LENERR_POS				(11)

/* SYNCBUSY */
#define SAMD51_SERCOM_SPI_SYNCBUSY_SWRST_POS			(0)
#define SAMD51_SERCOM_SPI_SYNCBUSY_ENABLE_POS			(1)
#define SAMD51_SERCOM_SPI_SYNCBUSY_CTRLB_POS			(2)
#define SAMD51_SERCOM_SPI_SYNCBUSY_LENGTH_POS			(4)

/* LENGTH */
#define SAMD51_SERCOM_SPI_LENGTH_LEN_POS				(0)
#define SAMD51_SERCOM_SPI_LENGTH_LENEN_POS				(8)

#define SAMD51_SERCOM_SPI_LENGTH_LEN_MASK				(0xff)

/* ADDR */
#define SAMD51_SERCOM_SPI_ADDR_ADDR_POS					(0)
#define SAMD51_SERCOM_SPI_ADDR_MASK_POS					(16)


#define SAMD51_SERCOM_SPI0_BASE				(0x40003000UL)
#define SAMD51_SERCOM_SPI1_BASE				(0x40003400UL)
#define SAMD51_SERCOM_SPI2_BASE				(0x41012000UL)
#define SAMD51_SERCOM_SPI3_BASE				(0x41014000UL)
#define SAMD51_SERCOM_SPI4_BASE				(0x43000000UL)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static volatile REG_SERCOM_SPI *_getRegSpi(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_SPI *reg = NULL;
	
	switch (sercom) {
	case SAMD51_SERCOM0:
		reg = (volatile REG_SERCOM_SPI *)SAMD51_SERCOM_SPI0_BASE;
		break;
	case SAMD51_SERCOM1:
		reg = (volatile REG_SERCOM_SPI *)SAMD51_SERCOM_SPI1_BASE;
		break;
	case SAMD51_SERCOM2:
		reg = (volatile REG_SERCOM_SPI *)SAMD51_SERCOM_SPI2_BASE;
		break;
	case SAMD51_SERCOM3:
		reg = (volatile REG_SERCOM_SPI *)SAMD51_SERCOM_SPI3_BASE;
		break;
	case SAMD51_SERCOM4:
		reg = (volatile REG_SERCOM_SPI *)SAMD51_SERCOM_SPI4_BASE;
		break;
	default:
		break;
	}
	
	return reg;
}


typedef struct Samd51SpiTrxContext_t
{
	volatile int busy;
	
	const uint8_t *txbuf;
	size_t   txbuf_size;
	size_t   txbuf_ptr;

	uint8_t *rxbuf;
	size_t   rxbuf_size;
	size_t   rxbuf_ptr;

	SAMD51_SPI_TXRX_DONE_CB cb;
} Samd51SpiTrxContext;

Samd51SpiTrxContext sSpiCtx;
static SAMD51_GPIO_PORT sSsPort;

/*--------------------------------------------------------------------------*/
static void _spi_data_register_empty_interrupt_handler(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_SPI *reg = _getRegSpi(sercom);

	// End of Sending
	if (sSpiCtx.txbuf_ptr == sSpiCtx.txbuf_size) {
		sSpiCtx.txbuf = NULL;
		reg->INTENCLR = (1 << SAMD51_SERCOM_SPI_INT_DRE_POS);
	}
	else {
		if (sSpiCtx.txbuf == NULL) {
//			reg->DATA = 0xff;
			reg->DATA = 0x00;
			sSpiCtx.txbuf_ptr++;
		}
		else {
			reg->DATA = sSpiCtx.txbuf[sSpiCtx.txbuf_ptr++];
		}
	}
}

/*--------------------------------------------------------------------------*/
static void _spi_transmit_complete_interrupt_handler(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_SPI *reg = _getRegSpi(sercom);
	reg->INTFLAG = (1 << SAMD51_SERCOM_SPI_INT_TXC_POS);

	// End of Tx
	if ((sSpiCtx.rxbuf == NULL) && (sSpiCtx.txbuf_ptr == sSpiCtx.txbuf_size)) {
		if (sSpiCtx.cb){
			sSpiCtx.cb(0, NULL, 0);
		}
		memset (&sSpiCtx, 0, sizeof(sSpiCtx));
		samd51_gpio_output(sSsPort, 1);
	}
	
	return;
}

/*--------------------------------------------------------------------------*/
static void _spi_receive_complete_interrupt_handler(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_SPI *reg = _getRegSpi(sercom);
	volatile uint32_t data = reg->DATA;

	if (sSpiCtx.rxbuf == NULL) {		
		return;	
	}
	sSpiCtx.rxbuf[sSpiCtx.rxbuf_ptr++] = (uint8_t)data;

	// End of Rx
	if (sSpiCtx.rxbuf_ptr >= sSpiCtx.rxbuf_size) {
		reg->CTRLB &= ~(1 << SAMD51_SERCOM_SPI_CTRLB_RXEN_POS);
		if (sSpiCtx.cb){
			sSpiCtx.cb(0, sSpiCtx.rxbuf, sSpiCtx.rxbuf_ptr);
		}
		memset (&sSpiCtx, 0, sizeof(sSpiCtx));
		samd51_gpio_output(sSsPort, 1);
	}
}


/*--------------------------------------------------------------------------*/
int samd51_spi_initialize(SAMD51_SERCOM sercom, SAMD51_GPIO_PORT ss, uint32_t peripheral_clock, uint32_t sclk_clock)
{
	volatile REG_SERCOM_SPI *reg = _getRegSpi(sercom);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}
	
	sSsPort = ss;
	memset (&sSpiCtx, 0, sizeof(Samd51SpiTrxContext));

	samd51_gpio_output(sSsPort, 1);

	if (reg->CTRLA & (1 << SAMD51_SERCOM_SPI_CTRLA_ENABLE_POS)) {
		reg->CTRLA |= (1 << SAMD51_SERCOM_SPI_CTRLA_SWRST_POS);
		
		while ((reg->SYNCBUSY & (1 << SAMD51_SERCOM_SPI_SYNCBUSY_SWRST_POS)));
	}

	//J Interrupt ‚ÌEnable
	samd51_sercom_set_interrupt_handler(sercom, 0, _spi_data_register_empty_interrupt_handler);
	samd51_sercom_set_interrupt_handler(sercom, 1, _spi_transmit_complete_interrupt_handler);
	samd51_sercom_set_interrupt_handler(sercom, 2, _spi_receive_complete_interrupt_handler);

	// Enable SPI
	reg->CTRLA = (3 << SAMD51_SERCOM_SPI_CTRLA_MODE_POS) |
				 (0 << SAMD51_SERCOM_SPI_CTRLA_FORM_POS) |
				 (3 << SAMD51_SERCOM_SPI_CTRLA_DIPO_POS) | // PAD[3] is MISO
				 (0 << SAMD51_SERCOM_SPI_CTRLA_DOPO_POS) | // PAD[0] is MOSI, PAD[1] is SCK
				 (0 << SAMD51_SERCOM_SPI_CTRLA_DORD_POS) |
				 (0 << SAMD51_SERCOM_SPI_CTRLA_CPOL_POS) |
				 (1 << SAMD51_SERCOM_SPI_CTRLA_CPHA_POS);

	reg->CTRLB = (0 << SAMD51_SERCOM_SPI_CTRLB_MSSEN_POS) |
				 (0 << SAMD51_SERCOM_SPI_CTRLB_RXEN_POS);

	reg->BAUD = (uint8_t)(peripheral_clock / (2*sclk_clock)) - 1;

	reg->INTENSET = (1 << SAMD51_SERCOM_SPI_INT_RXC_POS) |
					(1 << SAMD51_SERCOM_SPI_INT_TXC_POS) ;

	reg->CTRLA |= (1 << SAMD51_SERCOM_SPI_CTRLA_ENABLE_POS);
	while ((reg->SYNCBUSY & (1 << SAMD51_SERCOM_SPI_SYNCBUSY_ENABLE_POS)));
	
	return AI_OK;
}

/*--------------------------------------------------------------------------*/
void samd51_spi_finalize(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_SPI *reg = _getRegSpi(sercom);
	if (reg == NULL) {
		return;
	}

	samd51_sercom_reset_intterrupt(sercom);

	reg->CTRLA |= (1 << SAMD51_SERCOM_SPI_CTRLA_SWRST_POS);	
	while ((reg->SYNCBUSY & (1 << SAMD51_SERCOM_SPI_SYNCBUSY_SWRST_POS)));
}

/*--------------------------------------------------------------------------*/
int samd51_spi_txrx(SAMD51_SERCOM sercom, const uint8_t *txbuf, uint8_t *rxbuf, const size_t len, SAMD51_SPI_TXRX_DONE_CB callback)
{
	volatile REG_SERCOM_SPI *reg = _getRegSpi(sercom);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	if ((txbuf == NULL && rxbuf == NULL) || len == 0) {
		return AI_ERROR_NOBUF;
	}

	if (sSpiCtx.busy) {
		return AI_ERROR_BUSY;
	}
	sSpiCtx.busy = 1;
	
	// Setup TRX context.
	sSpiCtx.txbuf = txbuf;
	sSpiCtx.txbuf_ptr = 0;
	sSpiCtx.txbuf_size = len;

	sSpiCtx.rxbuf = rxbuf;
	sSpiCtx.rxbuf_ptr = 0;
	sSpiCtx.rxbuf_size = len;

	sSpiCtx.cb = callback;

	if (rxbuf) {
		reg->CTRLB |= (1 << SAMD51_SERCOM_SPI_CTRLB_RXEN_POS);
	}

	samd51_gpio_output(sSsPort, 0);

	// Start Transaction.
	// reg->DATA = sCtx.txbuf[sCtx.txbuf_ptr++];
	reg->INTENSET = (1 << SAMD51_SERCOM_SPI_INT_DRE_POS);

	if (callback == NULL) {
		while (sSpiCtx.busy);
	}

	return AI_OK;
}
