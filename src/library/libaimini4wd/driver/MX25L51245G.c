/*
 * MX25L51245G.c
 *
 * Created: 2023/02/22 10:20:27
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <string.h>

#include "internal/MX25L51245G.h"

#include "samd51_qspi.h"
#include "samd51_error.h"
#include "ai_mini4wd.h"

static MX25L51245G *p_mx25x = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _write_done(int error);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
int MX25L51245G_wait_status_ret_clear(MX25L51245G *ctx, uint8_t clear_mask)
{

	uint8_t status = 0;
	do {
		MX25L51245G_status_reg(ctx, &status);
	}while ((status & clear_mask) == clear_mask);

	return 0;

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

/*--------------------------------------------------------------------------*/
int MX25L51245G_wait_status_ret_set(MX25L51245G *ctx, uint8_t set_mask)
{
	uint8_t status = 0;
	do {
		MX25L51245G_status_reg(ctx, &status);
	}while ((status & set_mask) == 0);

	return 0;

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

/*--------------------------------------------------------------------------*/
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


/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
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


/*--------------------------------------------------------------------------*/
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

	ctx->initialized = 1;

	return 0;
}

/*--------------------------------------------------------------------------*/
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
	
	return samd51_qspi_exec_instruction(&inst, NULL, 0, buf, size, ctx->transaction_done);
}

/*--------------------------------------------------------------------------*/
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

	p_mx25x = ctx;

	// SPI_FLASH_PAGE_SIZEˆÈã‚Ì‘‚«ž‚Ý‚Í•ªŠ„‚·‚é
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

/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _write_done(int error)
{
	if (error) {
		return;
	}

	//J ‘‚¯‚é‚Ü‚Å‘Ò‚Â
	MX25L51245G_wait_status_ret_clear(p_mx25x, 0x01);

	if (p_mx25x->dma_ctx.size) {
		size_t next_size = p_mx25x->dma_ctx.size >= SPI_FLASH_PAGE_SIZE ? SPI_FLASH_PAGE_SIZE : p_mx25x->dma_ctx.size;

		int ret = MX25L51245G_write_enable(p_mx25x);
		if (ret != 0) {
			while(1);
		}
		MX25L51245G_wait_status_ret_set(p_mx25x, 0x02);

		p_mx25x->dma_ctx.inst.addr = p_mx25x->dma_ctx.addr;
		samd51_qspi_exec_instruction(&(p_mx25x->dma_ctx.inst), p_mx25x->dma_ctx.buf, next_size, NULL, 0, _write_done);
		p_mx25x->dma_ctx.addr += next_size;
		p_mx25x->dma_ctx.buf  += next_size;
		p_mx25x->dma_ctx.size -= next_size;
	}
	else {
		if (p_mx25x->transaction_done) {
			p_mx25x->transaction_done(0);
		}
		
		p_mx25x = NULL;
	}
	return ;
}
