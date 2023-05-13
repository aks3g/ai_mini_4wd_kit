/*
 * max31329.h
 *
 * Created: 2023/03/25 8:21:27
 *  Author: kiyot
 */ 


#ifndef MAX31329_H_
#define MAX31329_H_

#include <time.h>

int max31329_probe(SAMD51_SERCOM sercom, uint32_t *epoc_time);
int max31329_set_time(const struct tm *t);

#endif /* MAX31329_H_ */