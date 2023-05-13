/*
 * samd51_irq.h
 *
 * Created: 2023/04/12 21:59:38
 *  Author: 0300004435
 */ 


#ifndef SAMD51_IRQ_H_
#define SAMD51_IRQ_H_

#ifdef SUPPORT_RTOS
#define samd51_enable_irq(num)
#else /*SUPPORT_RTOS*/
#define samd51_enable_irq(num)		NVIC_EnableIRQ((num))
#endif/*SUPPORT_RTOS*/



#endif /* SAMD51_IRQ_H_ */