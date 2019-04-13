/*
 * sdhc_diskio.c
 *
 * Created: 2019/03/12
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */
#include <string.h>

#include "ff.h"
#include "diskio.h"

#include "samd51_sdhc.h"
#include "samd51_error.h"
#include "mmc.h"

#include "ai_mini4wd.h"


/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
static Samd51_dhcCardContext sCardCtx;



/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
static int _print_cid_regs(SdcCidReg *reg)
{
	if (reg == NULL) {
		return -1;
	}
	
	aiMini4wdDebugPrintf("*** Dump CID Regs. size  %d ***\r\n", sizeof(SdcCsdReg));
	aiMini4wdDebugPrintf("  Manufacturer ID = 0x%02x\r\n", reg->bm.manufacturer_id);
	aiMini4wdDebugPrintf("  OEM Application ID = 0x%02x\r\n", reg->bm.oem_application_id);
	aiMini4wdDebugPrintf("  Product Name = %c%c%c%c%c\r\n"
		, reg->bm.product_name[0]
		, reg->bm.product_name[1]
		, reg->bm.product_name[2]
		, reg->bm.product_name[3]
		, reg->bm.product_name[4]);

	aiMini4wdDebugPrintf("  Product Version= 0x%02x\r\n", reg->bm.product_revision);
	aiMini4wdDebugPrintf("  Product Serial Number = 0x%08x\r\n", reg->bm.product_serial_number);
	aiMini4wdDebugPrintf("  Manufacturing date =  %d\r\n", reg->bm.manufacturing_date);

	return 0;
}


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS sdhc_disk_initialize (void)
{
	//aiMini4wdDebugPrintf("[TRACE] Enter %s: \r\n", __FUNCTION__);
	int ret = samd51_sdhc_card_initialization_and_identification(SAMD51_SDHC0, &sCardCtx);

	return (ret != AI_OK) ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS sdhc_disk_status (void)
{
	//aiMini4wdDebugPrintf("[TRACE] Enter %s: \r\n", __FUNCTION__);

	return sCardCtx.status;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT sdhc_disk_read (
BYTE *buff,			/* Pointer to the data buffer to store read data */
DWORD sector,		/* Start sector number (LBA) */
UINT count			/* Sector count (1..128) */
)
{
	//aiMini4wdDebugPrintf("[TRACE] Enter %s: buf = %p, sector = 0x%08x, count = %d\r\n", __FUNCTION__, buff, sector, count);
	int ret = samd51_transfer(SAMD51_SDHC0, &sCardCtx, 1, sector, buff, count * 512, 1);
	
	return (ret != AI_OK) ? RES_ERROR : RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT sdhc_disk_write (
const BYTE *buff,	/* Pointer to the data to be written */
DWORD sector,		/* Start sector number (LBA) */
UINT count			/* Sector count (1..128) */
)
{
	//aiMini4wdDebugPrintf("[TRACE] Enter %s: \r\n", __FUNCTION__);
	int ret = samd51_transfer(SAMD51_SDHC0, &sCardCtx, 0, sector, (void *)buff, count * 512, 0);

	return (ret != AI_OK) ? RES_ERROR : RES_OK;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT sdhc_disk_ioctl (
BYTE cmd,		/* Control code */
void *buff		/* Buffer to send/receive control data */
)
{
	//aiMini4wdDebugPrintf("[TRACE] Enter %s: \r\n", __FUNCTION__);

	int ret = AI_OK;
	if (sCardCtx.type == SAMD51_SDHC_UNKNOWN || sCardCtx.status != 0) {
		return RES_NOTRDY;
	}


	switch (cmd) {
	case CTRL_SYNC:
		//J “Á‚É‚â‚é‚×‚«‚±‚Æ‚ª‚È‚¢‚Í‚¸
		break;
	case GET_SECTOR_COUNT:
		{
			size_t card_size = 0;
			if (sCardCtx.type == SAMD51_SDHC_SDv2) {
				card_size = sCardCtx.csd.v2.device_size + 1;
			}
			else if (sCardCtx.type == SAMD51_SDHC_SDv1 || sCardCtx.type == SAMD51_SDHC_MMC) {
				size_t block_len = 1 << sCardCtx.csd.v1.max_read_data_block_length;
				size_t mult = 1 << (2 + sCardCtx.csd.v1.device_size_multiplier);
				size_t blocknr = (sCardCtx.csd.v1.device_size) * mult;

				card_size = blocknr * block_len;
			}
			memcpy(buff, &card_size, sizeof(card_size));
		}
		break;
	case GET_SECTOR_SIZE:
		break;
	case GET_BLOCK_SIZE:
		{
			size_t block_size = 0;
			if (sCardCtx.type == SAMD51_SDHC_SDv2) {
				block_size = sCardCtx.csd.v2.erase_sector_size;
			}
			else if (sCardCtx.type == SAMD51_SDHC_SDv1) {
				block_size = sCardCtx.csd.v1.erase_sector_size;
			}
			else if (sCardCtx.type == SAMD51_SDHC_MMC) {
				//			block_size = sCardCtx.csd.mmc.erase_sector_size;
			}
			memcpy(buff, &block_size, sizeof(block_size));
		}
		break;

#if 0
		if (CardType & CT_SD2) {	/* SDv2? */
			if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
				xchg_spi(0xFF);
				if (rcvr_datablock(csd, 16)) {				/* Read partial block */
					for (n = 64 - 16; n; n--) xchg_spi(0xFF);	/* Purge trailing data */
					*(DWORD*)buff = 16UL << (csd[10] >> 4);
					res = RES_OK;
				}
			}
		} else {					/* SDv1 or MMCv3 */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
				if (CardType & CT_SD1) {	/* SDv1 */
					*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {					/* MMCv3 */
					*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
		}
		deselect();
#endif
		break;

	case CTRL_TRIM:
	case MMC_GET_TYPE:
	case MMC_GET_CSD:
	case MMC_GET_CID:
	case MMC_GET_OCR:
	case MMC_GET_SDSTAT:
	case ISDIO_READ:
	case ISDIO_WRITE:
	case ISDIO_MRITE:
		break;
	}

#if 0
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	DWORD *dp, st, ed, csize;
	#if _USE_ISDIO
	SDIO_CTRL *sdi;
	BYTE rc, *bp;
	UINT dc;
	#endif

	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;
	switch (cmd) {
		case CTRL_SYNC :		/* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
		if (select()) res = RES_OK;
		deselect();
		break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
		if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
			if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
				csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
				*(DWORD*)buff = csize << 10;
			} else {					/* SDC ver 1.XX or MMC*/
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
				*(DWORD*)buff = csize << (n - 9);
			}
			res = RES_OK;
		}
		deselect();
		break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		if (CardType & CT_SD2) {	/* SDv2? */
			if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
				xchg_spi(0xFF);
				if (rcvr_datablock(csd, 16)) {				/* Read partial block */
					for (n = 64 - 16; n; n--) xchg_spi(0xFF);	/* Purge trailing data */
					*(DWORD*)buff = 16UL << (csd[10] >> 4);
					res = RES_OK;
				}
			}
			} else {					/* SDv1 or MMCv3 */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
				if (CardType & CT_SD1) {	/* SDv1 */
					*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else {					/* MMCv3 */
					*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
		}
		deselect();
		break;

		case CTRL_TRIM:		/* Erase a block of sectors (used when _USE_TRIM in ffconf.h is 1) */
		if (!(CardType & CT_SDC)) break;				/* Check if the card is SDC */
		if (sdhc_disk_ioctl(MMC_GET_CSD, csd)) break;	/* Get CSD */
		if (!(csd[0] >> 6) && !(csd[10] & 0x40)) break;	/* Check if sector erase can be applied to the card */
		dp = buff; st = dp[0]; ed = dp[1];				/* Load sector block */
		if (!(CardType & CT_BLOCK)) {
			st *= 512; ed *= 512;
		}
		if (send_cmd(CMD32, st) == 0 && send_cmd(CMD33, ed) == 0 && send_cmd(CMD38, 0) == 0 && wait_ready(30000)) {	/* Erase sector block */
			res = RES_OK;	/* FatFs does not check result of this command */
		}
		break;

		/* Following commands are never used by FatFs module */

		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
		*ptr = CardType;
		res = RES_OK;
		break;

		case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
		if (send_cmd(CMD9, 0) == 0 && rcvr_datablock(ptr, 16)) {	/* READ_CSD */
			res = RES_OK;
		}
		deselect();
		break;

		case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
		if (send_cmd(CMD10, 0) == 0 && rcvr_datablock(ptr, 16)) {	/* READ_CID */
			res = RES_OK;
		}
		deselect();
		break;

		case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
		if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
			for (n = 4; n; n--) *ptr++ = xchg_spi(0xFF);
			res = RES_OK;
		}
		deselect();
		break;

		case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
		if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
			xchg_spi(0xFF);
			if (rcvr_datablock(ptr, 64)) res = RES_OK;
		}
		deselect();
		break;

		case CTRL_POWER_OFF :	/* Power off */
		power_off();
		Stat |= STA_NOINIT;
		res = RES_OK;
		break;
		#if _USE_ISDIO
		case ISDIO_READ:
		sdi = buff;
		if (send_cmd(CMD48, 0x80000000 | (DWORD)sdi->func << 28 | (DWORD)sdi->addr << 9 | ((sdi->ndata - 1) & 0x1FF)) == 0) {
			for (Timer1 = 100; (rc = xchg_spi(0xFF)) == 0xFF && Timer1; ) ;
			if (rc == 0xFE) {
				for (bp = sdi->data, dc = sdi->ndata; dc; dc--) *bp++ = xchg_spi(0xFF);
				for (dc = 514 - sdi->ndata; dc; dc--) xchg_spi(0xFF);
				res = RES_OK;
			}
		}
		deselect();
		break;

		case ISDIO_WRITE:
		sdi = buff;
		if (send_cmd(CMD49, 0x80000000 | (DWORD)sdi->func << 28 | (DWORD)sdi->addr << 9 | ((sdi->ndata - 1) & 0x1FF)) == 0) {
			xchg_spi(0xFF); xchg_spi(0xFE);
			for (bp = sdi->data, dc = sdi->ndata; dc; dc--) xchg_spi(*bp++);
			for (dc = 514 - sdi->ndata; dc; dc--) xchg_spi(0xFF);
			if ((xchg_spi(0xFF) & 0x1F) == 0x05) res = RES_OK;
		}
		deselect();
		break;

		case ISDIO_MRITE:
		sdi = buff;
		if (send_cmd(CMD49, 0x84000000 | (DWORD)sdi->func << 28 | (DWORD)sdi->addr << 9 | sdi->ndata >> 8) == 0) {
			xchg_spi(0xFF); xchg_spi(0xFE);
			xchg_spi(sdi->ndata);
			for (dc = 513; dc; dc--) xchg_spi(0xFF);
			if ((xchg_spi(0xFF) & 0x1F) == 0x05) res = RES_OK;
		}
		deselect();
		break;
		#endif
		default:
		res = RES_PARERR;
	}

	return res;
#endif
	return ret;
}
#endif


/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure                                      */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */
void sdhc_disk_timerproc (void)
{
	return;
}

