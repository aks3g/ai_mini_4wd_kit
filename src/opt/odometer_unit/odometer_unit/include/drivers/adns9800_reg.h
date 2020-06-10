/*
 * adns9800_reg.h
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef AVAGO_ADNS_9800_H_
#define AVAGO_ADNS_9800_H_


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define ADNS9800_REG_PRODUCT_ID						(0x00)
#define ADNS9800_REG_REVISION_ID					(0x01)
#define ADNS9800_REG_MOTION							(0x02)
#define ADNS9800_REG_DELTA_X_L						(0x03)
#define ADNS9800_REG_DELTA_X_H						(0x04)
#define ADNS9800_REG_DELTA_Y_L						(0x05)
#define ADNS9800_REG_DELTA_Y_H						(0x06)
#define ADNS9800_REG_SQUAL							(0x07)
#define ADNS9800_REG_PIXEL_SUM						(0x08)
#define ADNS9800_REG_MAXIMUM_PIXEL					(0x09)
#define ADNS9800_REG_MINIMUM_PIXEL					(0x0A)
#define ADNS9800_REG_SHUTTER_LOWER					(0x0B)
#define ADNS9800_REG_SHUTTER_UPPER					(0x0C)
#define ADNS9800_REG_FRAME_PERIOD_LOWER				(0x0D)
#define ADNS9800_REG_FRAME_PERIOD_UPPER				(0x0E)
#define ADNS9800_REG_CONFIGURATION_I				(0x0F)
#define ADNS9800_REG_CONFIGURATION_II				(0x10)
#define ADNS9800_REG_FRAME_CAPTURE					(0x12)
#define ADNS9800_REG_SROM_ENABLE					(0x13)
#define ADNS9800_REG_RUN_DOWNSHIFT					(0x14)
#define ADNS9800_REG_REST1_RATE						(0x15)
#define ADNS9800_REG_REST1_DOWNSHIFT				(0x16)
#define ADNS9800_REG_REST2_RATE						(0x17)
#define ADNS9800_REG_REST2_DOWNSHIFT				(0x18)
#define ADNS9800_REG_REST3_RATE						(0x19)
#define ADNS9800_REG_FRAME_PERIOD_MAX_BOUND_LOWER	(0x1A)
#define ADNS9800_REG_FRAME_PERIOD_MAX_BOUND_UPPER	(0x1B)
#define ADNS9800_REG_FRAME_PERIOD_MIN_BOUND_LOWER	(0x1C)
#define ADNS9800_REG_FRAME_PERIOD_MIN_BOUND_UPPER	(0x1D)
#define ADNS9800_REG_SHUTTER_MAX_BOUND_LOWER		(0x1E)
#define ADNS9800_REG_SHUTTER_MAX_BOUND_UPPER		(0x1F)
#define ADNS9800_REG_LASER_CTRL0					(0x20)
#define ADNS9800_REG_OBSERVATION					(0x24)
#define ADNS9800_REG_DATA_OUT_LOWER					(0x25)
#define ADNS9800_REG_DATA_OUT_UPPER					(0x26)
#define ADNS9800_REG_SROM_ID						(0x2A)
#define ADNS9800_REG_LIFT_DETECTION_THR				(0x2E)
#define ADNS9800_REG_CONFIGURATION_V				(0x2f)
#define ADNS9800_REG_CONFIGURATION_IV				(0x39)
#define ADNS9800_REG_POWER_UP_RESET					(0x3A)
#define ADNS9800_REG_SHUTDOWN						(0x3B)
#define ADNS9800_REG_INVERSE_PRODUCT_ID				(0x3F)
#define ADNS9800_REG_SNAP_ANGLE						(0x42)
#define ADNS9800_REG_MOTION_BURST					(0x50)
#define ADNS9800_REG_SROM_LOAD_BURST				(0x62)
#define ADNS9800_REG_PIXEL_BURST					(0x64)

#define ADNS9800_REG_READ							(0x00)
#define ADNS9800_REG_WRITE							(0x80)


#endif /* AVAGO_ADNS_9800_H_ */