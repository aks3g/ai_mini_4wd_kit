#ifndef AI_MINI4WD_CFG_H_
#define AI_MINI4WD_CFG_H_

#include "samd51x.h"

#define INTPRI_AC				(-3)
#define INTPRI_ADC0_1			(-3)
#define INTPRI_DMAC0			(-3)
#define INTPRI_DMAC1			(-3)
#define INTPRI_DMAC2			(-3)
#define INTPRI_DMAC3			(-3)
#define INTPRI_DMAC4			(-3)
#define INTPRI_EIC0				(-3)
#define INTPRI_EIC1				(-3)
#define INTPRI_EIC2				(-3)
#define INTPRI_EIC3				(-3)
#define INTPRI_EIC4				(-3)
#define INTPRI_EIC5				(-3)
#define INTPRI_EIC6				(-3)
#define INTPRI_EIC7				(-3)
#define INTPRI_EIC8				(-3)
#define INTPRI_EIC9				(-3)
#define INTPRI_EIC10			(-3)
#define INTPRI_EIC11			(-3)
#define INTPRI_EIC12			(-3)
#define INTPRI_EIC13			(-3)
#define INTPRI_EIC14			(-3)
#define INTPRI_EIC15			(-3)
#define INTPRI_SERCOM2_0		(-3)
#define INTPRI_SERCOM2_1		(-3)
#define INTPRI_SERCOM3_0		(-3)
#define INTPRI_SERCOM3_1		(-3)
#define INTPRI_TC0				(-3)
#define INTPRI_TC2				(-3)

#define INTATR_AC				(0)
#define INTATR_ADC0_1			(0)
#define INTATR_DMAC0			(0)
#define INTATR_DMAC1			(0)
#define INTATR_DMAC2			(0)
#define INTATR_DMAC3			(0)
#define INTATR_DMAC4			(0)
#define INTATR_EIC0				(0)
#define INTATR_EIC1				(0)
#define INTATR_EIC2				(0)
#define INTATR_EIC3				(0)
#define INTATR_EIC4				(0)
#define INTATR_EIC5				(0)
#define INTATR_EIC6				(0)
#define INTATR_EIC7				(0)
#define INTATR_EIC8				(0)
#define INTATR_EIC9				(0)
#define INTATR_EIC10			(0)
#define INTATR_EIC11			(0)
#define INTATR_EIC12			(0)
#define INTATR_EIC13			(0)
#define INTATR_EIC14			(0)
#define INTATR_EIC15			(0)
#define INTATR_SERCOM2_0		(0)
#define INTATR_SERCOM2_1		(0)
#define INTATR_SERCOM3_0		(0)
#define INTATR_SERCOM3_1		(0)
#define INTATR_TC0				(0)
#define INTATR_TC2				(0)

extern void isr_ac(intptr_t exinf);
extern void isr_adc0_1(intptr_t exinf);
extern void isr_dmac0(intptr_t exinf);
extern void isr_dmac1(intptr_t exinf);
extern void isr_dmac2(intptr_t exinf);
extern void isr_dmac3(intptr_t exinf);
extern void isr_dmac4(intptr_t exinf);
extern void isr_eic0(intptr_t exinf);
extern void isr_eic1(intptr_t exinf);
extern void isr_eic2(intptr_t exinf);
extern void isr_eic3(intptr_t exinf);
extern void isr_eic4(intptr_t exinf);
extern void isr_eic5(intptr_t exinf);
extern void isr_eic6(intptr_t exinf);
extern void isr_eic7(intptr_t exinf);
extern void isr_eic8(intptr_t exinf);
extern void isr_eic9(intptr_t exinf);
extern void isr_eic1(intptr_t exinf);
extern void isr_eic1(intptr_t exinf);
extern void isr_eic1(intptr_t exinf);
extern void isr_eic1(intptr_t exinf);
extern void isr_eic1(intptr_t exinf);
extern void isr_eic1(intptr_t exinf);
extern void isr_sercom2_0(intptr_t exinf);
extern void isr_sercom2_1(intptr_t exinf);
extern void isr_sercom3_0(intptr_t exinf);
extern void isr_sercom3_1(intptr_t exinf);
extern void isr_tc0(intptr_t exinf);
extern void isr_tc2(intptr_t exinf);

#endif/*AI_MINI4WD_CFG_H_*/