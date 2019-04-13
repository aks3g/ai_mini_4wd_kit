/*
 * Copyright (c) 2017 Quantum.inc. All rights reserved.
 * This software is released under the MIT License, see LICENSE.txt.
 */

#include <stdint.h>

#include <samd51_error.h>
#include <samd51_i2c.h>

#include "../include/ai_mini4wd_sensor.h"
#include "../include/internal/lsm6ds3h.h"
#include "../include/internal/sensor.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define LSM6DS3H_ADR									(0x6A)
#define LSM6DS3H_CONTINUOUS								(0x80)

/*---------------------------------------------------------------------------*/
#define LSM6DS3H_RESERVED_0X00							(0x00)
#define LSM6DS3H_FUNC_CFG_ACCESS						(0x01)
#define LSM6DS3H_RESERVED_0X02							(0x02)
#define LSM6DS3H_RESERVED_0X03							(0x03)
#define LSM6DS3H_SENSOR_SYNC_TIME_FRAME					(0x04)
#define LSM6DS3H_RESERVED_0X05							(0x05)
#define LSM6DS3H_FIFO_CTRL1								(0x06)
#define LSM6DS3H_FIFO_CTRL2								(0x07)
#define LSM6DS3H_FIFO_CTRL3								(0x08)
#define LSM6DS3H_FIFO_CTRL4								(0x09)
#define LSM6DS3H_FIFO_CTRL5								(0x0A)
#define LSM6DS3H_ORIENT_CFG_G							(0x0B)				
#define LSM6DS3H_RESERVED_0x0C							(0x0C)
#define LSM6DS3H_INT1_CTRL								(0x0D)
#define LSM6DS3H_INT2_CTRL								(0x0E)
#define LSM6DS3H_WHO_AM_I								(0x0F)
#define LSM6DS3H_CTRL1_XL								(0x10)
#define LSM6DS3H_CTRL2_G								(0x11)
#define LSM6DS3H_CTRL3_C								(0x12)
#define LSM6DS3H_CTRL4_C								(0x13)
#define LSM6DS3H_CTRL5_C								(0x14)
#define LSM6DS3H_CTRL6_C								(0x15)
#define LSM6DS3H_CTRL7_G								(0x16)
#define LSM6DS3H_CTRL8_XL								(0x17)
#define LSM6DS3H_CTRL9_XL								(0x18)
#define LSM6DS3H_CTRL10_C								(0x19)
#define LSM6DS3H_MASTER_CONFIG							(0x1A)
#define LSM6DS3H_WAKE_UP_SRC							(0x1B)
#define LSM6DS3H_TAP_SRC								(0x1C)
#define LSM6DS3H_D6D_SRC								(0x1D)
#define LSM6DS3H_STATUS_REG								(0x1E)
#define LSM6DS3H_RESEVED_0x1F							(0x1F)
#define LSM6DS3H_OUT_TEMP_L								(0x20)
#define LSM6DS3H_OUT_TEMP_H								(0x21)
#define LSM6DS3H_OUTX_L_G								(0x22)
#define LSM6DS3H_OUTX_H_G								(0x23)
#define LSM6DS3H_OUTY_L_G								(0x24)
#define LSM6DS3H_OUTY_H_G								(0x25)
#define LSM6DS3H_OUTZ_L_G								(0x26)
#define LSM6DS3H_OUTZ_H_G								(0x27)
#define LSM6DS3H_OUTX_L_XL								(0x28)
#define LSM6DS3H_OUTX_H_XL								(0x29)
#define LSM6DS3H_OUTY_L_XL								(0x2A)
#define LSM6DS3H_OUTY_H_XL								(0x2B)
#define LSM6DS3H_OUTZ_L_XL								(0x2C)
#define LSM6DS3H_OUTZ_H_XL								(0x2D)

