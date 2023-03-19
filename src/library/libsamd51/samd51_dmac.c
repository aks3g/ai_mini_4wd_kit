/*
 * samd51_dma.c
 *
 * Created: 2023/03/01 6:06:42
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
#include <samd51_dmac.h>



typedef struct REG_DMA_CH_t
{
	volatile uint32_t CHCTRLA;
	volatile uint8_t  CHCTRLB;
	volatile uint8_t  CHPRILVL;
	volatile uint8_t  CHEVCTRL;
	volatile uint8_t  RSVD_07;
	volatile uint8_t  RSVD_08;
	volatile uint8_t  RSVD_09;
	volatile uint8_t  RSVD_0A;
	volatile uint8_t  RSVD_0B;
	volatile uint8_t  CHINTENCLR;
	volatile uint8_t  CHINTENSET;
	volatile uint8_t  CHINTFLAG;
	volatile uint8_t  CHSTATUS;
} REG_DMA_CH;

typedef struct REG_DMAC_t
{
	volatile uint16_t CTRL;
	volatile uint16_t CRCCTRL;
	volatile uint32_t CRCDATAIN;
	volatile uint32_t CRCCHKSUM;
	volatile uint8_t  CRCSTATUS;
	volatile uint8_t  DBGCTRL;
	volatile uint8_t  RSVD_0E;
	volatile uint8_t  RSVD_0F;
	volatile uint32_t SWTRIGCTRL;
	volatile uint32_t PRICTRL0;
	volatile uint8_t  RSVD_18;
	volatile uint8_t  RSVD_19;
	volatile uint8_t  RSVD_1A;
	volatile uint8_t  RSVD_1B;
	volatile uint8_t  RSVD_1C;
	volatile uint8_t  RSVD_1D;
	volatile uint8_t  RSVD_1E;
	volatile uint8_t  RSVD_1F;
	volatile uint16_t INTPEND;
	volatile uint8_t  RSVD_22;
	volatile uint8_t  RSVD_23;
	volatile uint32_t INTSTATUS;
	volatile uint32_t BUSYCH;
	volatile uint32_t PENDCH;
	volatile uint32_t ACTIVE;
	volatile uint32_t BASEADDR;
	volatile uint32_t WRBADDR;
	volatile uint8_t  RSVD_3C;
	volatile uint8_t  RSVD_3D;
	volatile uint8_t  RSVD_3E;
	volatile uint8_t  RSVD_3F;
	volatile REG_DMA_CH ch[32];
} REG_DMAC;


#define DMAC_CTRL_ENABLE_POS							(1)
#define DMAC_CTRL_PRIOLITY_LEVEL_0_ENABLE_POS			(8)
#define DMAC_CTRL_PRIOLITY_LEVEL_1_ENABLE_POS			(9)
#define DMAC_CTRL_PRIOLITY_LEVEL_2_ENABLE_POS			(10)
#define DMAC_CTRL_PRIOLITY_LEVEL_3_ENABLE_POS			(11)


#define DMAC_CH_CTRLA_SW_RST_POS						(0)
#define DMAC_CH_CTRLA_ENABLE_POS						(1)
#define DMAC_CH_CTRLA_RUNSTDBY_POS						(6)
#define DMAC_CH_CTRLA_TRIGSRC_POS						(8)
#define DMAC_CH_CTRLA_TRIGACT_POS						(20)
#define DMAC_CH_CTRLA_BURSTLEN_POS						(24)
#define DMAC_CH_CTRLA_THRESHOLD_POS						(28)

#define DMAC_CH_INT_TERR_POS							(0)
#define DMAC_CH_INT_TCMPL_POS							(1)
#define DMAC_CH_INT_SUSP_POS							(2)

#define DMAC_DESC_BTCTRL_VALID_POS						(0)
#define DMAC_DESC_BTCTRL_EVOSEL_POS						(1)
#define DMAC_DESC_BTCTRL_BLOCKACT_POS					(3)
#define DMAC_DESC_BTCTRL_VEATSIZE_POS					(8)
#define DMAC_DESC_BTCTRL_SRCINC_POS						(10)
#define DMAC_DESC_BTCTRL_DSTINC_POS						(11)
#define DMAC_DESC_BTCTRL_STEPSEL_POS					(12)
#define DMAC_DESC_BTCTRL_STEPSIZE_POS					(13)


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#define SAMD51_DMAC_BASE		(0x4100A000UL)
#define SAMD51_DMAC_CHANNELS	(31)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
__attribute__((__aligned__(16))) static REG_DMA_DESC descs[SAMD51_DMAC_CHANNELS];
__attribute__((__aligned__(16))) static REG_DMA_DESC wbdescs[SAMD51_DMAC_CHANNELS];
static SAMD51_DMAC_TRANSACTION_DONE sCallbacks[SAMD51_DMAC_CHANNELS];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_dmac_initialize(void)
{
	memset(descs, 0, sizeof(descs));
	memset(wbdescs, 0, sizeof(wbdescs));

	volatile REG_DMAC *reg = (volatile REG_DMAC *)SAMD51_DMAC_BASE;
	reg->BASEADDR = (uint32_t)descs;
	reg->WRBADDR  = (uint32_t)wbdescs;

//	reg->PRICTRL0=0;
	NVIC_EnableIRQ(DMAC_0_IRQn);
	NVIC_EnableIRQ(DMAC_1_IRQn);
	NVIC_EnableIRQ(DMAC_2_IRQn);
	NVIC_EnableIRQ(DMAC_3_IRQn);
	NVIC_EnableIRQ(DMAC_4_IRQn);

	// Enable DMAC
	reg->CTRL = (0x0f<<DMAC_CTRL_PRIOLITY_LEVEL_0_ENABLE_POS) | (1<<DMAC_CTRL_ENABLE_POS);

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_dmac_transaction_start(int ch, SAMD51_DMAC_TRIGSRC trig, SAMD51_DMAC_TRIGACT act, REG_DMA_DESC *p_desc, SAMD51_DMAC_TRANSACTION_DONE cb)
{
	volatile REG_DMAC *reg = (volatile REG_DMAC *)SAMD51_DMAC_BASE;
	if (ch > SAMD51_DMAC_CHANNELS) {
		return AI_ERROR_INVALID;
	}
	// Copy Transfer Descriptor
	memcpy(&descs[ch], p_desc, sizeof(REG_DMA_DESC));

	if (cb) {
		sCallbacks[ch] = cb;
		reg->ch[ch].CHINTENSET = (1<<DMAC_CH_INT_TERR_POS) | (1<<DMAC_CH_INT_TCMPL_POS);
	}

	// DMA channel configuration
	reg->ch[ch].CHCTRLA = ((uint32_t)trig << DMAC_CH_CTRLA_TRIGSRC_POS) | 
						  ((uint32_t)act << DMAC_CH_CTRLA_TRIGACT_POS) |
						  ((uint32_t)0x0 << DMAC_CH_CTRLA_BURSTLEN_POS) |
						  (0<<DMAC_CH_CTRLA_ENABLE_POS);
	
	// Enable Channel
	reg->ch[ch].CHCTRLA |= (1<<DMAC_CH_CTRLA_ENABLE_POS);

	return 0;
}

/*--------------------------------------------------------------------------*/
int samd51_dmac_sw_trigger(uint8_t ch)
{
	volatile REG_DMAC *reg = (volatile REG_DMAC *)SAMD51_DMAC_BASE;
	if (ch > 31) {
		return AI_ERROR_INVALID;
	}

	reg->SWTRIGCTRL |= (1<<ch);

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DMAC_0_Handler(void)
{
	volatile REG_DMAC *reg = (volatile REG_DMAC *)SAMD51_DMAC_BASE;
	uint32_t status = reg->INTSTATUS;
	uint32_t pend   = reg->INTPEND;
	reg->INTPEND = pend;

	uint32_t ch = 0;
	if (status & (1<<ch)) {
		uint8_t ch_intflag = reg->ch[ch].CHINTFLAG;
		reg->ch[ch].CHINTFLAG = ch_intflag;
		if (ch_intflag & (1<<DMAC_CH_INT_TERR_POS) && sCallbacks[ch]) {
			sCallbacks[ch](-1);
		}
		else if (sCallbacks[ch]) {
			sCallbacks[ch](0);
		}
	}
}

void DMAC_1_Handler(void)
{
	volatile REG_DMAC *reg = (volatile REG_DMAC *)SAMD51_DMAC_BASE;
	uint32_t status = reg->INTSTATUS;
	uint32_t pend   = reg->INTPEND;
	reg->INTPEND = pend;

	uint32_t ch = 1;
	if (status & (1<<ch)) {
		uint8_t ch_intflag = reg->ch[ch].CHINTFLAG;
		reg->ch[ch].CHINTFLAG = ch_intflag;
		if (ch_intflag & (1<<DMAC_CH_INT_TERR_POS) && sCallbacks[ch]) {
			sCallbacks[ch](-1);
		}
		else if (sCallbacks[ch]) {
			sCallbacks[ch](0);
		}
	}
}

void DMAC_2_Handler(void)
{
	volatile REG_DMAC *reg = (volatile REG_DMAC *)SAMD51_DMAC_BASE;
	uint32_t status = reg->INTSTATUS;
	uint32_t pend   = reg->INTPEND;
	reg->INTPEND = pend;

	uint32_t ch = 2;
	if (status & (1<<ch)) {
		uint8_t ch_intflag = reg->ch[ch].CHINTFLAG;
		reg->ch[ch].CHINTFLAG = ch_intflag;
		if (ch_intflag & (1<<DMAC_CH_INT_TERR_POS) && sCallbacks[ch]) {
			sCallbacks[ch](-1);
		}
		else if (sCallbacks[ch]) {
			sCallbacks[ch](0);
		}
	}
}

void DMAC_3_Handler(void)
{
	volatile REG_DMAC *reg = (volatile REG_DMAC *)SAMD51_DMAC_BASE;
	uint32_t status = reg->INTSTATUS;
	uint32_t pend   = reg->INTPEND;
	reg->INTPEND = pend;

	uint32_t ch = 3;
	if (status & (1<<ch)) {
		uint8_t ch_intflag = reg->ch[ch].CHINTFLAG;
		reg->ch[ch].CHINTFLAG = ch_intflag;
		if (ch_intflag & (1<<DMAC_CH_INT_TERR_POS) && sCallbacks[ch]) {
			sCallbacks[ch](-1);
		}
		else if (sCallbacks[ch]) {
			sCallbacks[ch](0);
		}
	}
}

void DMAC_4_Handler(void)
{
	volatile REG_DMAC *reg = (volatile REG_DMAC *)SAMD51_DMAC_BASE;
	uint32_t status = reg->INTSTATUS & 0xfffffff0;
//	uint32_t pend   = reg->INTPEND;

	uint32_t ch = 4;
	while (status) {
		if (status & (1<<ch)) {
			uint8_t ch_intflag = reg->ch[ch].CHINTFLAG;
			reg->ch[ch].CHINTFLAG = ch_intflag;
			if (ch_intflag & (1<<DMAC_CH_INT_TERR_POS) && sCallbacks[ch]) {
				sCallbacks[ch](-1);
			}
			else if (sCallbacks[ch]) {
				sCallbacks[ch](0);
			}
			
			status &=~(1<<ch);
		}
		ch++;
	}
}