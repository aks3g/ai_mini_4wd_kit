/*
 * samd51_clock.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */
#include <stddef.h>
#include <stdint.h>

#include <sam.h>

#include <samd51_error.h>
#include <samd51_clock.h>

typedef struct STRUCT_GCLK_GENCTRL_REG_t
{
	uint32_t genctrl[12];
} STRUCT_GCLK_GENCTRL_REG;

#define GCLK_GENCTRL_REG		(*(volatile STRUCT_GCLK_GENCTRL_REG *)0x40001C20UL)

typedef struct STRUCT_GCLK_PCHCTRL_REG_t
{
	uint32_t pchctrl[48];
} STRUCT_GCLK_PCHCTRL_REG;

#define GCLK_PCHCTRL_REG		(*(volatile STRUCT_GCLK_PCHCTRL_REG *)0x40001C80UL)

#pragma pack(1)
typedef struct STRUCT_OSC_REG_t
{
	uint8_t evctrl;
	uint8_t rsvd[3];
	uint32_t intenclr;
	uint32_t intenset;
	uint32_t intflag;
	uint32_t status;
	uint32_t xoscctrl0;
	uint32_t xoscctrl1;
	uint8_t dfllctrla;
	uint8_t rsvd1[3];
	uint8_t dfllctrlb;
	uint8_t rsvd2[3];
	uint32_t dfflval;
	uint32_t dfllmul;
	uint8_t dfllsync;
	uint8_t rsvd3[3];
	uint8_t dpll0ctrla;
	uint8_t rsvd4[3];
	uint32_t dpll0ratio;
	uint32_t dpll0ctrlb;
	uint32_t dpll0syncbusy;
	uint32_t dpll0status;
	uint8_t dpll1ctrla;
	uint8_t rsvd5[3];
	uint32_t dpll1ratio;
	uint32_t dpll1ctrlb;
	uint32_t dpll1syncbusy;
	uint32_t dpll1status;
} STRUCT_OSC_REG;

#define OSC_REG					(*(volatile STRUCT_OSC_REG*)0x40001000UL)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_gclk_configure_generator(uint8_t generator_id, SAMD51_GCLK_SRC src, uint16_t div, uint8_t run_standby, SAMD51_GCLK_DIV_MODE div_mode)
{
	if (generator_id >= GCLK_GEN_NUM) {
		return AI_ERROR_NODEV;
	}

	GCLK_GENCTRL_REG.genctrl[generator_id] = (((uint32_t)div) << 16) | (run_standby ? (1<<13) : 0) | (div_mode ? (1<<12) : 0) | (uint32_t)(1 << 8) | (uint32_t)src ;
	
	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_gclk_configure_peripheral_channel(SAMD51_PERIPHERAL_CHANNEL channel, uint8_t generator_id)
{
	if (generator_id >= GCLK_GEN_NUM) {
		return AI_ERROR_NODEV;
	}

	GCLK_PCHCTRL_REG.pchctrl[channel] = generator_id | (1 << 6); // Enable;

	return AI_OK;	
}

/*--------------------------------------------------------------------------*/
int samd51_mclk_enable(SAMD51_MCLK_DEVICE dev, uint8_t enable)
{
	uint32_t bus = dev & SAMD51_MCLK_BUS_MASK;
	uint32_t dev_bit = dev & ~SAMD51_MCLK_BUS_MASK;
	
	if (bus == SAMD51_MCLK_AHB) {
		REG_MCLK_AHBMASK = (REG_MCLK_AHBMASK & ~dev_bit) | (enable ? dev_bit : 0);
	}
	else if (bus == SAMD51_MCLK_APBA) {
		REG_MCLK_APBAMASK = (REG_MCLK_APBAMASK & ~dev_bit) | (enable ? dev_bit : 0);
	}
	else if (bus == SAMD51_MCLK_APBB) {
		REG_MCLK_APBBMASK = (REG_MCLK_APBBMASK & ~dev_bit) | (enable ? dev_bit : 0);
	}
	else if (bus == SAMD51_MCLK_APBC) {
		REG_MCLK_APBCMASK = (REG_MCLK_APBCMASK & ~dev_bit) | (enable ? dev_bit : 0);
	}
	else if (bus == SAMD51_MCLK_APBD) {
		REG_MCLK_APBDMASK = (REG_MCLK_APBDMASK & ~dev_bit) | (enable ? dev_bit : 0);
	}
	else {
		return AI_ERROR_NODEV;
	}

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int samd51_oscillator_dpll_enable(int index, SAMD51_OSC_REF ref, uint32_t ref_clk_freq_hz, uint32_t output_freq_hz, SAMD51_OSC_OPT *opt)
{
	uint32_t dpll_control_b = 0;
	if (opt != NULL) {
		if (ref == SAMD51_OSC_REF_XOSC0 || ref == SAMD51_OSC_REF_XOSC1) {
			dpll_control_b |= ((opt->xosc_div & 0x7ff) << 16);
		}
	
		if (opt->filter_en) {
			dpll_control_b |= (1UL << 15) | (((uint32_t)opt->dco_filter) << 12)  | (opt->filter << 0);
		}
	}

	//J Lock‚ðÅ‘å1.1msec‘Ò‚Â
	dpll_control_b |= (0x0UL << 8);	

	//J Clockƒ\[ƒXÝ’è
	dpll_control_b |= ((uint32_t)ref) << 5;

	//J f_clk_dpll_n = f_ckr x (LDR + 1 + LDRFRAC/32);
	//J LDR = (f_clk_dpll_n / f_ckr - 1) - LDFRAC/32
	uint32_t ldr = (output_freq_hz / ref_clk_freq_hz) - 1;

	// LDRFRAC = (32 x f_clk_dpll_n / f_ckr - 32) - 32 x LDR)
	uint32_t frac = ((32 * output_freq_hz) / ref_clk_freq_hz) - 32*(ldr + 1);

	uint32_t dpll_ratio_control = ((frac & 0x1f) << 16) | (ldr & 0x1fff);
	
	if (index == 0) {
		OSC_REG.dpll0ctrlb = dpll_control_b;

		OSC_REG.dpll0ratio = dpll_ratio_control;
		while (OSC_REG.dpll0syncbusy != 0);
		
		OSC_REG.dpll0ctrla = (0UL << 7) | (1UL << 1);
		while (OSC_REG.dpll0syncbusy != 0);
	}
	else if (index == 1) {
		OSC_REG.dpll1ctrlb = dpll_control_b;

		OSC_REG.dpll1ratio = dpll_ratio_control;
		while (OSC_REG.dpll1syncbusy != 0);
		
		OSC_REG.dpll1ctrla = (0UL << 7) | (1UL << 1);
		while (OSC_REG.dpll1syncbusy != 0);
	}
	else {
		return AI_ERROR_NODEV;
	}

	return AI_OK;
}