#define LSM6DS3H_SENSORHUB1_REG							(0x2E)
#define LSM6DS3H_SENSORHUB2_REG							(0x2F)
#define LSM6DS3H_SENSORHUB3_REG							(0x30)
#define LSM6DS3H_SENSORHUB4_REG							(0x31)
#define LSM6DS3H_SENSORHUB5_REG							(0x32)
#define LSM6DS3H_SENSORHUB6_REG							(0x33)
#define LSM6DS3H_SENSORHUB7_REG							(0x34)
#define LSM6DS3H_SENSORHUB8_REG							(0x35)
#define LSM6DS3H_SENSORHUB9_REG							(0x36)
#define LSM6DS3H_SENSORHUB10_REG						(0x37)
#define LSM6DS3H_SENSORHUB11_REG						(0x38)
#define LSM6DS3H_SENSORHUB12_REG						(0x39)
#define LSM6DS3H_FIFO_STATUS1							(0x3A)
#define LSM6DS3H_FIFO_STATUS2							(0x3B)
#define LSM6DS3H_FIFO_STATUS3							(0x3C)
#define LSM6DS3H_FIFO_STATUS4							(0x3D)
#define LSM6DS3H_FIFO_DATA_OUT_L						(0x3E)
#define LSM6DS3H_FIFO_DATA_OUT_H						(0x3F)
#define LSM6DS3H_TIMESTAMP0_REG							(0x40)
#define LSM6DS3H_TIMESTAMP1_REG							(0x41)
#define LSM6DS3H_TIMESTAMP2_REG							(0x42)
#define LSM6DS3H_RESERVED_0x43							(0x43)
#define LSM6DS3H_RESERVED_0x44							(0x44)
#define LSM6DS3H_RESERVED_0x45							(0x45)
#define LSM6DS3H_RESERVED_0x46							(0x46)
#define LSM6DS3H_RESERVED_0x47							(0x47)
#define LSM6DS3H_RESERVED_0x48							(0x48)
#define LSM6DS3H_STEP_TIMESTAMP_L						(0x49)		
#define LSM6DS3H_STEP_TIMESTAMP_H						(0x4A)
#define LSM6DS3H_STEP_COUNTER_L							(0x4B)
#define LSM6DS3H_STEP_COUNTER_H							(0x4C)
#define LSM6DS3H_SENSORHUB13_REG						(0x4D)
#define LSM6DS3H_SENSORHUB14_REG						(0x4E)
#define LSM6DS3H_SENSORHUB15_REG						(0x4F)
#define LSM6DS3H_SENSORHUB16_REG						(0x50)
#define LSM6DS3H_SENSORHUB17_REG						(0x51)
#define LSM6DS3H_SENSORHUB18_REG						(0x52)
#define LSM6DS3H_FUNC_SRC								(0x53)
#define LSM6DS3H_RESERVED_0X54							(0x54)
#define LSM6DS3H_RESERVED_0X55							(0x55)
#define LSM6DS3H_RESERVED_0X56							(0x56)
#define LSM6DS3H_RESERVED_0X57							(0x57)
#define LSM6DS3H_TAP_CFG								(0x58)
#define LSM6DS3H_TAP_THS_6D								(0x59)
#define LSM6DS3H_INT_DUR2								(0x5A)
#define LSM6DS3H_WAKE_UP_THS							(0x5B)
#define LSM6DS3H_WAKE_UP_DUR							(0x5C)
#define LSM6DS3H_FREE_FALL								(0x5D)
#define LSM6DS3H_MD1_CFG								(0x5E)
#define LSM6DS3H_MD2_CFG								(0x5F)
#define LSM6DS3H_RESERVED_0X60							(0x60)
#define LSM6DS3H_RESERVED_0X61							(0x61)
#define LSM6DS3H_RESERVED_0X62							(0x62)
#define LSM6DS3H_RESERVED_0X63							(0x63)
#define LSM6DS3H_RESERVED_0X64							(0x64)
#define LSM6DS3H_RESERVED_0XX65							(0x65)
#define LSM6DS3H_OUT_MAG_RAW_X_L						(0x66)
#define LSM6DS3H_OUT_MAG_RAW_X_H						(0x67)
#define LSM6DS3H_OUT_MAG_RAW_Y_L						(0x68)
#define LSM6DS3H_OUT_MAG_RAW_Y_H						(0x69)
#define LSM6DS3H_OUT_MAG_RAW_Z_L						(0x6A)
#define LSM6DS3H_OUT_MAG_RAW_Z_H						(0x6B)
#define LSM6DS3H_RESERVED_0x6C							(0x6C)
#define LSM6DS3H_RESERVED_0x6D							(0x6D)
#define LSM6DS3H_RESERVED_0x6E							(0x6E)
#define LSM6DS3H_RESERVED_0x6F							(0x6F)
#define LSM6DS3H_CTRL_SPIAUX							(0x70)


/*---------------------------------------------------------------------------*/
// INT1 Bit map
#define INT1_STEP_DETECTOR_BP							(7)
#define INT1_SIGN_MOT_BP								(6)
#define INT1_FULL_FLAG_BP								(5)
#define INT1_FIFO_OVR_BP								(4)
#define INT1_FTH_BP										(3)
#define INT1_BOOT_BP									(2)
#define INT1_DRDY_G_BP									(1)
#define INT1_DRDY_XL_BP									(0)

