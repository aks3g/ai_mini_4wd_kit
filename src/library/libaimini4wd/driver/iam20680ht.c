/*
 * iam20680ht.c
 *
 * Created: 2023/03/20 4:38:28
 *  Author: kiyot
 */ 
#include <stdint.h>

#include <samd51_error.h>
#include <samd51_i2c.h>

#include "../include/ai_mini4wd.h"
#include "../include/ai_mini4wd_sensor.h"
#include "../include/internal/lsm6ds3h.h"
#include "../include/internal/odometer.h"
#include "../include/internal/sensor.h"


#define IAM20680HT_SELF_TEST_X_GYRO					(0x00)
#define IAM20680HT_SELF_TEST_Y_GYRO					(0x01)
#define IAM20680HT_SELF_TEST_Z_GYRO					(0x02)
#define IAM20680HT_SELF_TEST_X_ACCEL				(0x0D)
#define IAM20680HT_SELF_TEST_Y_ACCEL				(0x0E)
#define IAM20680HT_SELF_TEST_Z_ACCEL				(0x0F)

#define IAM20680HT_XG_OFFS_USRH						(0x13)
#define IAM20680HT_XG_OFFS_USRL						(0x14)
#define IAM20680HT_YG_OFFS_USRH						(0x15)
#define IAM20680HT_YG_OFFS_USRL						(0x16)
#define IAM20680HT_ZG_OFFS_USRH						(0x17)
#define IAM20680HT_ZG_OFFS_USRL						(0x18)

#define IAM20680HT_SMPLRT_DIV						(0x19)
#define IAM20680HT_CONFIG							(0x1A)
#define IAM20680HT_GYRO_CONFIG						(0x1B)
#define IAM20680HT_ACCEL_CONFIG						(0x1C)
#define IAM20680HT_ACCEL_CONFIG2					(0x1D)
#define IAM20680HT_LP_MODE_CFG						(0x1E)
#define IAM20680HT_ACCEL_WOM_THR					(0x1F)

#define IAM20680HT_FIFO_EN							(0x23)
#define IAM20680HT_FSYNC_INT						(0x36)
#define IAM20680HT_INT_PIN_CFG						(0x37)
#define IAM20680HT_INT_ENABLE						(0x38)
#define IAM20680HT_INT_STATUS						(0x3A)

#define IAM20680HT_ACCEL_XOUT_H						(0x3B)
#define IAM20680HT_ACCEL_XOUT_L						(0x3C)
#define IAM20680HT_ACCEL_YOUT_H						(0x3D)
#define IAM20680HT_ACCEL_YOUT_L						(0x3E)
#define IAM20680HT_ACCEL_ZOUT_H						(0x3F)
#define IAM20680HT_ACCEL_ZOUT_L						(0x40)

#define IAM20680HT_TEMP_OUT_H						(0x41)
#define IAM20680HT_TEMP_OUT_L						(0x42)

#define IAM20680HT_GYRO_XOUT_H						(0x43)
#define IAM20680HT_GYRO_XOUT_L						(0x44)
#define IAM20680HT_GYRO_YOUT_H						(0x45)
#define IAM20680HT_GYRO_YOUT_L						(0x46)
#define IAM20680HT_GYRO_ZOUT_H						(0x47)
#define IAM20680HT_GYRO_ZOUT_L						(0x48)

#define IAM20680HT_SIGNAL_PATH_RESET				(0x68)
#define IAM20680HT_ACCEL_INTEL_CTRL					(0x69)
#define IAM20680HT_USER_CTRL						(0x6A)
#define IAM20680HT_PWM_MGMT_1						(0x6B)
#define IAM20680HT_PWM_MGMT_1						(0x6C)

#define IAM20680HT_FIFO_COUNTH						(0x72)
#define IAM20680HT_FIFO_COUNTL						(0x73)
#define IAM20680HT_FIFO_R_W							(0x74)
#define IAM20680HT_WHO_AM_I							(0x75)

#define IAM20680HT_XA_OFFSET_H						(0x77)
#define IAM20680HT_XA_OFFSET_L						(0x78)
#define IAM20680HT_YA_OFFSET_H						(0x7A)
#define IAM20680HT_YA_OFFSET_L						(0x7B)
#define IAM20680HT_ZA_OFFSET_H						(0x7D)
#define IAM20680HT_ZA_OFFSET_L						(0x7E)


int iam20680ht_probe(void);
int iam20680ht_on_int1(void);
int iam20680ht_on_int2(void);
int iam20680ht_grab_oneshot(void);