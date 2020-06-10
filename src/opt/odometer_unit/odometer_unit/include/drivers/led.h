/*
 * led.h
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef LED_H_
#define LED_H_

enum LED_IDX_t 
{
	LED0,
	LED1	
};

typedef enum LED_IDX_t LED_IDX;

void led_set(LED_IDX idx, uint8_t on);

#endif /* LED_H_ */