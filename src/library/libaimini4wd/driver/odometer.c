/*
 * odometer.c
 *
 * Created: 2021/03/01 9:51:14
 * Copyright ? 2021 Kiyotaka Akasaka. All rights reserved.
 */ 

#include <stdint.h>
#include <string.h>

#include <samd51_error.h>
#include <samd51_i2c.h>

#include "internal/odometer.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define ODOMETER_ADR					(0x33)

/*---------------------------------------------------------------------------*/
#define ODOMETER_REG_SIZE				(0x20)

#define ODOMETER_REG_IDENT				(0x00)
#define ODOMETER_REG_HW_VERSION			(0x01)
#define ODOMETER_REG_FW_VERSION			(0x02)
#define ODOMETER_REG_RESET				(0x03)
#define ODOMETER_REG_STATUS				(0x04)
#define ODOMETER_REG_SROM_VERSION		(0x05)
#define ODOMETER_REG_LED				(0x06)
#define ODOMETER_REG_DEBUG_PRINT		(0x07)

#define ODOMETER_REG_PRODUCT_ID			(0x10)
#define ODOMETER_REG_REVISION_ID		(0x11)
#define ODOMETER_REG_MOTION				(0x12)
#define ODOMETER_REG_DELTAX_0			(0x13)
#define ODOMETER_REG_DELTAX_1			(0x14)
#define ODOMETER_REG_DELTAX_2			(0x15)
#define ODOMETER_REG_DELTAX_3			(0x16)
#define ODOMETER_REG_DELTAY_0			(0x17)
#define ODOMETER_REG_DELTAY_1			(0x18)
#define ODOMETER_REG_DELTAY_2			(0x19)
#define ODOMETER_REG_DELTAY_3			(0x1A)
#define ODOMETER_REG_SQUAL				(0x1B)
#define ODOMETER_REG_PIXEL_SUM			(0x1C)
#define ODOMETER_REG_CPI_L				(0x1D)
#define ODOMETER_REG_CPI_H				(0x1E)


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef struct OdometerInfo_t
{
	uint8_t hw_rev;
	uint8_t fw_rev;
	uint8_t srom_version;
} OdometerInfo;

static OdometerInfo info;
static OdometerData sOdometerData;
static volatile int sOdometerBusy = 0;
static volatile int sOdometerReady = 0;

float gMMpCnt = (25.4f / 1601.0f);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
int odometer_probe(void)
{
	uint8_t txbuf[2];
	uint8_t rxbuf[1];
	int ret = 0;

	// read Who am i Register
	txbuf[0] = ODOMETER_REG_IDENT;
	ret = samd51_i2c_txrx(SAMD51_SERCOM2, ODOMETER_ADR, txbuf, 1, rxbuf, 1, NULL);
	if (ret != 0) {
		return ret;
	}
	else if (rxbuf[0] != 0xa1) {
		return AI_ERROR_NODEV;
	}

	// Read Hw Information
	txbuf[0] = ODOMETER_REG_HW_VERSION;
	ret = samd51_i2c_txrx(SAMD51_SERCOM2, ODOMETER_ADR, txbuf, 1, &info.hw_rev, 1, NULL);
	if (ret != 0) {
		return ret;
	}

	txbuf[0] = ODOMETER_REG_FW_VERSION;
	ret = samd51_i2c_txrx(SAMD51_SERCOM2, ODOMETER_ADR, txbuf, 1, &info.fw_rev, 1, NULL);
	if (ret != 0) {
		return ret;
	}

	txbuf[0] = ODOMETER_REG_SROM_VERSION;
	ret = samd51_i2c_txrx(SAMD51_SERCOM2, ODOMETER_ADR, txbuf, 1, &info.srom_version, 1, NULL);
	if (ret != 0) {
		return ret;
	}

	sOdometerReady = 1;
	memset(&sOdometerData, 0, sizeof(sOdometerData));
	
	return ret;
}


static void _odometry_data_done(int error)
{
	(void)error;
	sOdometerBusy = 0;
}

int odometer_grab(void)
{
	if (!sOdometerReady) {
		return AI_ERROR_NODEV;
	}
	
	static uint8_t txbuf[1] = {ODOMETER_REG_MOTION};

	sOdometerBusy = 1;
	txbuf[0] = ODOMETER_REG_DELTAX_0;
	memset(&sOdometerData, 0, sizeof(sOdometerData));

	int ret =  samd51_i2c_txrx(SAMD51_SERCOM2, ODOMETER_ADR, txbuf, 1, (uint8_t *)&sOdometerData, sizeof(sOdometerData), _odometry_data_done);

	if (ret) while(1);

	return ret;
}

OdometerData *odometer_get_latest_data(void)
{
	return &sOdometerData;
}

int odometer_is_busy(void) {
	if (!sOdometerReady) {
		return 0;
	}

	return sOdometerBusy;
}

void odometer_set_cpi(uint32_t cpi)
{
	gMMpCnt = 25.4f / (float)cpi;
}