/*
 * library_test.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <ai_mini4wd.h>
#include <ai_mini4wd_fs.h>
#include <ai_mini4wd_error.h>
#include <ai_mini4wd_sensor.h>
#include <ai_mini4wd_timer.h>
#include <ai_mini4wd_hid.h>

#include <samd51_timer.h>
#include <samd51_clock.h>
#include <samd51_qspi.h>
#include <samd51_dmac.h>

#if 0

uint32_t stamp_idx=0;
uint32_t timestamp[32] = {0};

void split_time(void)
{
	timestamp[stamp_idx++] = samd51_tcc_get_counter(SAMD51_TCC2);
}



/* 3Byte Adress Command Set */
#define X25L51245G_READ					(0x03)
#define X25L51245G_FAST_READ			(0x0B)
#define X25L51245G_2READ				(0xBB)
#define X25L51245G_DREAD				(0x3B)
#define X25L51245G_4READ				(0xEB)
#define X25L51245G_QREAD				(0x6B)
#define X25L51245G_FASTDTRD				(0x0D)
#define X25L51245G_2DTRD				(0xBD)
#define X25L51245G_4DTRD				(0xED)
#define X25L51245G_PP					(0x02)
#define X25L51245G_4PP					(0x38)
#define X25L51245G_SE					(0x20)
#define X25L51245G_BE32K				(0x52)
#define X25L51245G_BE					(0xD8)
#define X25L51245G_CE					(0x60)

/* 4Byte Adress Command Set */
#define X25L51245G_READ4B				(0x13)
#define X25L51245G_FAST_READ4B			(0x0C)
#define X25L51245G_2READ4B				(0xBC)
#define X25L51245G_DREAD4B				(0x3C)
#define X25L51245G_4READ4B				(0xEC)
#define X25L51245G_QREAD4B				(0x6C)
#define X25L51245G_FASTDTRD4B			(0x0E)
#define X25L51245G_2DTRD4B				(0xBE)
#define X25L51245G_4DTRD4B				(0xEE)
#define X25L51245G_PP4B					(0x12)
#define X25L51245G_4PP4B				(0x3E)
#define X25L51245G_BE4B					(0xDC)
#define X25L51245G_BE32K4B				(0x5C)
#define X25L51245G_SE4B					(0x21)

/* Register/Setting Commands */
#define X25L51245G_WREN					(0x06)
#define X25L51245G_WRDI					(0x04)
#define X25L51245G_FMEN					(0x41)
#define X25L51245G_RDSR					(0x05)
#define X25L51245G_RDCR					(0x15)
#define X25L51245G_WRSR					(0x01)
#define X25L51245G_RDEAR				(0xC8)
#define X25L51245G_WREAR				(0xC5)
#define X25L51245G_WPSEL				(0x68)
#define X25L51245G_EQIO					(0x35)
#define X25L51245G_RSTQIO				(0xF5)
#define X25L51245G_EN4B					(0xB7)
#define X25L51245G_EX4B					(0xE9)
#define X25L51245G_PGM_ERS_SUS			(0xB0)
#define X25L51245G_PGM_ERS_RES			(0x30)
#define X25L51245G_DP					(0xB9)
#define X25L51245G_RDP					(0xAB)
#define X25L51245G_SBL					(0xC0)
#define X25L51245G_RDFBR				(0x16)
#define X25L51245G_WRFBR				(0x17)
#define X25L51245G_ESFBR				(0x18)

