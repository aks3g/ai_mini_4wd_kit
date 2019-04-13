/*
 * lsm6ds3h.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */

#ifndef LSM6DS3H_H_
#define LSM6DS3H_H_

#define LSM6DS3H_ACCEL_LSB				(0.488f)
#define LSM6DS3H_ANGUL_LSB				(70.0f)


int lsm6ds3h_probe(void);
int lsm6ds3h_on_int1(void);
int lsm6ds3h_on_int2(void);
int lsm6ds3h_grab_oneshot(void);


#endif /* LSM6DS3H_H_ */