/*
 * samd51_clock.c
 *
 * Created: 2019/05/22
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <sam.h>

#include <samd51_error.h>
#include <samd51_nvmctrl.h>


#pragma pack(1)
typedef struct STRUCT_NVMCTRL_REG_t
{
	volatile uint16_t ctrla;
	volatile uint8_t  reserved[2];
	volatile uint16_t ctrlb;
	volatile uint8_t  reserved2[2];
	volatile uint32_t param;
	volatile uint16_t intenclr;
	volatile uint16_t intenset;
	volatile uint16_t intflag;
	volatile uint16_t status;
	volatile uint32_t addr;
	volatile uint32_t runlock;
	volatile uint32_t pbldata0;
	volatile uint32_t pbldata1;
	volatile uint32_t eccerr;
	volatile uint8_t  dbgctrl;
	volatile uint8_t  reserved3;
	volatile uint8_t  seecfg;
	volatile uint8_t  reserved4;
	volatile uint32_t seestat;

} STRUCT_NVMCTRL_REG;

#define NVMCTRL_REG					(*(volatile STRUCT_NVMCTRL_REG *)0x41004000UL)

#define SAMD51_NVM_EXEC_CMD			(0xA5)
#define SAMD51_NVMCTRL_CMD_EP		(0x00) // Erase Page
#define SAMD51_NVMCTRL_CMD_EB		(0x01) // Erase Block
#define SAMD51_NVMCTRL_CMD_WP		(0x03) // Write Page
#define SAMD51_NVMCTRL_CMD_WQW		(0x04) // Write Quad Word
#define SAMD51_NVMCTRL_CMD_SWRST	(0x10) // Software Reset
#define SAMD51_NVMCTRL_CMD_LR		(0x11) // Lock Region
#define SAMD51_NVMCTRL_CMD_UR		(0x12) // Unlock Region
#define SAMD51_NVMCTRL_CMD_SPRM		(0x13) // Sets the Power reduction mode
#define SAMD51_NVMCTRL_CMD_CPRM		(0x14) // Clears the power reduction mode
#define SAMD51_NCMCTRL_CMD_PBC		(0x15) // Page Buffer clear
#define SAMD51_NVMCTRL_CMD_SSB		(0x16) // Set Security Bit
#define SAMD51_NVMCTRL_CMD_BKSWRST	(0x17) // Bank swap and system reset.
#define SAMD51_NVMCTRL_CMD_CELCK	(0x18) // Chip Erase Lock
#define SAMD51_NVMCTRL_CMD_CEULCK	(0x19) // Chip Erase Unlock
#define SAMD51_NVMCTRL_CMD_SBPDIS	(0x1A) // Sets STATUS.BPDIS
#define SAMD51_NVMCTRL_CMD_CBPDIS	(0x1B) // Clears STATUS.BPDIS
#define SAMD51_NVMCTRL_CMD_ASEES0	(0x30) // Configure SmartEEPROM to use Sector0
#define SAMD51_NVMCTRL_CMD_ASEES1	(0x31) // Configure SmartEEPROM to use Sector1
#define SAMD51_NVMCTRL_CMD_SEERALOC	(0x32) // Start SmartEEPROM sector reallocation algorithm
#define SAMD51_NVMCTRL_CMD_SEEFLUSH	(0x33) // Flush SmartEEPROM data when in buffered mode
#define SAMD51_NVMCTRL_CMD_LSEE		(0x34) // Lock access to SmartEEPROM data from any memory
#define SAMD51_NVMCTRL_CMD_USEE		(0x35) // Unlock access to SmartEEPROM
#define SAMD51_NVMCTRL_CMD_LSEER	(0x36) // Lock access to the SmartEEPROM Reg.
#define SAMD51_NVMCTRL_CMD_USEER	(0x47) // Unlock access to the SmartEEPROM Reg.


#define SAMD51_NVMCTRL_FLAG_SEEWRC		(1 << 10)
#define SAMD51_NVMCTRL_FLAG_SEESOVF		(1 << 9)
#define SAMD51_NVMCTRL_FLAG_SEESFULL	(1 << 8)
#define SAMD51_NVMCTRL_FLAG_SUSP		(1 << 7)
#define SAMD51_NVMCTRL_FLAG_NVME		(1 << 6)
#define SAMD51_NVMCTRL_FLAG_ECCDE		(1 << 5)
#define SAMD51_NVMCTRL_FLAG_ECCSE		(1 << 4)
#define SAMD51_NVMCTRL_FLAG_LOCKE		(1 << 3)
#define SAMD51_NVMCTRL_FLAG_PROGE		(1 << 2)
#define SAMD51_NVMCTRL_FLAG_ADDRE		(1 << 1)
#define SAMD51_NVMCTRL_FLAG_DONE		(1 << 0)


int samd51_nvmctrl_read(uint32_t phisical_adr, void *buf, size_t size)
{
	memcpy(buf, (void *)phisical_adr, size);

	return AI_OK;
}

int samd51_nvmctrl_write_page(const uint32_t phisical_adr, const void *buf, const size_t npages)
{
	size_t page_size = samd51_nvmctrl_get_page_size();
	if (phisical_adr & (page_size -1)) {
		return AI_ERROR_INVALID;
	}

	int ret = AI_OK;
	NVMCTRL_REG.intflag = NVMCTRL_REG.intflag; // Ensure clear All flags.

	// Manual Write
	NVMCTRL_REG.ctrla = (NVMCTRL_REG.ctrla & ~(0x3 << 4)) | (0 << 4);

	size_t i=0;
	uint32_t *buf_32 = (uint32_t *)buf;	
	uint32_t addr = phisical_adr;
	for (i=0 ;  i<npages ; ++i) {
		//Check Status
		if ((NVMCTRL_REG.status & 0x01) == 0) {
			ret = AI_ERROR_BUSY;
			break;
		}

		//J Buffer Clear
		NVMCTRL_REG.ctrlb = (uint16_t)(((SAMD51_NVM_EXEC_CMD) << 8) | SAMD51_NCMCTRL_CMD_PBC);
		while ((NVMCTRL_REG.intflag & SAMD51_NVMCTRL_FLAG_DONE) == 0);

		//Check Status
		if ((NVMCTRL_REG.status & 0x01) == 0) {
			ret = AI_ERROR_BUSY;
			break;
		}

		// Clear All flags.
		NVMCTRL_REG.intflag = NVMCTRL_REG.intflag;

		// Push data to page buffer
		//J 内部バスからFlash領域に書き込むと、いったんバッファに入る
		int n = 0;
		for (n=0 ; n<page_size ; n+=4, addr+=4, buf_32++) {
			*(uint32_t *)addr = *buf_32;
		}

		// Write Page
		NVMCTRL_REG.ctrlb = (uint16_t)(((SAMD51_NVM_EXEC_CMD) << 8) | SAMD51_NVMCTRL_CMD_WP);
		while ((NVMCTRL_REG.intflag & SAMD51_NVMCTRL_FLAG_DONE) == 0);
		if (NVMCTRL_REG.intflag & ~SAMD51_NVMCTRL_FLAG_DONE) {
			ret = AI_ERROR_NOT_READY;
			break;
		}
		NVMCTRL_REG.intflag = NVMCTRL_REG.intflag; // Clear All flags.
	}

	NVMCTRL_REG.intflag = NVMCTRL_REG.intflag; // Ensure clear All flags.

	return ret;
}

int samd51_nvmctrl_erase_page(const uint32_t phisical_adr, size_t npages)
{
	size_t page_size = samd51_nvmctrl_get_page_size();	
	if (phisical_adr & (page_size -1)) {
		return AI_ERROR_INVALID;
	}

	int ret = AI_OK;

	size_t i=0;
	uint32_t addr = phisical_adr;
	for (i=0 ;  i<npages ; ++i) {
		NVMCTRL_REG.addr = addr;
		NVMCTRL_REG.ctrlb = (uint16_t)(((SAMD51_NVM_EXEC_CMD) << 8) | SAMD51_NVMCTRL_CMD_EB);

		while ((NVMCTRL_REG.intflag & SAMD51_NVMCTRL_FLAG_DONE) == 0);
		if (NVMCTRL_REG.intflag & ~SAMD51_NVMCTRL_FLAG_DONE) {
			ret = AI_ERROR_NOT_READY;
			break;
		}

		addr += page_size;
		NVMCTRL_REG.intflag = NVMCTRL_REG.intflag; // Clear All flags.
	}

	NVMCTRL_REG.intflag = NVMCTRL_REG.intflag; // Ensure clear All flags.

	return ret;
}

size_t samd51_nvmctrl_get_page_size(void)
{
	uint32_t page_size = (NVMCTRL_REG.param >> 16) & 0x00000007;
	page_size = 1 << (page_size + 3);

	return (size_t)page_size;
}


size_t samd51_nvmctrl_get_pages(void)
{
	uint32_t pages = (NVMCTRL_REG.param) & 0x0000ffff;

	return (size_t)pages;
}
