/*
 * samd51_i2c.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <string.h>

#include <sam.h>

#include <samd51_error.h>
#include <samd51_i2c.h>


typedef struct REG_SERCOM_I2C_t
{
	volatile uint32_t CTRLA;
	volatile uint32_t CTRLB;
	volatile uint32_t CTRLC;
	volatile uint32_t BAUD;
	volatile uint8_t _rserved0[4];
	volatile uint8_t INTENCLR;
	volatile uint8_t _reserved1;
	volatile uint8_t INTENSET;
	volatile uint8_t _reserved2;
	volatile uint8_t INTFLAG;
	volatile uint8_t _reserved3;
	volatile uint16_t STATUS;
	volatile uint32_t SYNCBUSY;
	volatile uint8_t _reserved4[4];
	volatile uint32_t ADDR;
	volatile uint32_t DATA;
} REG_SERCOM_I2C;

/* CTRLA */
#define SAMD51_SERCOM_I2C_SWRST_POS				(0)
#define SAMD51_SERCOM_I2C_ENABLE_POS			(1)
#define SAMD51_SERCOM_I2C_MODE_POS				(2)
#define SAMD51_SERCOM_I2C_RUNSTDBY_POS			(7)
#define SAMD51_SERCOM_I2C_PINOUT_POS			(16)
#define SAMD51_SERCOM_I2C_SDAHOLD_POS			(20)
#define SAMD51_SERCOM_I2C_MEXTTOEN_POS			(22)
#define SAMD51_SERCOM_I2C_SEXTTOEN_POS			(23)
#define SAMD51_SERCOM_I2C_SPEED_POS				(24)
#define SAMD51_SERCOM_I2C_SCLSM_POS				(27)
#define SAMD51_SERCOM_I2C_INACTOUT_POS			(28)
#define SAMD51_SERCOM_I2C_LOWTOUT_POS			(30)

/* CTRLB */
#define SAMD51_SERCOM_I2C_SMEN_POS				(8)
#define SAMD51_SERCOM_I2C_QCEN_POS				(9)
#define SAMD51_SERCOM_I2C_CMD_POS				(16)
#define SAMD51_SERCOM_I2C_ACKACT_POS			(18)

/* CTRLC */
#define SAMD51_SERCOM_I2C_DATA32B_POS			(24)

/* BAUD */
#define SAMD51_SERCOM_I2C_BAUD_POS				(0)
#define SAMD51_SERCOM_I2C_BAUDLOW_POS			(8)
#define SAMD51_SERCOM_I2C_HSBAUD_POS			(16)
#define SAMD51_SERCOM_I2C_HSBAUDLOW_POS			(24)

/* INTENCLR, INTENSET, INTFLAG */
#define SAMD51_SERCOM_I2C_INTFLAG_MB_POS		(0)
#define SAMD51_SERCOM_I2C_INTFLAG_SB_POS		(1)
#define SAMD51_SERCOM_I2C_INTFLAG_ERROR_POS		(7)

/* STATUS */
#define SAMD51_SERCOM_I2C_BUSERR_POS			(0)
#define SAMD51_SERCOM_I2C_ARBLOST_POS			(1)
#define SAMD51_SERCOM_I2C_RXNACK_POS			(2)
#define SAMD51_SERCOM_I2C_BUSSTATE_POS			(4)
#define SAMD51_SERCOM_I2C_STAT_LOWTOUT_POS			(6)
#define SAMD51_SERCOM_I2C_CLKHOLD_POS			(7)
#define SAMD51_SERCOM_I2C_MEXTTOUT_POS			(8)
#define SAMD51_SERCOM_I2C_SEXTTOUT_POS			(9)
#define SAMD51_SERCOM_I2C_LENERR_POS			(10)

/* SYNCBUSY */
#define SAMD51_SERCOM_I2C_SWRST_POS				(0)
#define SAMD51_SERCOM_I2C_ENABLE_POS			(1)
#define SAMD51_SERCOM_I2C_SYSOP_POS				(2)

/* ADDR */
#define SAMD51_SERCOM_I2C_ADDR_POS				(0)
#define SAMD51_SERCOM_I2C_LENEN_POS				(13)
#define SAMD51_SERCOM_I2C_HS_POS				(14)
#define SAMD51_SERCOM_I2C_TENBITEN_POS			(15)
#define SAMD51_SERCOM_I2C_LEN_POS				(16)