// INT2 Bit map
#define INT2_STEP_DETECTOR_BP							(7)
#define INT2_STEP_COUNT_OV								(6)
#define INT2_FULL_FLAG_BP								(5)
#define INT2_FIFO_OVR_BP								(4)
#define INT2_FTH_BP										(3)
#define INT2_DRDY_TEMP_BP								(2)
#define INT2_DRDY_G_BP									(1)
#define INT2_DRDY_XL_BP									(0)

// CTRL1_XL Bit map
#define ODR_XL3_BP										(7)
#define ODR_XL2_BP										(6)
#define ODR_XL1_BP										(5)
#define ODR_XL0_BP										(4)
#define FS_XL1_BP										(3)
#define FS_XL0_BP										(2)
#define BW_XL1_BP										(1)
#define BW_XL0_BP										(0)

//CTRL2_G Bit map
#define ODR_G3_BP										(7)
#define ODR_G2_BP										(6)
#define ODR_G1_BP										(5)
#define ODR_G0_BP										(4)
#define FS_G1_BP										(3)
#define FS_G0_BP										(2)
#define FS_125_BP										(1)

//CTRL3_C
#define BOOT_BP											(7)
#define BDU_BP											(6)
#define H_LACTIVE_BP									(5)
#define PP_OD_BP										(4)
#define SIM_BP											(3)
#define IF_INC_BP										(2)
#define BLE_BP											(1)
#define SW_RESET_BP										(0)

//CTRL4_C
#define XL_BW_SCAL_ODR_BP								(7)
#define SLEEP_G_BP										(6)
#define INT2_ON_INT1_BP									(5)
#define FIFO_TEMP_EN_BP									(4)
#define DRDY_MASK_BP									(3)
#define I2C_DISABLE_BP									(2)
#define _3_3KHZ_ODR_BP									(1)
#define STOP_ON_FTH_BP									(0)

//CTRL5_C
#define ROUNDING2_BP									(7)
#define ROUNDING1_BP									(6)
#define ROUNDING0_BP									(5)
#define ST1_G_BP										(3)
#define ST0_G_BP										(2)
#define ST1_XL_BP										(1)
#define ST0_XL_BP										(0)

//CTRL6_C
#define TRIG_EN_BP										(7)
#define LVLEN_BP										(6)
#define LVL2_EN_BP										(5)
#define XL_HM_MODE_BP									(4)

//CTRL7_G
#define G_HM_MODE_BP									(7)
#define HP_G_EN_BP										(6)
#define HPCF_G1_BP										(5)
#define HPCF_G0_BP										(4)
#define HP_G_RST_BP										(3)
#define ROUNDING_STATUS_BP								(2)

//CTRL8_XL
#define LPF2_XL_EN_BP									(7)
#define HPCF_XL1_BP										(6)
#define HPCF_XL0_BP										(5)
#define HP_SLOPE_XL_EN_BP								(2)
#define LOW_PASS_ON_6D_BP								(0)

//CTRL9_XL
#define ZEN_XL_BP										(5)
#define YEN_XL_BP										(4)
#define XEN_XL_BP										(3)
#define SOFT_EN_BP										(2)

//CTRL10_C
#define ZEN_G_BP										(5)
#define YEN_G_BP										(4)
#define XEN_G_BP										(3)
#define FUNC_EN_BP										(2)
#define PEDO_RST_STEP_BP								(1)
#define SIGN_MOTION_EN_BP								(0)

//MASTER_CONFIG
#define DRDY_ON_INT1_BP									(7)
#define DATA_VALID_SEL_FIFO_BP							(6)
#define START_CONFIG_BP									(4)
#define PULL_UP_EN_BP									(3)
#define PASS_THROUGH_MODE_BP							(2)
#define IRON_EN_BP										(1)
#define MASTER_ON_BP									(0)



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static void _checkDataReadyAndRead(void);
static void _capture_done_cb(int status);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static volatile uint8_t sAccelReady = 0;
static volatile uint8_t sGyroReady  = 0;
static volatile uint8_t sInhibit = 0;


