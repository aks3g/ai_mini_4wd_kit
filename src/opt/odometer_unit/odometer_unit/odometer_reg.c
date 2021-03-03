/*
 * odometer_reg.c
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */
#include <stdint.h>
#include <string.h>

#include "odometer_reg.h"
#include "odometer_reg_cb.h"

#include "drivers/adns9800.h"



typedef void (*reg_write_cb)(uint8_t);
static uint8_t sReg[REG_SIZE];

static reg_write_cb sRegWriteCbSet[REG_SIZE] = 
{
/* 00 */	NULL,
/* 01 */	NULL,
/* 02 */	NULL,
/* 03 */	on_write_reg_reset,
/* 04 */	NULL,
/* 05 */	on_write_reg_srom_version,
/* 06 */	on_write_reg_led,
/* 07 */	on_write_reg_debug_print_enable,
/* 08 */	NULL,
/* 09 */	NULL,
/* 0A */	NULL,
/* 0B */	NULL,
/* 0C */	NULL,
/* 0D */	NULL,
/* 0E */	NULL,
/* 0F */	NULL,
/* 10 */	NULL,
/* 11 */	NULL,
/* 12 */	NULL,
/* 13 */	NULL,
/* 14 */	NULL,
/* 15 */	NULL,
/* 16 */	NULL,
/* 17 */	NULL,
/* 18 */	NULL,
/* 19 */	NULL,
/* 1A */	NULL,
/* 1B */	NULL,
/* 1C */	NULL,
/* 1D */	on_write_cpi_l,
/* 1E */	on_write_cpi_h,
/* 1F */	NULL,
};

dword_byte gDeltaX_mm = {.dword = 0};
dword_byte gDeltaY_mm = {.dword = 0};

uint8_t reg_initialize(void)
{
	memset (sReg, 0, sizeof(sReg));

	sReg[REG_IDENT] = 0xa1;
	sReg[REG_HW_VERSION] = 0x01;
	sReg[REG_FW_VERSION] = 0x01;
	sReg[REG_SROM_VERSION] = 0xa4;

	adns9800_read(ADNS9800_REG_PRODUCT_ID,  &sReg[REG_PRODUCT_ID]);
	adns9800_read(ADNS9800_REG_REVISION_ID, &sReg[REG_REVISION_ID]);

	sReg[REG_CPI_L] = 200;
	sReg[REG_CPI_H] = 0;

	return 0;
}

uint8_t reg_read(uint8_t offset, uint8_t *data)
{
	if (offset >= REG_SIZE) {
		return 0xff;
	}

	*data = sReg[offset];

	return 0;
}

uint8_t reg_write(uint8_t offset, uint8_t data)
{
	if (offset >= REG_SIZE) {
		return 0xff;
	}

	if (sRegWriteCbSet[offset] != NULL) {
		sRegWriteCbSet[offset](data);
	}

	sReg[offset] = data;
	return 0;
}

void reg_update_delta(void)
{
	memcpy(&(sReg[REG_DELTAX_0]), &gDeltaX_mm.dword, sizeof(int32_t));
	memcpy(&(sReg[REG_DELTAY_0]), &gDeltaY_mm.dword, sizeof(int32_t));

	memset(&(gDeltaX_mm.dword), 0, sizeof(int32_t));
	memset(&(gDeltaY_mm.dword), 0, sizeof(int32_t));
}