#define SAMD51_SERCOM_I2C0_BASE				(0x40003000UL)
#define SAMD51_SERCOM_I2C1_BASE				(0x40003400UL)
#define SAMD51_SERCOM_I2C2_BASE				(0x41012000UL)
#define SAMD51_SERCOM_I2C3_BASE				(0x41014000UL)
#define SAMD51_SERCOM_I2C4_BASE				(0x43000000UL)


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef struct SAMD51_I2C_COMMUNICATION_CONTEXT_t
{
	SAMD51_I2C_DONE_CB callback;
	const uint8_t *tx_buf;
	size_t   tx_size;
	
	uint8_t *rx_buf;
	size_t   rx_size;
	
	int   index;
	
} SAMD51_I2C_COMMUNICATION_CONTEXT;

static SAMD51_I2C_COMMUNICATION_CONTEXT sI2cCtx[5];
static volatile int sI2cInterfaceIsBusy[5] = {0, 0, 0, 0, 0};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static volatile REG_SERCOM_I2C *_getRegI2C(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_I2C *reg = NULL;
	
	switch (sercom) {
	case SAMD51_SERCOM0:
		reg = (volatile REG_SERCOM_I2C *)SAMD51_SERCOM_I2C0_BASE;
		break;
	case SAMD51_SERCOM1:
		reg = (volatile REG_SERCOM_I2C *)SAMD51_SERCOM_I2C1_BASE;
		break;
	case SAMD51_SERCOM2:
		reg = (volatile REG_SERCOM_I2C *)SAMD51_SERCOM_I2C2_BASE;
		break;
	case SAMD51_SERCOM3:
		reg = (volatile REG_SERCOM_I2C *)SAMD51_SERCOM_I2C3_BASE;
		break;
	case SAMD51_SERCOM4:
		reg = (volatile REG_SERCOM_I2C *)SAMD51_SERCOM_I2C4_BASE;
		break;
	default:
		break;
	}
	
	return reg;
}

/*--------------------------------------------------------------------------*/
static void _i2c_master_intterupt_handler(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_I2C *reg = _getRegI2C(sercom);
	SAMD51_I2C_COMMUNICATION_CONTEXT *ctx = &sI2cCtx[sercom];

	if (reg->STATUS & (1 << SAMD51_SERCOM_I2C_RXNACK_POS)) {
		//J NAKを受け取ったらSTOP Conditionを出して終わる
		reg->CTRLB = (3 << SAMD51_SERCOM_I2C_CMD_POS);
		sI2cInterfaceIsBusy[(int)sercom] = 0;
		if (ctx->callback) {
			ctx->callback(AI_ERROR_I2C_NACK);
		}
		memset(ctx, 0x00, sizeof(SAMD51_I2C_COMMUNICATION_CONTEXT));

		return;
	}

	if (ctx->tx_size && (ctx->index < ctx->tx_size) && ctx->tx_buf) {
		reg->DATA = (uint32_t)ctx->tx_buf[ctx->index];
		ctx->index++;
		
		if(ctx->index >= ctx->tx_size) {
			ctx->tx_buf = NULL;
			ctx->index = 0;
		}
	}
	else if (ctx->rx_size && (ctx->index < ctx->rx_size) && ctx->rx_buf) {
		//J Read Operation の時の準備が必要
		ctx->tx_size = 0;

		reg->CTRLB = (1 << SAMD51_SERCOM_I2C_CMD_POS);
		reg->ADDR  |= 1;
	}
	else {
		//J 全ての処理が終わった後はStopコンディションを出してコールバックを叩く
		reg->CTRLB = (3 << SAMD51_SERCOM_I2C_CMD_POS);
		sI2cInterfaceIsBusy[(int)sercom] = 0;
		if (ctx->callback) {
			ctx->callback(AI_OK);
		}
		memset(ctx, 0x00, sizeof(SAMD51_I2C_COMMUNICATION_CONTEXT));
	}
	
	return;
}