static AiMini4wdImuRawData sRxBuf;
static uint8_t sTxbuf[1];


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
int lsm6ds3h_probe(void)
{
	uint8_t txbuf[2];
	uint8_t rxbuf[12];
	uint8_t ret = 0;

	// read Who am i Register	
	txbuf[0] = LSM6DS3H_WHO_AM_I;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 1, rxbuf, 1, NULL);
	if (ret != 0) {
		return ret;
	}
	else if (rxbuf[0] != 0x69) {
		return AI_ERROR_NODEV;
	}

	// SW Reset
	uint8_t ctrl3_c = (1 << SW_RESET_BP);
	txbuf[0] = LSM6DS3H_CTRL3_C;
	txbuf[1] = ctrl3_c;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

	volatile uint16_t wait = 0xffff;
	while (wait--);

	// Setup Ctrl1 xl
	uint8_t ctrl1 = (0x03 << ODR_XL0_BP) | (0x01 << FS_XL0_BP) | (0x00 << BW_XL0_BP);
	txbuf[0] = LSM6DS3H_CTRL1_XL;
	txbuf[1] = ctrl1;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

	// Setup Ctrl2 g
	uint8_t ctrl2 = (0x03 << ODR_G0_BP) | (0x03 << FS_G0_BP);
	txbuf[0] = LSM6DS3H_CTRL2_G;
	txbuf[1] = ctrl2;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

	// Setup Ctrl3 c
	uint8_t ctrl3 = (0x01 << BDU_BP) | (0x01 << IF_INC_BP);
	txbuf[0] = LSM6DS3H_CTRL3_C;
	txbuf[1] = ctrl3;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

	// Setup Ctrl4 c
	uint8_t ctrl4 = (0x01 << XL_BW_SCAL_ODR_BP);
	txbuf[0] = LSM6DS3H_CTRL4_C;
	txbuf[1] = ctrl4;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

	// Setup Ctrl9 xl
	uint8_t ctrl9 = (0x01 << ZEN_XL_BP) | (0x01 << YEN_XL_BP) | (0x01 << XEN_XL_BP);
	txbuf[0] = LSM6DS3H_CTRL9_XL;
	txbuf[1] = ctrl9;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

	// Setup Ctrl10 c
	uint8_t ctrl10 = (0x01 << ZEN_G_BP) | (0x01 << YEN_G_BP) | (0x01 << XEN_G_BP);
	txbuf[0] = LSM6DS3H_CTRL10_C;
	txbuf[1] = ctrl10;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

//	sInhibit = 0;

	// Setup INT1 Ctrl
	uint8_t int1_ctrl = (1<<INT1_DRDY_G_BP); //J INT1_DRDY_XL: 加速度データReady
	txbuf[0] = LSM6DS3H_INT1_CTRL;
	txbuf[1] = int1_ctrl;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

	// Setup INT2 Ctrl
	uint8_t int2_ctrl = (1<<INT1_DRDY_XL_BP); //J INT2_DRDY_G:  ジャイロセンサデータReady
	txbuf[0] = LSM6DS3H_INT2_CTRL;
	txbuf[1] = int2_ctrl;
	ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

	//J Data Readyを待つ
	while (1) {
		txbuf[0] = LSM6DS3H_STATUS_REG;
		ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, txbuf, 1, rxbuf, 1, NULL);
		if (ret != 0) {
			continue;
		}
		
		if ((rxbuf[0] & 0x03) == 0x03) {
			break;
		}
	}

	return 0;
}

int lsm6ds3h_grab_oneshot(void)
{
	sInhibit    = 0;
	sAccelReady = 1;
	sGyroReady  = 1;

	_checkDataReadyAndRead();

	return 0;	
}


/*---------------------------------------------------------------------------*/
int lsm6ds3h_on_int1(void)
{
	if (sInhibit || sAccelReady) {
		return -1;
	}

	sAccelReady = 1;

	_checkDataReadyAndRead();
	
	return 0;
}

/*---------------------------------------------------------------------------*/
int lsm6ds3h_on_int2(void)
{
	if (sInhibit || sGyroReady) {
		return -1;
	}

	sGyroReady = 1;

	_checkDataReadyAndRead();

	return 0;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static void _checkDataReadyAndRead(void)
{
	int ret = 0;
	if ((sAccelReady != 0) && (sGyroReady != 0)) {
		sInhibit    = 1;
		sAccelReady = 0;
		sGyroReady  = 0;
				
		sTxbuf[0] = LSM6DS3H_OUTX_L_G;

		ret = -1;
		while ((ret != 0) && (ret != AI_ERROR_I2C_BUSY)) {
			ret = samd51_i2c_txrx(SAMD51_SERCOM3, LSM6DS3H_ADR, sTxbuf, 1, sRxBuf.bytes, 12, _capture_done_cb);
		}
	}

	return;
}

/*---------------------------------------------------------------------------*/
static void _capture_done_cb(int status)
{
	if (status == AI_OK) {
		aiMini4wdUpdateSensorData(&sRxBuf);
	}

	sInhibit = 0;

	return;
}