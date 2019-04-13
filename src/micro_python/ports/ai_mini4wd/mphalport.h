/*
 * mphalport.h
 *
 * Created: 2019/03/10
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef MPHALPORT_H_
#define MPHALPORT_H_


static inline mp_uint_t mp_hal_ticks_ms(void) { return 0; }
static inline void mp_hal_set_interrupt_char(char c) {}



#endif /* MPHALPORT_H_ */