/*--------------------------------------------------------------------------*/
static void _i2c_slave_intterupt_handler(SAMD51_SERCOM sercom)
{	
	volatile REG_SERCOM_I2C *reg = _getRegI2C(sercom);
	SAMD51_I2C_COMMUNICATION_CONTEXT *ctx = &sI2cCtx[sercom];

	if (ctx->rx_size && (ctx->index < ctx->rx_size) && ctx->rx_buf) {
		ctx->rx_buf[ctx->index] = (reg->DATA & 0xff);
		ctx->index++;
		
		if (ctx->index == ctx->rx_size) {
			//J 全ての処理が終わった後はStopコンディションを出してコールバックを叩く
			reg->CTRLB = (1 << SAMD51_SERCOM_I2C_ACKACT_POS) | (3 << SAMD51_SERCOM_I2C_CMD_POS);
			sI2cInterfaceIsBusy[(int)sercom] = 0;
			if (ctx->callback) {
				ctx->callback(AI_OK);
			}
			memset(ctx, 0x00, sizeof(SAMD51_I2C_COMMUNICATION_CONTEXT));
		}
		else {
			reg->CTRLB = (2 << SAMD51_SERCOM_I2C_CMD_POS);
		}
	}
	else {
		//J 全ての処理が終わった後はStopコンディションを出してコールバックを叩く
		reg->CTRLB = (3 << SAMD51_SERCOM_I2C_CMD_POS);
		sI2cInterfaceIsBusy[(int)sercom] = 0;
		if (ctx->callback) {
			ctx->callback(AI_OK);
		}
		memset(ctx, 0x00, sizeof(SAMD51_I2C_COMMUNICATION_CONTEXT));
	}

	return;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_i2c_initialize(SAMD51_SERCOM sercom, uint32_t i2c_clock)
{
	volatile REG_SERCOM_I2C *reg = _getRegI2C(sercom);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	if (reg->CTRLA & (1 << SAMD51_SERCOM_I2C_ENABLE_POS)) {
		reg->CTRLA |= (1 << SAMD51_SERCOM_I2C_SWRST_POS);
		
		while ((reg->SYNCBUSY & (1 << SAMD51_SERCOM_I2C_SWRST_POS)));	
	}

	//J Interrupt のEnable	
	samd51_sercom_set_interrupt_handler(sercom, 0, _i2c_master_intterupt_handler);
	samd51_sercom_set_interrupt_handler(sercom, 1, _i2c_slave_intterupt_handler);


	//Enable I2C p.1039
	reg->CTRLA  = (0x5 << SAMD51_SERCOM_I2C_MODE_POS);
	reg->CTRLA |= (1 << SAMD51_SERCOM_I2C_INACTOUT_POS);

	float refclk = 48000000; ///TODO Clockモジュールから持ってくる
	volatile uint32_t baud = ((refclk / (float)i2c_clock) - 10.0) / 2.0;
	reg->BAUD = (baud << SAMD51_SERCOM_I2C_BAUD_POS);

//	reg->STATUS = (reg->STATUS & ~(0x03 << SAMD51_SERCOM_I2C_BUSSTATE_POS)) | (0x01 << SAMD51_SERCOM_I2C_BUSSTATE_POS);
	reg->STATUS = 0x10;
	reg->CTRLA |= (1 << SAMD51_SERCOM_I2C_ENABLE_POS);

	//Enable Interrupt
	reg->INTENSET = (1 << SAMD51_SERCOM_I2C_INTFLAG_MB_POS) | (1 << SAMD51_SERCOM_I2C_INTFLAG_SB_POS);

	memset (&(sI2cCtx[(int)sercom]), 0x00, sizeof(sI2cCtx[0]));

	//J BUS StateがUnknown抜けるのを待つ
	while ((reg->STATUS & (3 << SAMD51_SERCOM_I2C_BUSSTATE_POS)) == 0);
	
	return AI_OK;	
}

/*--------------------------------------------------------------------------*/
void samd51_i2c_finalize(SAMD51_SERCOM sercom)
{
	volatile REG_SERCOM_I2C *reg = _getRegI2C(sercom);
	if (reg == NULL) {
		return;
	}

	samd51_sercom_reset_intterrupt(sercom);

	reg->CTRLA = (1 << SAMD51_SERCOM_I2C_SWRST_POS);
	while(reg->SYNCBUSY & (1 << SAMD51_SERCOM_I2C_SWRST_POS));
}


/*--------------------------------------------------------------------------*/
int samd51_i2c_txrx(SAMD51_SERCOM sercom, const uint8_t slave_addr, const uint8_t *txbuf, const size_t txlen, uint8_t *rxbuf, const size_t rxlen, SAMD51_I2C_DONE_CB callback)
{
	volatile REG_SERCOM_I2C *reg = _getRegI2C(sercom);
	if (reg == NULL) {
		return AI_ERROR_NODEV;
	}

	if (sI2cInterfaceIsBusy[(int)sercom]) {
		return AI_ERROR_I2C_BUSY;
	}
	
	sI2cInterfaceIsBusy[(int)sercom] = 1;

	//J トランザクションの内容を保存
	sI2cCtx[(int)sercom].index    = 0;
	sI2cCtx[(int)sercom].tx_buf   = txbuf;
	sI2cCtx[(int)sercom].tx_size  = txlen;
	sI2cCtx[(int)sercom].rx_buf   = rxbuf;
	sI2cCtx[(int)sercom].rx_size  = rxlen;
	sI2cCtx[(int)sercom].callback = callback;
	
	//J ADDRレジスタに書き込むことでStart Conditionを発行する
	if (txlen) {
		reg->ADDR = (uint8_t)(slave_addr << 1) | 0;			
	}
	else if (rxlen) {
		reg->ADDR = (uint8_t)(slave_addr << 1) | 1;
	}

	if (callback == NULL) {
		while (sI2cInterfaceIsBusy[(int)sercom]);
	}
	
	return AI_OK;
}
