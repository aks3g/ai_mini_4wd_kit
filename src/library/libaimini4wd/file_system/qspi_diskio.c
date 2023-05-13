/*
 * qspi_diskio.c
 *
 * Created: 2023/03/01 4:12:31
 *  Author: kiyot
 */ 
#include <string.h>

#include "ff.h"
#include "diskio.h"

#include "internal/MX25L51245G.h"

#include "samd51_qspi.h"
#include "samd51_error.h"
#include "ai_mini4wd.h"


#define SPI_FLASH_IDLE				(0)
#define SPI_FLASH_BUSY_ERASE		(1)
#define SPI_FLASH_BUSY_WRITE		(2)
#define SPI_FLASH_BUSY_READ			(3)



MX25L51245G mx25x = {0};

static int sFlashBusy = 0;

static int spi_flash_is_busy(void);
static void _flash_write_done(int status);

void _rtos_null_process(void){return;}
void __rtos_write_done_hook(void) __attribute__ ((weak, alias("_rtos_null_process")));
void __rtos_wait_write_done(void) __attribute__ ((weak, alias("_rtos_null_process")));

/*--------------------------------------------------------------------------*/
DSTATUS qspi_disk_initialize (void)
{
	MX25L51245G_init(&mx25x);
	return RES_OK;
}

/*--------------------------------------------------------------------------*/
DSTATUS qspi_disk_status (void)
{
	if (mx25x.initialized) {
		return RES_OK;
	}
	else {
		return RES_NOTRDY;
	}
}

/*--------------------------------------------------------------------------*/
DRESULT qspi_disk_read (BYTE* buff, DWORD sector, UINT count)
{
	uint32_t addr = sector * SPI_FLASH_SECTOR_SIZE;
	size_t size = count * SPI_FLASH_SECTOR_SIZE;
	if (mx25x.initialized == 0) {
		return RES_NOTRDY;
	}
	while(spi_flash_is_busy());

	mx25x.transaction_done = NULL;
	return MX25L51245G_read_data(&mx25x, addr, size, buff);
}

/*--------------------------------------------------------------------------*/
DRESULT qspi_disk_write (const BYTE* buff, DWORD sector, UINT count)
{
	uint32_t addr = sector * SPI_FLASH_SECTOR_SIZE;
	size_t size = count * SPI_FLASH_SECTOR_SIZE;
	if (mx25x.initialized == 0) {
		return RES_NOTRDY;
	}
	while(spi_flash_is_busy());

	sFlashBusy = SPI_FLASH_BUSY_ERASE;
	mx25x.dma_ctx.addr = addr;
	mx25x.dma_ctx.buf  = (BYTE *)buff;
	mx25x.dma_ctx.size = size;
	mx25x.transaction_done = _flash_write_done;

	//J 先ず消す
	MX25L51245G_write_enable(&mx25x);
	MX25L51245G_wait_status_ret_set(&mx25x, 0x02);

	//J Timer CBの中でStatusをPollingしてWriteをKickする
	int ret = MX25L51245G_erase_sector(&mx25x, addr);

	__rtos_wait_write_done();
	while(spi_flash_is_busy());

	return ret;
}

/*--------------------------------------------------------------------------*/
DRESULT qspi_disk_check_busy(void)
{
	if (spi_flash_is_busy()) {
		return RES_NOTRDY;
	}
	else {
		return RES_OK;
	}
}

/*--------------------------------------------------------------------------*/
DRESULT qspi_disk_ioctl (BYTE cmd, void* buff)
{
	size_t card_size, sector_size, block_size;
	int ret = AI_OK;
	if (mx25x.initialized == 0) {
		return RES_NOTRDY;
	}
	switch (cmd) {
	case GET_SECTOR_COUNT:
		card_size = 16 * 1024 *1024 / SPI_FLASH_SECTOR_SIZE;
		memcpy(buff, &card_size, sizeof(card_size));
		break;
	case GET_SECTOR_SIZE:
		sector_size = SPI_FLASH_SECTOR_SIZE;
		memcpy(buff, &sector_size, sizeof(sector_size));
		break;
	case GET_BLOCK_SIZE:
		block_size = 1;
		memcpy(buff, &block_size, sizeof(block_size));
		break;
	case CTRL_SYNC:
	case CTRL_TRIM:
		break;
	}

	return ret;
}

/*--------------------------------------------------------------------------*/
void qspi_disk_timerproc (void)
{
	return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static int spi_flash_is_busy(void) {
	return sFlashBusy || samd51_qspi_is_busy();
}

/*--------------------------------------------------------------------------*/
void flash_timer_proc(void)
{
	uint8_t status = 0xff;
	if (sFlashBusy == SPI_FLASH_BUSY_ERASE) {
		MX25L51245G_status_reg(&mx25x, &status);
		if ((status & 0x01) == 0) {
			//J 書き込むためのデータがある場合にはここから書き始める
			if (mx25x.dma_ctx.size != 0) {
				sFlashBusy = SPI_FLASH_BUSY_ERASE;
				MX25L51245G_write_enable(&mx25x);
				MX25L51245G_wait_status_ret_set(&mx25x, 0x02);
				MX25L51245G_write_data(&mx25x, mx25x.dma_ctx.addr, mx25x.dma_ctx.size, mx25x.dma_ctx.buf);
			}
			else {
				sFlashBusy = SPI_FLASH_BUSY_WRITE;
			}
		}
	}
	else {
		
	}
}

/*--------------------------------------------------------------------------*/
static void _flash_write_done(int status)
{
	sFlashBusy = SPI_FLASH_IDLE;
	__rtos_write_done_hook();
}