/* ID/Security */
#define X25L51245G_RDID					(0x9F)
#define X25L51245G_RES					(0xAB)
#define X25L51245G_REMS					(0x90)
#define X25L51245G_QPIID				(0xAF)
#define X25L51245G_RDSFDP				(0x5A)
#define X25L51245G_ENSO					(0xB1)
#define X25L51245G_EXSO					(0xC1)
#define X25L51245G_RDSCUR				(0x2B)
#define X25L51245G_WRSCUR				(0x2F)
#define X25L51245G_GBLK					(0x7E)
#define X25L51245G_GBULK				(0x98)
#define X25L51245G_WRLR					(0x2C)
#define X25L51245G_RDLR					(0x2D)
#define X25L51245G_WRPASS				(0x28)
#define X25L51245G_RDPASS				(0x27)
#define X25L51245G_PASSULK				(0x29)
#define X25L51245G_WRSPB				(0xE3)
#define X25L51245G_ESSPB				(0xE4)
#define X25L51245G_RDSPB				(0xE2)
#define X25L51245G_SPBLK				(0xA6)
#define X25L51245G_RDSPBLK				(0xA7)
#define X25L51245G_WRDPB				(0xE1)
#define X25L51245G_RDDPB				(0xE0)

#define X25L51245G_NOP					(0x00)
#define X25L51245G_RSTEN				(0x66)
#define X25L51245G_RST					(0x99)

typedef struct M25L51245G_DMA_TRANSFER_t
{
	SAMD51_QSPI_INSTRUCTION inst;
	size_t size;
	uint8_t *buf;
	uint32_t addr;
} M25L51245G_DMA_TRANSFER;

typedef void (*MX25L5124G_TRANSACTION_DONE)(int status);

typedef struct {
	uint32_t qpi;
	M25L51245G_DMA_TRANSFER dma_ctx;
	MX25L5124G_TRANSACTION_DONE transaction_done;
} MX25L51245G;

MX25L51245G mx25x;

#define SPI_FLASH_SECTOR_SIZE (4096)
#define SPI_FLASH_PAGE_SIZE (256)

int MX25L51245G_identification(MX25L51245G *ctx, uint8_t *manuf_id, uint8_t *type, uint8_t *density)
{
	int ret = 0;
	uint8_t ids[3];
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_RDID;
		inst.frame.st.dataen  = 1;
		inst.frame.st.instren = 1;
	}
	
	ret = samd51_qspi_exec_instruction(&inst, NULL, 0, ids, sizeof(ids),NULL);
	if (ret == 0) {
		*manuf_id = ids[0];
		*type     = ids[1];
		*density  = ids[2];
	}

	return ret;
}

int MX25L51245G_status_reg(MX25L51245G *ctx, uint8_t *status)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_RDSR;
		inst.frame.st.dataen  = 1;
		inst.frame.st.instren = 1;
		inst.frame.st.width = (ctx->qpi) ? 6 : 0;
	}
	return samd51_qspi_exec_instruction(&inst, NULL, 0, status, sizeof(uint8_t),NULL);
}

int MX25L51245G_write_status_configuration_reg(MX25L51245G *ctx, uint8_t status, uint8_t config)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_WRSR;
		inst.frame.st.dataen  = 1;
		inst.frame.st.instren = 1;
		inst.frame.st.width = (ctx->qpi) ? 6 : 0;
		inst.frame.st.tfrtype = 0x02;
	}
	uint8_t arr[2];
	arr[0] = status;
	arr[1] = config;
	
	return samd51_qspi_exec_instruction(&inst, arr, sizeof(arr), NULL, 0, NULL);
}

int MX25L51245G_wait_status_ret_clear(MX25L51245G *ctx, uint8_t clear_mask)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_RDSR;
		inst.frame.st.dataen  = 1;
		inst.frame.st.instren = 1;
		inst.frame.st.width = (ctx->qpi) ? 6 : 0;
	}

	return samd51_qspi_exec_instruction_until_bit_clear(&inst,clear_mask,100);
}

int MX25L51245G_wait_status_ret_set(MX25L51245G *ctx, uint8_t set_mask)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_RDSR;
		inst.frame.st.dataen  = 1;
		inst.frame.st.instren = 1;
		inst.frame.st.width = (ctx->qpi) ? 6 : 0;
	}

	return samd51_qspi_exec_instruction_until_bit_set(&inst,set_mask,100);
}

