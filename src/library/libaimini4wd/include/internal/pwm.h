/*
 * pwm.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef PWM_H_
#define PWM_H_

int aiMini4WdInitializePwm(void);
int aiMini4wdMotorDriverUpdateRpm(float rpm);

#endif /* PWM_H_ */