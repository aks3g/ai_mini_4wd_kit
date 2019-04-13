/*
 * ai_mini4wd_motor_driver.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef AI_MINI4WD_MOTOR_DRIVER_H_
#define AI_MINI4WD_MOTOR_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

int aiMini4wdMotorDriverDrive(int duty);
int aiMini4wdMotorDriverSetRpm(int rpm);
int aiMini4wdMotorDriverSetPidGain(float Kp, float Ki, float Kd);

int aiMini4wdMotorDriverGetDriveCurrent(float *current_mA);

#ifdef __cplusplus
}
#endif

#endif /* AI_MINI4WD_MOTOR_DRIVER_H_ */