int MX25L51245G_configuration_reg(MX25L51245G *ctx, uint8_t *conf)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_RDCR;
		inst.frame.st.dataen  = 1;
		inst.frame.st.instren = 1;
		inst.frame.st.width = (ctx->qpi) ? 6 : 0;
	}
	
	return samd51_qspi_exec_instruction(&inst, NULL, 0, conf, sizeof(uint8_t),NULL);
}


int MX25L51245G_enter_4byte_address_mode(MX25L51245G *ctx)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_EN4B;
		inst.frame.st.dataen  = 0;
		inst.frame.st.instren = 1;
	}
	
	return samd51_qspi_exec_instruction(&inst, NULL, 0, NULL, 0,NULL);
}

int MX25L51245G_enable_QPI(MX25L51245G *ctx)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_EQIO;
		inst.frame.st.dataen  = 0;
		inst.frame.st.instren = 1;
	}
	
	ctx->qpi = 1;
	
	return samd51_qspi_exec_instruction(&inst, NULL, 0, NULL, 0,NULL);
}

int MX25L51245G_disable_QPI(MX25L51245G *ctx)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_RSTQIO;
		inst.frame.st.dataen  = 0;
		inst.frame.st.instren = 1;
		inst.frame.st.width = (ctx->qpi) ? 6 : 0;
	}
	
	ctx->qpi = 0;
	
	return samd51_qspi_exec_instruction(&inst, NULL, 0, NULL, 0,NULL);
}

int MX25L51245G_write_enable(MX25L51245G *ctx)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0;
		inst.code.st.instr = X25L51245G_WREN;
		inst.frame.st.dataen  = 0;
		inst.frame.st.instren = 1;
		inst.frame.st.width = (ctx->qpi) ? 6 : 0;
	}

	
	return samd51_qspi_exec_instruction(&inst, NULL, 0, NULL, 0,NULL);
}


int MX25L51245G_init(MX25L51245G *ctx)
{
	memset (ctx, 0, sizeof(MX25L51245G));
	
	volatile uint8_t manuf_id=0, type, density;
	MX25L51245G_identification(ctx, &manuf_id, &type, &density);

//	MX25L51245G_enter_4byte_address_mode(ctx);

//	MX25L51245G_enable_QPI(ctx);
//	MX25L51245G_disable_QPI(ctx);

	volatile uint32_t wait = 0x7fffff;
	while (wait--);

	return 0;
}

int MX25L51245G_read_data(MX25L51245G *ctx, uint32_t addr, size_t size, uint8_t *buf)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = 0x01000000 | addr;
		inst.code.st.instr = (ctx->qpi) ? X25L51245G_4READ4B : X25L51245G_READ4B;
		inst.frame.st.dataen  = 1;
		inst.frame.st.addren   = 1;
		inst.frame.st.addrlen  = 1;
		inst.frame.st.instren = 1;
		inst.frame.st.width = (ctx->qpi) ? 6 : 0;
		inst.frame.st.dummy = (ctx->qpi) ? 6 : 0;
		inst.frame.st.tfrtype = 0x01;
	}
		
	return samd51_qspi_exec_instruction(&inst, NULL, 0, buf, size, mx25x.transaction_done);
}

void _write_done(int error)
{
	if (error) {
		return;
	}

	//J 書けるまで待つ	
	MX25L51245G_wait_status_ret_clear(&mx25x, 0x01);

	if (mx25x.dma_ctx.size) {
		size_t next_size = mx25x.dma_ctx.size >= SPI_FLASH_PAGE_SIZE ? SPI_FLASH_PAGE_SIZE : mx25x.dma_ctx.size;

		MX25L51245G_write_enable(&mx25x);
		MX25L51245G_wait_status_ret_set(&mx25x, 0x02);

		mx25x.dma_ctx.inst.addr = mx25x.dma_ctx.addr;
		samd51_qspi_exec_instruction(&mx25x.dma_ctx.inst, mx25x.dma_ctx.buf, next_size, NULL, 0, _write_done);
		mx25x.dma_ctx.addr += next_size;
		mx25x.dma_ctx.buf  += next_size;
		mx25x.dma_ctx.size -= next_size;
	}
	else {
		if (mx25x.transaction_done) {
			mx25x.transaction_done(0);
		}
	}
	return ;
}

