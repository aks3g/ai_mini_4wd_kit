/*
 * samd51_qspi.c
 *
 * Created: 2023/02/21 4:56:31
 *  Author: kiyot
 */ 
#include <sam.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

//J Debug INFOê∂ê¨ÇÃÇΩÇﬂÇ…ïKóv
#include <stdio.h>
#include <stdarg.h>

#include <samd51_error.h>
#include <samd51_qspi.h>
#include <samd51_dmac.h>


typedef struct REG_QSPI_t {
	volatile uint32_t CTRLA;
	volatile uint32_t CTRLB;
	volatile uint32_t BAUD;
	volatile uint32_t RXDATA;
	volatile uint32_t TXDATA;
	volatile uint32_t INTENCLR;
	volatile uint32_t INTENSET;
	volatile uint32_t INTFLAG;
	volatile uint32_t STATUS;
	volatile uint32_t RSVD_24;
	volatile uint32_t RSVD_28;
	volatile uint32_t RSVD_2C;
	volatile uint32_t INSTRADDR;
	volatile uint32_t INSTRCTRL;
	volatile uint32_t INSTRFRAME;
	volatile uint32_t RSVD_3C;
	volatile uint32_t SCRAMBCTRL;
	volatile uint32_t SCRAMBKEY;
} REG_QSPI;

/* CTRLA */
#define SAMD51_QSPI_CTRLA_SWRST_POS				(0)
#define SAMD51_QSPI_CTRLA_ENABLE_POS			(1)
#define SAMD51_QSPI_CTRLA_LASTXFER_POS			(24)

/* CTRLB */
#define SAMD51_QSPI_CTRLB_MODE_POS				(0)
#define SAMD51_QSPI_CTRLB_LOOPEN_POS			(1)
#define SAMD51_QSPI_CTRLB_WDRBT_POS				(2)
#define SAMD51_QSPI_CTRLB_SMEMREG_POS			(3)
#define SAMD51_QSPI_CTRLB_CSMODE_POS			(4)
#define SAMD51_QSPI_CTRLB_DATALEN_POS			(8)
#define SAMD51_QSPI_CTRLB_DLYBCT_POS			(16)
#define SAMD51_QSPI_CTRLB_DLYCS_POS				(24)

/* BAUD */
#define SAMD51_QSPI_BAUD_CPOL_POS				(0)
#define SAMD51_QSPI_BAUD_CPHA_POS				(1)
#define SAMD51_QSPI_BAUD_BAUD_POS				(8)
#define SAMD51_QSPI_BAUD_DLYBS_POS				(16)

/* INTxxxx */
#define SAMD51_QSPI_INT_RXC_POS					(0)
#define SAMD51_QSPI_INT_DRE_POS					(1)
#define SAMD51_QSPI_INT_TXC_POS					(2)
#define SAMD51_QSPI_INT_ERROR_POS				(3)
#define SAMD51_QSPI_INT_CSRISE_POS				(8)
#define SAMD51_QSPI_INT_INSTREND_POS			(10)

/* STATUS */
#define SAMD51_QSPI_STATUS_ENABLE_POS			(1)
#define SAMD51_QSPI_STATUS_CSSTATUS_POS			(9)

/* INSTRCTRL */
#define SAMD51_QSPI_INSTRCTRL_INSTR_POS			(0)
#define SAMD51_QSPI_INSTRCTRL_OPTCODE_POS		(16)

/* INSTRFRAME */
#define SAMD51_QSPI_INSTRFRAME_WIDTH_POS		(0)
#define SAMD51_QSPI_INSTRFRAME_INSTREN_POS		(4)
#define SAMD51_QSPI_INSTRFRAME_ADDREN_POS		(5)
#define SAMD51_QSPI_INSTRFRAME_OPTCODEEN_POS	(6)
#define SAMD51_QSPI_INSTRFRAME_DATAEN_POS		(7)
#define SAMD51_QSPI_INSTRFRAME_OPTCODELEN_POS	(8)
#define SAMD51_QSPI_INSTRFRAME_ADDRLEN_POS		(10)
#define SAMD51_QSPI_INSTRFRAME_TFRTYPE_POS		(12)
#define SAMD51_QSPI_INSTRFRAME_CRMODE_POS		(14)
#define SAMD51_QSPI_INSTRFRAME_DDREN_POS		(15)
#define SAMD51_QSPI_INSTRFRAME_DUMMYLEN_POS		(16)

