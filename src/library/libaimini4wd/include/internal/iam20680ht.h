/*
 * iam20680ht.h
 *
 * Created: 2023/03/20 4:39:33
 *  Author: kiyot
 */ 


#ifndef IAM20680HT_H_
#define IAM20680HT_H_

#define IAM20680HT_ACCEL_LSB				(0.488f)
#define IAM20680HT_ANGUL_LSB				(70.0f)


int iam20680ht_probe(void);
int iam20680ht_on_int1(void);
int iam20680ht_on_int2(void);
int iam20680ht_grab_oneshot(void);

#endif /* IAM20680HT_H_ */