int MX25L51245G_write_data(MX25L51245G *ctx, uint32_t addr, size_t size, uint8_t *buf)
{
	{
		memset(&ctx->dma_ctx.inst, 0, sizeof(ctx->dma_ctx.inst));
		ctx->dma_ctx.inst.addr = addr;
		ctx->dma_ctx.inst.code.st.instr = (ctx->qpi) ? X25L51245G_4PP4B : X25L51245G_PP4B;
		ctx->dma_ctx.inst.frame.st.dataen  = 1;
		ctx->dma_ctx.inst.frame.st.addren   = 1;
		ctx->dma_ctx.inst.frame.st.addrlen  = 1;
		ctx->dma_ctx.inst.frame.st.instren = 1;
		ctx->dma_ctx.inst.frame.st.width = (ctx->qpi) ? 4 : 0;
		ctx->dma_ctx.inst.frame.st.tfrtype = 0x03;
		ctx->dma_ctx.inst.frame.st.dummy = 0;
	}

	// SPI_FLASH_PAGE_SIZE以上の書き込みは分割する
	if (size > SPI_FLASH_PAGE_SIZE) {
		int ret =  samd51_qspi_exec_instruction(&ctx->dma_ctx.inst, buf, SPI_FLASH_PAGE_SIZE, NULL, 0, _write_done);
		ctx->dma_ctx.addr = addr + SPI_FLASH_PAGE_SIZE;
		ctx->dma_ctx.buf  = buf + SPI_FLASH_PAGE_SIZE;
		ctx->dma_ctx.size = size - SPI_FLASH_PAGE_SIZE;
		return ret;

	}
	else {
		return samd51_qspi_exec_instruction(&ctx->dma_ctx.inst, buf, size, NULL, 0, _write_done);
	}
}

int MX25L51245G_erase_sector(MX25L51245G *ctx, uint32_t addr)
{
	SAMD51_QSPI_INSTRUCTION inst;
	{
		memset(&inst, 0, sizeof(inst));
		inst.addr = addr;
		inst.code.st.instr = X25L51245G_SE4B;
		inst.frame.st.dataen  = 0;
		inst.frame.st.addren   = 1;
		inst.frame.st.addrlen  = 1;
		inst.frame.st.instren = 1;
		inst.frame.st.width = (ctx->qpi) ? 4 : 0;
		inst.frame.st.tfrtype = 0x00;
	}
		
	return samd51_qspi_exec_instruction(&inst, NULL, 0, NULL, 0,NULL);
}

uint8_t sector_buf[4096];
uint8_t page_buf[256];

typedef void (*SPI_FLASH_DONE_CB)(int status);

static int sFlashBusy = 0;

int spi_flash_is_busy(void) {
	return sFlashBusy || samd51_qspi_is_busy();
}

int spi_flash_initialize(void)
{
	memset(&mx25x, 0, sizeof(mx25x));
	MX25L51245G_init(&mx25x);
}

int spi_flash_get_sector_size(void)
{
	return SPI_FLASH_SECTOR_SIZE;
}

#define SERIAL_FLASH_IDLE			(0)
#define SERIAL_FLASH_BUSY_ERASE		(1)
#define SERIAL_FLASH_BUSY_WRITE		(2)
#define SERIAL_FLASH_BUSY_READ		(3)

