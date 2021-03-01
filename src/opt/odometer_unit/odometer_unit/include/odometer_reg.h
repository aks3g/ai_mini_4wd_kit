/*
 * odometer_reg.h
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef ODOMETER_REG_H_
#define ODOMETER_REG_H_

#define REG_SIZE				(0x20)

#define REG_IDENT				(0x00)
#define REG_HW_VERSION			(0x01)
#define REG_FW_VERSION			(0x02)
#define REG_RESET				(0x03)
#define REG_STATUS				(0x04)
#define REG_SROM_VERSION		(0x05)
#define REG_LED					(0x06)
#define REG_DEBUG_PRINT			(0x07)

#define REG_PRODUCT_ID			(0x10)
#define REG_REVISION_ID			(0x11)
#define REG_MOTION				(0x12)
#define REG_DELTAX_0			(0x13)
#define REG_DELTAX_1			(0x14)
#define REG_DELTAX_2			(0x15)
#define REG_DELTAX_3			(0x16)
#define REG_DELTAY_0			(0x17)
#define REG_DELTAY_1			(0x18)
#define REG_DELTAY_2			(0x19)
#define REG_DELTAY_3			(0x1A)
#define REG_SQUAL				(0x1B)
#define REG_PIXEL_SUM			(0x1C)
#define REG_CPI_L				(0x1D)
#define REG_CPI_H				(0x1E)

uint8_t reg_initialize(void);
uint8_t reg_read(uint8_t offset, uint8_t *data);
uint8_t reg_write(uint8_t offset, uint8_t data);

typedef union dword_byte_t
{
	uint32_t dword;
	uint8_t  bytes[4];
} dword_byte;

extern dword_byte gDeltaX_mm;
extern dword_byte gDeltaY_mm;

#endif /* ODOMETER_REG_H_ */