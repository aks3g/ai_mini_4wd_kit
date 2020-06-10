/*
 * adns9800.h
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef ADHS9800_H_
#define ADHS9800_H_

#include "drivers/adns9800_reg.h"

typedef struct Adn9800MotionData_t
{
	uint8_t motion;
	uint8_t observation;
	int16_t delta_x;
	int16_t delta_y;
	uint8_t squal;
	uint8_t pixel_sum;
	uint8_t maximum_pixel;
	uint8_t minimum_pixel;
	uint8_t shutter_upper;
	uint8_t shutter_lower;
	uint8_t frame_period_upper;
	uint8_t frame_period_lower;
} Adn9800MotionData;

int adns9800_initialize(void);
int adns9800_reset(uint8_t assert);

int adns9800_switch_srom(uint8_t srom_id);

int adns9800_read(uint8_t offset, uint8_t *value);
int adns9800_write(uint8_t offset, uint8_t value);

int adns9800_update(void);
int adns9800_set_cpi(uint16_t cpi);

int adns9800_debug_print(void);
int adns9800_enable_debug_print(int set);

#endif /* ADHS9800_H_ */