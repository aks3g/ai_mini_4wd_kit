/*
 * reset.c
 *
 * Created: 2021/08/05 5:19:18
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <stddef.h>


#include <sam.h>

#include <ai_mini4wd.h>
#include <ai_mini4wd_error.h>

#include <samd51_nvmctrl.h>
#include <samd51_error.h>
#include <samd51_gpio.h>
#include <samd51_sercom.h>
#include <samd51_uart.h>
#include <samd51_clock.h>
#include <samd51_timer.h>
#include <samd51_i2c.h>
#include <samd51_sdhc.h>
#include <samd51_adc.h>
#include <samd51_ac.h>
#include <samd51_dac.h>
#include <samd51_interrupt.h>
#include <samd51_usb_device.h>
#include <samd51_dmac.h>
#include <samd51_qspi.h>

void RESTART (volatile uint32_t *entry_addr, volatile uint32_t *stack_addr)
{
	volatile uint32_t stack_ptr = *stack_addr;
	volatile uint32_t start_adr = *entry_addr;

	__asm__ volatile ("MOV	r0, %[in]" : : [in] "r" (stack_ptr) : "r0");
	__asm__ volatile ("MOV	r1, %[in]" : : [in] "r" (start_adr) : "r1");
	__asm__ volatile ("MSR	MSP, r0");
	__asm__ volatile ("BX	r1");
}

void aiMini4wdReset(uint32_t reset_addr)
{
	if (reset_addr == 0) {
		NVIC_SystemReset();
	}
	
	__disable_irq();

	// Reset all pheripherals
	if (aiMini4wdusbEnabled()) {
		samd51_usb_finalize();
	}
	if (aiMini4wdDebugUartEnabled() || aiMini4wdLedIndicatorEnabled() || aiMini4wdOdometerEnabled()) {
		samd51_uart_finalize(SAMD51_SERCOM2);
	}

	samd51_ac_finalize();
	samd51_adc_finalize(0);
	samd51_adc_finalize(1);
	samd51_dac_finalize();
	samd51_external_interrupt_finalize();
	samd51_i2c_finalize(SAMD51_SERCOM3);
	samd51_sdhc_finalize(SAMD51_SDHC0);
	samd51_tc_finalize(SAMD51_TC0);
	samd51_tc_finalize(SAMD51_TC2);
	samd51_tc_finalize(SAMD51_TC4);
	samd51_dmac_finalize();
	samd51_qspi_finalize();

	NVIC->ICER[0] = 0xFFFFFFFFUL;
	NVIC->ICER[1] = 0xFFFFFFFFUL;
	NVIC->ICER[2] = 0xFFFFFFFFUL;
	NVIC->ICER[3] = 0xFFFFFFFFUL;
	NVIC->ICER[4] = 0xFFFFFFFFUL;
	NVIC->ICER[5] = 0xFFFFFFFFUL;
	NVIC->ICER[6] = 0xFFFFFFFFUL;
	NVIC->ICER[7] = 0xFFFFFFFFUL;

	NVIC->ICPR[0] = 0xFFFFFFFFUL;
	NVIC->ICPR[1] = 0xFFFFFFFFUL;
	NVIC->ICPR[2] = 0xFFFFFFFFUL;
	NVIC->ICPR[3] = 0xFFFFFFFFUL;
	NVIC->ICPR[4] = 0xFFFFFFFFUL;
	NVIC->ICPR[5] = 0xFFFFFFFFUL;
	NVIC->ICPR[6] = 0xFFFFFFFFUL;
	NVIC->ICPR[7] = 0xFFFFFFFFUL;

	//J Escape from Boot loader.
	SCB->VTOR = reset_addr;
	__DSB();
	
	__enable_irq();

	RESTART((uint32_t *)(reset_addr+4), (uint32_t *)reset_addr);

	return;
}