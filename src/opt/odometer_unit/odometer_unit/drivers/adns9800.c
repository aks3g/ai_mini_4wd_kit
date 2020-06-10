/*
 * adhs9800.c
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/pgmspace.h>

#include <spi_driver.h>
#include <gpio_driver.h>

#include "drivers/adns9800_reg.h"
#include "drivers/adns9800_fw.h"
#include "drivers/adns9800.h"
#include "odometer_reg.h"

extern const  uint8_t adns9800_firmware_a4[];
extern const  uint8_t adns9800_firmware_a5[];
extern const  uint8_t adns9800_firmware_a6[];

static const uint8_t *s_srom_addr = adns9800_firmware_a4;

static int32_t sDeltaX = 0;
static int32_t sDeltaY = 0;
static int32_t sDeltaX_mm = 0;
static int32_t sDeltaY_mm = 0;
static uint16_t sCPIx10 = 2000;

static uint8_t sDebugPrintEnabled = 0;



static void _delay(uint32_t usec)
{
	volatile uint32_t _usec = usec * 0.75;
	while (_usec) _usec--;
	return;
}


int adns9800_read(uint8_t offset, uint8_t *value)
{
	gpio_output(GPIO_PORTC, GPIO_PIN4, 0);

	spi_tx(offset | ADNS9800_REG_READ);
	_delay(100);
	*value = spi_rx();

	gpio_output(GPIO_PORTC, GPIO_PIN4, 1);
	
	return 0;
}


int adns9800_write(uint8_t offset, uint8_t value)
{
	gpio_output(GPIO_PORTC, GPIO_PIN4, 0);

	spi_tx(offset | ADNS9800_REG_WRITE);
	spi_tx(value);

	gpio_output(GPIO_PORTC, GPIO_PIN4, 1);
	
	return 0;
}

int adns9800_initialize(void)
{
	//J 起動方法(データシートより)
	// 1. Apply power to VDD5/VDD3 and VDDIO in any order
	// 2. Drive NCS high, and then low to reset the SPI port.
	// 3. Write 0x5a to Power_Up_Reset register (address 0x3a).
	// 4. Wait for at least 50ms time.
	// 5. Read from registers 0x02, 0x03, 0x04, 0x05 and 0x06  one time regardless of the motion pin state.
	// 6. SROM download.
	// 7. Enable laser by setting Forced_Disable bit (Bit-7) of LASER_CTRL0 register (address 0x20) to 0
	adns9800_write(ADNS9800_REG_POWER_UP_RESET, 0x5A);
	_delay(100*1000L);

	uint8_t dummy = 0;
	adns9800_read(ADNS9800_REG_MOTION,    &dummy);
	adns9800_read(ADNS9800_REG_DELTA_X_L, &dummy);
	adns9800_read(ADNS9800_REG_DELTA_X_H, &dummy);
	adns9800_read(ADNS9800_REG_DELTA_Y_L, &dummy);
	adns9800_read(ADNS9800_REG_DELTA_Y_H, &dummy);

	//J SROM情報を送りつける
	// 1. Select the 3 K bytes SROM size at Configuration_IV register, address 0x39
	// 2. Write 0x1d to SROM_Enable register for initializing
	// 3. Wait for one frame
	// 4. Write 0x18 to SROM_Enable register again to start SROM downloading
	// 5. Write SROM file into SROM_Load_Burst register, 1st data must start with
	//    SROM_Load_Burst register address. All the
	//    SROM data must be downloaded before SROM start running.
	adns9800_write(ADNS9800_REG_CONFIGURATION_IV, 0x02);
	adns9800_write(ADNS9800_REG_SROM_ENABLE, 0x1D);		//J マジックナンバー？
	_delay(10 * 1000L);
	adns9800_write(ADNS9800_REG_SROM_ENABLE, 0x18);		//J マジックナンバー？

	uint16_t i = 0;
	uint16_t srom = (uint16_t) s_srom_addr;
	uint8_t val = 0;
	gpio_output(GPIO_PORTC, GPIO_PIN4, 0);
	for (i=0 ; i<FIRMWARE_LENGTH ; ++i) {
		val = pgm_read_byte(srom + i);
		spi_tx(val);
		_delay(100);
	}
	gpio_output(GPIO_PORTC, GPIO_PIN4, 1);
	_delay(200 * 1000L);


	uint8_t config1 = 0x01;
	adns9800_write(ADNS9800_REG_CONFIGURATION_I, config1);

	//J 7200fpsに設定
	//J 36000m/hで進むとすると、1secで10mすすむ
	//J 7200fpsで動くとすると、1frameは1/7200[sec]
	//J 1frameあたり 10/7200 [m] = 0.0014[m] = 1.4[mm]動く
	// Frame_Period_Max_Bound = 0x1b20(6944)
	uint8_t value = 0x20;
	adns9800_write(ADNS9800_REG_FRAME_PERIOD_MAX_BOUND_LOWER, value);
	value = 0x1b;
	adns9800_write(ADNS9800_REG_FRAME_PERIOD_MAX_BOUND_UPPER, value);

	// Frame_Period_Min_Bound = 0x0fa0(4000)
	value = 0xa0;
	adns9800_write(ADNS9800_REG_FRAME_PERIOD_MIN_BOUND_LOWER, value);
	value = 0x0f;
	adns9800_write(ADNS9800_REG_FRAME_PERIOD_MIN_BOUND_UPPER, value);

	// Shutter_Max_Bound = 0x0b80(2944)
	value = 0x80;
	adns9800_write(ADNS9800_REG_SHUTTER_MAX_BOUND_LOWER, value);
	value = 0x0b;
	adns9800_write(ADNS9800_REG_SHUTTER_MAX_BOUND_UPPER, value);

	//J レーザーレジスタの内容を変更
	uint8_t laserCtrl0 = 0;
	adns9800_read(ADNS9800_REG_LASER_CTRL0, &laserCtrl0);
	laserCtrl0 = laserCtrl0 & ~(0x81);
	adns9800_write(ADNS9800_REG_LASER_CTRL0, laserCtrl0);

	sDeltaX = 0;
	sDeltaY = 0;

	return 0;
}

int adns9800_update(void)
{
	Adn9800MotionData data;
	uint8_t *ptr = (uint8_t *)&data;
	uint8_t i = 0;
	gpio_output(GPIO_PORTC, GPIO_PIN4, 0);

	spi_tx(ADNS9800_REG_MOTION_BURST);
	_delay(100);
	
	for (i=0; i<sizeof(Adn9800MotionData) ; ++i) {
		ptr[i]= spi_rx();
	}

	gpio_output(GPIO_PORTC, GPIO_PIN4, 1);

	sDeltaX += data.delta_x;
	sDeltaY += data.delta_y;

	sDeltaX_mm = sDeltaX * 254 / sCPIx10;
	sDeltaY_mm = sDeltaY * 254 / sCPIx10;

	reg_write(REG_MOTION,    ptr[0]);
	reg_write(REG_SQUAL,     ptr[6]);
	reg_write(REG_PIXEL_SUM, ptr[7]);

	ptr = (uint8_t *)&sDeltaX_mm;
	reg_write(REG_DELTAX_0, ptr[0]);
	reg_write(REG_DELTAX_1, ptr[1]);
	reg_write(REG_DELTAX_2, ptr[2]);
	reg_write(REG_DELTAX_3, ptr[3]);
	ptr = (uint8_t *)&sDeltaY_mm;
	reg_write(REG_DELTAY_0, ptr[0]);
	reg_write(REG_DELTAY_1, ptr[1]);
	reg_write(REG_DELTAY_2, ptr[2]);
	reg_write(REG_DELTAY_3, ptr[3]);

	return 0;
}

int adns9800_set_cpi(uint16_t cpi)
{
	sCPIx10 = 10 * cpi;
	
	return 0;
}

int adns9800_debug_print(void)
{
	if (!sDebugPrintEnabled) {
		return -1;
	}
	printf("Delta %8ld, %8ld\r", sDeltaX_mm, sDeltaY_mm);

	return 0;
}

int adns9800_enable_debug_print(int set)
{
	sDebugPrintEnabled = set;
	
	return 0;
}

int adns9800_reset(uint8_t assert)
{
	if (assert) {
		adns9800_initialize();
		reg_write(REG_RESET, 0x00);
	}
	
	return 0;
}

int adns9800_switch_srom(uint8_t srom_id)
{
	if (srom_id == 0xa4) {
		s_srom_addr = adns9800_firmware_a4;
	}
	else if (srom_id == 0xa5) {
		s_srom_addr = adns9800_firmware_a5;
	}
	else if (srom_id == 0xa5) {
		s_srom_addr = adns9800_firmware_a6;
	}
	else {
		return -2;
	}

	return 0;
}
