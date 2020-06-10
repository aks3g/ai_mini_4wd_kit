/*
 * utils.h
 *
 * Created: 2013/09/07 14:47:29
 *  Author: sazae7
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#ifndef UTILS_H_
#define UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
#ifndef NULL
#define NULL 0
#endif

/*---------------------------------------------------------------------------*/
#define BIT(x)		(1 << x)


/*---------------------------------------------------------------------------*/
/* Interrupt controller */
#define Enable_Int()        {PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm; sei();}
#define Disable_Int()       cli()


#ifdef __cplusplus
};
#endif

#endif /* UTILS_H_ */