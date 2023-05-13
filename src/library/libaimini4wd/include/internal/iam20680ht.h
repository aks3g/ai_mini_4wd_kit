/*
 * iam20680ht.h
 *
 * Created: 2023/03/20 4:39:33
 *  Author: kiyot
 */ 


#ifndef IAM20680HT_H_
#define IAM20680HT_H_

#define IAM20680HT_ACCEL_LSB				(1000.0/2048.0f)
#define IAM20680HT_ANGUL_LSB				(1000.0/16.4f)

#define IAM20680HT_ACCEL_TO_mG(reg_val)		((reg_val) * IAM20680HT_ACCEL_LSB)
#define IAM20680HT_ANGLE_TO_mdps(reg_val)	((reg_val) * IAM20680HT_ANGUL_LSB)


int iam20680ht_probe(void);
int iam20680ht_on_int(void);
int iam20680ht_grab_oneshot(void);

#endif /* IAM20680HT_H_ */