/*
 * MX25L51245G.h
 *
 * Created: 2023/03/15 16:24:11
 *  Author: kiyot
 */ 


#ifndef MX25L51245G_H_
#define MX25L51245G_H_

#include <samd51_qspi.h>

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

#define SPI_FLASH_SECTOR_SIZE		(4096)
#define SPI_FLASH_PAGE_SIZE			(256)

typedef struct M25L51245G_DMA_TRANSFER_t
{
	SAMD51_QSPI_INSTRUCTION inst;
	size_t size;
	uint8_t *buf;
	uint32_t addr;
} M25L51245G_DMA_TRANSFER;

typedef void (*MX25L5124G_TRANSACTION_DONE)(int status);

typedef struct {
	int initialized;
	uint32_t qpi;
	M25L51245G_DMA_TRANSFER dma_ctx;
	MX25L5124G_TRANSACTION_DONE transaction_done;
} MX25L51245G;


int MX25L51245G_identification(MX25L51245G *ctx, uint8_t *manuf_id, uint8_t *type, uint8_t *density);
int MX25L51245G_status_reg(MX25L51245G *ctx, uint8_t *status);
int MX25L51245G_write_status_configuration_reg(MX25L51245G *ctx, uint8_t status, uint8_t config);
int MX25L51245G_wait_status_ret_clear(MX25L51245G *ctx, uint8_t clear_mask);
int MX25L51245G_wait_status_ret_set(MX25L51245G *ctx, uint8_t set_mask);
int MX25L51245G_configuration_reg(MX25L51245G *ctx, uint8_t *conf);
int MX25L51245G_enter_4byte_address_mode(MX25L51245G *ctx);
int MX25L51245G_enable_QPI(MX25L51245G *ctx);
int MX25L51245G_disable_QPI(MX25L51245G *ctx);
int MX25L51245G_write_enable(MX25L51245G *ctx);
int MX25L51245G_init(MX25L51245G *ctx);
int MX25L51245G_read_data(MX25L51245G *ctx, uint32_t addr, size_t size, uint8_t *buf);
int MX25L51245G_write_data(MX25L51245G *ctx, uint32_t addr, size_t size, uint8_t *buf);
int MX25L51245G_erase_sector(MX25L51245G *ctx, uint32_t addr);

#endif /* MX25L51245G_H_ */