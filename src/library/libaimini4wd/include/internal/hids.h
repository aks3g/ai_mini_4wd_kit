/*
 * hids.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef HIDS_H_
#define HIDS_H_

int aiMini4wdInitializeSwitch(void);
int aiMini4wdUpdateSwitchStatus(void);

void aiMini4wdSetStatusLed(int on);



#endif /* HIDS_H_ */