#define SAMD51_QSPI_BASE						(0x42003400UL)
#define SAMD51_QSPI_ACCESS_BASE					(0x04000000UL)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static REG_DMA_DESC sDmacDesc;

static SAMD51_QSPI_TRANSACTION_DONE s_transaction_done;
static volatile int sQspiBusy = 0;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _qspi_dmac_transaction_done(int status)
{
	volatile REG_QSPI *reg = (volatile REG_QSPI *)(SAMD51_QSPI_BASE);
	reg->CTRLA = (1<<SAMD51_QSPI_CTRLA_LASTXFER_POS) | (1 << SAMD51_QSPI_CTRLA_ENABLE_POS);
	while ((reg->INTFLAG & (1<<SAMD51_QSPI_INT_INSTREND_POS)) == 0);
		
	sQspiBusy = 0;
	s_transaction_done(status);

	return;
}


/*--------------------------------------------------------------------------*/
static int _qspi_copy(uint8_t *src, uint8_t *dst, size_t len, SAMD51_QSPI_TRANSACTION_DONE cb)
{
	if (len == 0) {
		return AI_ERROR_NOBUF;
	}
	if (src == NULL || dst == NULL) {
		return AI_ERROR_NULL;
	}

	if (cb) {		
		sDmacDesc.BTCTRL.bf.STEPSIZE = 0;
		sDmacDesc.BTCTRL.bf.STEPSEL  = 0;
		sDmacDesc.BTCTRL.bf.DSTINC = 1;
		sDmacDesc.BTCTRL.bf.SRCINC = 1;
		sDmacDesc.BTCTRL.bf.BEATSIZE = 0; // 8byte
		sDmacDesc.BTCTRL.bf.BLOCKACT = 0; // No Interrupt
		sDmacDesc.BTCTRL.bf.EVOSEL = 0; // Disable
		sDmacDesc.BTCTRL.bf.VALID = 1;
		
		sDmacDesc.BTCNT = (uint16_t)len;
		sDmacDesc.DSTADDR = dst + len;
		sDmacDesc.SRCADDR = src + len;
		sDmacDesc.next = NULL;
		
		int ret = samd51_dmac_transaction_start(0, SAMD51_DMAC_TRIGSRC_DISABLE, SAMD51_DMAC_TRIGACT_BLOCK, &sDmacDesc, _qspi_dmac_transaction_done);
		if (ret != AI_OK) {
			return ret;
		}
		s_transaction_done = cb;
		samd51_dmac_sw_trigger(0);
	}
	else {
		while(len--) {
			*dst++ =  *src++;
		}
	}

	return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_qspi_initialize(uint8_t baud_div, uint8_t delay_before_sck, uint8_t minimum_inactive_CS_delay, uint8_t delay_between_consecutive_transfers, SAMD51_QSPI_CLOCK_MODE mode)
{
	volatile REG_QSPI *reg = (volatile REG_QSPI *)(SAMD51_QSPI_BASE);
	
	reg->CTRLB = (0 << SAMD51_QSPI_CTRLB_SMEMREG_POS) | (1 << SAMD51_QSPI_CTRLB_MODE_POS) | ((uint32_t)minimum_inactive_CS_delay << 24 ) | ((uint32_t)delay_between_consecutive_transfers << 16);	
	reg->CTRLA = (1 << SAMD51_QSPI_CTRLA_ENABLE_POS);
	reg->BAUD = (baud_div << SAMD51_QSPI_BAUD_BAUD_POS) | (delay_before_sck << SAMD51_QSPI_BAUD_DLYBS_POS) | ((uint32_t)mode);

	return AI_OK;	
}


/*--------------------------------------------------------------------------*/
int samd51_qspi_exec_instruction(SAMD51_QSPI_INSTRUCTION *inst, uint8_t *wbuf, size_t wbuf_len, uint8_t *rbuf, size_t rbuf_len, SAMD51_QSPI_TRANSACTION_DONE cb)
{
	volatile REG_QSPI *reg = (volatile REG_QSPI *)(SAMD51_QSPI_BASE);
	if (sQspiBusy) return AI_ERROR_BUSY;

	sQspiBusy = 1;

	reg->INTFLAG = 0;

	reg->INSTRADDR = inst->addr;
	reg->INSTRCTRL = inst->code.reg;
	reg->INSTRFRAME = inst->frame.reg;

	__DSB();
	__ISB();

	if (wbuf) {
		_qspi_copy(wbuf, (uint8_t *)(SAMD51_QSPI_ACCESS_BASE + (inst->addr & 0x00ffffff)), wbuf_len, cb);
	}
	else if (rbuf){
		_qspi_copy((uint8_t *)(SAMD51_QSPI_ACCESS_BASE+(inst->addr & 0x00ffffff)), rbuf, rbuf_len, cb);
	}

	//J End of instruction	
	if (cb == NULL) {
		reg->CTRLA = (1<<SAMD51_QSPI_CTRLA_LASTXFER_POS) | (1 << SAMD51_QSPI_CTRLA_ENABLE_POS);
		while ((reg->INTFLAG & (1<<SAMD51_QSPI_INT_INSTREND_POS)) == 0);

		sQspiBusy = 0;
	}

	return AI_OK;	
}

/*--------------------------------------------------------------------------*/
int samd51_qspi_exec_instruction_until_bit_set(SAMD51_QSPI_INSTRUCTION *inst, uint8_t set_mask, uint32_t timeout)
{
	volatile REG_QSPI *reg = (volatile REG_QSPI *)(SAMD51_QSPI_BASE);
	if (sQspiBusy) return AI_ERROR_BUSY;

	reg->INTFLAG = 0;

	reg->INSTRADDR = inst->addr;
	reg->INSTRCTRL = inst->code.reg;
	reg->INSTRFRAME = inst->frame.reg;

	__DSB();
	__ISB();

	uint8_t *pdummy = (uint8_t *)SAMD51_QSPI_ACCESS_BASE;
	volatile uint8_t val=0;
	do {
		val = *pdummy++;
	}while ((val & set_mask) == 0);

	//J End of instruction
	reg->CTRLA = (1<<SAMD51_QSPI_CTRLA_LASTXFER_POS) | (1 << SAMD51_QSPI_CTRLA_ENABLE_POS);
	while ((reg->INTFLAG & (1<<SAMD51_QSPI_INT_INSTREND_POS)) == 0);

	return 0;	
}

/*--------------------------------------------------------------------------*/
int samd51_qspi_exec_instruction_until_bit_clear(SAMD51_QSPI_INSTRUCTION *inst, uint8_t clear_mask, uint32_t timeout)
{
	volatile REG_QSPI *reg = (volatile REG_QSPI *)(SAMD51_QSPI_BASE);
	if (sQspiBusy) return AI_ERROR_BUSY;

	reg->INTFLAG = 0;

	reg->INSTRADDR = inst->addr;
	reg->INSTRCTRL = inst->code.reg;
	reg->INSTRFRAME = inst->frame.reg;

	__DSB();
	__ISB();

	uint8_t *pdummy = (uint8_t *)SAMD51_QSPI_ACCESS_BASE;
	volatile uint8_t val=0;
	do {
		val = *pdummy++;
	}while ((val & clear_mask) == clear_mask);

	//J End of instruction
	reg->CTRLA = (1<<SAMD51_QSPI_CTRLA_LASTXFER_POS) | (1 << SAMD51_QSPI_CTRLA_ENABLE_POS);
	while ((reg->INTFLAG & (1<<SAMD51_QSPI_INT_INSTREND_POS)) == 0);

	return 0;
}

int samd51_qspi_is_busy(void)
{
	if (sQspiBusy) return 1;
	else           return 0;	
}