int spi_flash_read_sector(uint32_t addr, uint8_t *buf, size_t size, SPI_FLASH_DONE_CB func)
{
	if (sFlashBusy) return AI_ERROR_BUSY;

	if ((addr & (SPI_FLASH_SECTOR_SIZE-1)) != 0) {
		return AI_ERROR_INVALID_ALIGN;
	}
	else if (((size & (SPI_FLASH_SECTOR_SIZE-1)) != 0) || (size == 0)){
		return AI_ERROR_NOBUF;
	}
	else if (buf == NULL) {
		return AI_ERROR_NULL;
	}
	sFlashBusy = SERIAL_FLASH_BUSY_READ;
	mx25x.transaction_done = func;

	return MX25L51245G_read_data(&mx25x, addr, size, buf);
}

void _flash_timer_cb(void)
{
	uint8_t status = 0;
	if (sFlashBusy == SERIAL_FLASH_BUSY_ERASE) {
		MX25L51245G_status_reg(&mx25x, &status);
		if ((status & 0x01) == 0) {
			//J 書き込むためのデータがある場合にはここから書き始める
			if (mx25x.dma_ctx.size != 0) {			
				sFlashBusy = SERIAL_FLASH_BUSY_WRITE;
				MX25L51245G_write_enable(&mx25x);
				MX25L51245G_wait_status_ret_set(&mx25x, 0x02);
				MX25L51245G_write_data(&mx25x, mx25x.dma_ctx.addr, mx25x.dma_ctx.size, mx25x.dma_ctx.buf);
			}
			else {
				sFlashBusy = SERIAL_FLASH_IDLE;
			}
		}
	}
	else {
		
	}
}

static void _flash_write_done(int status)
{
	split_time();
	sFlashBusy = SERIAL_FLASH_IDLE;
}

static void _flash_read_done(int status)
{
	split_time();
	sFlashBusy = SERIAL_FLASH_IDLE;
}

int spi_flash_write_sector(uint32_t addr, uint8_t *buf, size_t size, SPI_FLASH_DONE_CB func)
{
	if (sFlashBusy) return AI_ERROR_BUSY;
	
	if ((addr & (SPI_FLASH_SECTOR_SIZE-1)) != 0) {
		return AI_ERROR_INVALID_ALIGN;
	}
	else if (((size & (SPI_FLASH_SECTOR_SIZE-1)) != 0) || (size == 0)){
		return AI_ERROR_NOBUF;
	}
	else if (buf == NULL) {
		return AI_ERROR_NULL;
	}

	sFlashBusy = SERIAL_FLASH_BUSY_ERASE;
	mx25x.dma_ctx.addr = addr;
	mx25x.dma_ctx.buf  = buf;
	mx25x.dma_ctx.size = size;
	mx25x.transaction_done = func;

	//J 先ず消す
	MX25L51245G_write_enable(&mx25x);
	MX25L51245G_wait_status_ret_set(&mx25x, 0x02);

	//J Timer CBの中でStatusをPollingしてWriteをKickする
	return MX25L51245G_erase_sector(&mx25x, addr);
}

__attribute__((__aligned__(32))) uint8_t wbuf[4096];
__attribute__((__aligned__(32))) uint8_t rbuf[4096];
static REG_DMA_DESC sDmacDesc;

#endif

int main(void)
{
	char line[1024];

	int ret =aiMini4wdInitialize(AI_MINI_4WD_INIT_FLAG_USE_TEST_TYPE_HW);
	if (ret != AI_OK) {
		while(1);
	}

	AiMini4wdFile *fp = aiMini4wdFsOpen("test.txt", "w");
	if (fp == NULL) {
		aiMini4wdDiskFormat();
		while(1);
	}

	aiMini4wdFsWrite(fp, "hogehoge\n", strlen("hogehoge\n"));
	aiMini4wdFsClose(fp);

	fp = aiMini4wdFsOpen("test.txt", "r");
	aiMini4wdFsRead(fp, line, sizeof(line));
	aiMini4wdFsClose(fp);
	
	while(1);

	return 0;
}
