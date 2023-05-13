#include "ai_mini4wd_cfg.h"
#include "ai_mini4wd_trace.h"

extern void AC_Handler(void);
extern void ADC0_1_Handler(void);
extern void DMAC_0_Handler(void);
extern void DMAC_1_Handler(void);
extern void DMAC_2_Handler(void);
extern void DMAC_3_Handler(void);
extern void DMAC_4_Handler(void);
extern void EIC_0_Handler(void);
extern void EIC_1_Handler(void);
extern void EIC_2_Handler(void);
extern void EIC_3_Handler(void);
extern void EIC_4_Handler(void);
extern void EIC_5_Handler(void);
extern void EIC_6_Handler(void);
extern void EIC_7_Handler(void);
extern void EIC_8_Handler(void);
extern void EIC_9_Handler(void);
extern void EIC_10_Handler(void);
extern void EIC_11_Handler(void);
extern void EIC_12_Handler(void);
extern void EIC_13_Handler(void);
extern void EIC_14_Handler(void);
extern void EIC_15_Handler(void);
extern void SERCOM2_0_Handler(void);
extern void SERCOM2_1_Handler(void);
extern void SERCOM3_0_Handler(void);
extern void SERCOM3_1_Handler(void);
extern void TC0_Handler(void);
extern void TC2_Handler(void);

extern ulong_t	_kernel_current_time;
/*
 * 本来libsamd51.aの中にいる割込みハンドラへのゲートウェイ
 */
void isr_ac(intptr_t exinf)
{
	(void)exinf;
	AC_Handler();
}

void isr_adc0_1(intptr_t exinf)
{
	(void)exinf;
	ADC0_1_Handler();
}

void isr_dmac0(intptr_t exinf)
{
	(void)exinf;
	DMAC_0_Handler();
}
void isr_dmac1(intptr_t exinf)
{
	(void)exinf;
	DMAC_1_Handler();
}
void isr_dmac2(intptr_t exinf)
{
	(void)exinf;
	DMAC_2_Handler();
}

void isr_dmac3(intptr_t exinf)
{
	(void)exinf;
	DMAC_3_Handler();
}


void isr_dmac4(intptr_t exinf)
{
	(void)exinf;
	DMAC_4_Handler();
}


void isr_eic0(intptr_t exinf)
{
	(void)exinf;
	EIC_0_Handler();
}


void isr_eic1(intptr_t exinf)
{
	(void)exinf;
	EIC_1_Handler();
}


void isr_eic2(intptr_t exinf)
{
	(void)exinf;
	EIC_2_Handler();
}


void isr_eic3(intptr_t exinf)
{
	(void)exinf;
	EIC_3_Handler();
}


void isr_eic4(intptr_t exinf)
{
	(void)exinf;
	EIC_4_Handler();
}


void isr_eic5(intptr_t exinf)
{
	(void)exinf;
	EIC_5_Handler();
}


void isr_eic6(intptr_t exinf)
{
	(void)exinf;
	EIC_6_Handler();
}


void isr_eic7(intptr_t exinf)
{
	(void)exinf;
	EIC_7_Handler();
}


void isr_eic8(intptr_t exinf)
{
	(void)exinf;
	EIC_8_Handler();
}


void isr_eic9(intptr_t exinf)
{
	(void)exinf;
	EIC_9_Handler();
}


void isr_eic10(intptr_t exinf)
{
	(void)exinf;
	EIC_10_Handler();
}


void isr_eic11(intptr_t exinf)
{
	(void)exinf;
	EIC_11_Handler();
}


void isr_eic12(intptr_t exinf)
{
	(void)exinf;
	EIC_12_Handler();
}


void isr_eic13(intptr_t exinf)
{
	(void)exinf;
	EIC_13_Handler();
}


void isr_eic14(intptr_t exinf)
{
	(void)exinf;
	EIC_14_Handler();
}


void isr_eic15(intptr_t exinf)
{
	(void)exinf;
	EIC_15_Handler();
}


void isr_sercom2_0(intptr_t exinf)
{
	(void)exinf;
	SERCOM2_0_Handler();
}


void isr_sercom2_1(intptr_t exinf)
{
	(void)exinf;
	SERCOM2_1_Handler();
}


void isr_sercom3_0(intptr_t exinf)
{
	(void)exinf;
	SERCOM3_0_Handler();
}


void isr_sercom3_1(intptr_t exinf)
{
	(void)exinf;
	SERCOM3_1_Handler();
}


void isr_tc0(intptr_t exinf)
{
	(void)exinf;
	TC0_Handler();
}


void isr_tc2(intptr_t exinf)
{
	(void)exinf;
	TC2_Handler();
}


