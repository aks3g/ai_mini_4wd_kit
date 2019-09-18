/*
 * samd51_bootloader.c
 *
 * Created: 2019/09/05 23:34:12
 * Author : kiyot
 */ 
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <sam.h>

#include <ai_mini4wd.h>
#include <ai_mini4wd_error.h>
#include <ai_mini4wd_hid.h>
#include <ai_mini4wd_timer.h>
#include <ai_mini4wd_sensor.h>
#include <ai_mini4wd_motor_driver.h>
#include <ai_mini4wd_fs.h>

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

#include "../libaimini4wd/include/internal/registry.h"


#define FLASH_HEAD					(0x1E000)

void RESTART (volatile uint32_t *entry_addr, volatile uint32_t *stack_addr)
{
	volatile uint32_t stack_ptr = *stack_addr;
	volatile uint32_t start_adr = *entry_addr;

	__asm__ volatile ("MOV	r0, %[in]" : : [in] "r" (stack_ptr) : "r0");
	__asm__ volatile ("MOV	r1, %[in]" : : [in] "r" (start_adr) : "r1");
	__asm__ volatile ("MSR	MSP, r0");
	__asm__ volatile ("BX	r1");
}

static int sTgl = 0;
static volatile uint32_t sProgress = 0;
void _timerCb(void)
{
	uint8_t toggle_led = 0;
	
	switch (sProgress) {
	case 0:
		aiMini4wdPrintLedPattern(0);
		break;
	case 1:
		aiMini4wdPrintLedPattern(0);
		toggle_led = (0x01);
		break;
	case 2:
		aiMini4wdPrintLedPattern(0x1);
		toggle_led = (0x02);
		break;
	case 3:
		aiMini4wdPrintLedPattern(0x3);
		toggle_led = (0x04);
		break;
	case 4:
		aiMini4wdPrintLedPattern(0x7);
		toggle_led = (0x08);
		break;
	}

	if (toggle_led != 0) {
		if (sTgl) {
			aiMini4wdSetLedPattern(toggle_led);
		}
		else {
			aiMini4wdClearLedPattern(toggle_led);
		}
		sTgl = 1 - sTgl;
	}
}


int main(void)
{
	uint8_t buf[512];

	int ret =aiMini4wdInitialize(0x80000000 | AI_MINI_4WD_INIT_FLAG_USE_DEBUG_PRINT);
	if (ret != AI_OK) {
		goto ERROR;
	}

	aiMini4WdTimerRegister100msCallback(_timerCb);

	AiMini4wdFile *hex_file;
	hex_file = aiMini4wdFsOpen("MINI4WD.AUP", "r");
	if (hex_file == NULL) {
		goto ENSURE;
	}

	UpdateFileHeader header;
	ret = aiMini4wdFsRead(hex_file, (void*)&header, sizeof(header));
	if (ret < sizeof(header)) {
		goto ERROR;
    }

	if (strncmp("4WD", header.signature, 3) != 0) {
		goto ERROR;
	}

	AiMini4wdRegistry *regstry = aiMini4wdRegistryGet();
	if ((regstry != NULL) && (0 == memcmp(regstry->sdk_data.field.fw.file_hash, header.file_hash, 16))){
		goto ENSURE;
	}

	aiMini4wdDebugPrintf("Valid update file is exits.\n");
	aiMini4wdDebugPrintf("Signature is OK\n");
	aiMini4wdDebugPrintf("Version: 0x%08x\n", header.version);
	aiMini4wdDebugPrintf("Hash: %c%c%c%c\n", header.hash[0], header.hash[1], header.hash[2], header.hash[3]);
	aiMini4wdDebugPrintf("Size: %d[byte]\n", header.size);

	//J 必要な領域を消す
	uint32_t pages = (header.size + 511) / 512;
	for (uint32_t i=0 ; i<pages ; ++i) {
		ret = samd51_nvmctrl_erase_page(FLASH_HEAD + (i*512), 1);
		if (ret != AI_OK) {
			goto ERROR;
		}

		sProgress = ((4 * i) / pages) + 1;
	}

	// 512Byte毎に読み込んでFlashに書き込む
	uint32_t addr = FLASH_HEAD;
	while(1) {
		memset (buf, 0xff, sizeof(buf));
		ret = aiMini4wdFsRead(hex_file, buf, sizeof(buf));
		if (ret <= 0) {
			break;
		}

		ret = samd51_nvmctrl_write_page(addr, buf, 1);
		if (ret != AI_OK) {
	  		goto ERROR;
		}
		addr += sizeof(buf);
	}

	//J 書き込んだファイルのFile HeaderをRegistryに書く	
	memcpy(&(regstry->sdk_data.field.fw), &header, sizeof(header));
	aiMini4wdRegistryUpdate();

ENSURE:

	__disable_irq();

	// Reset all pheripherals
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
	samd51_uart_finalize(SAMD51_SERCOM2);

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
	SCB->VTOR = FLASH_HEAD;
	__DSB();
  
	__enable_irq();

	RESTART((uint32_t *)(FLASH_HEAD+4), (uint32_t *)FLASH_HEAD);

	return 0;

ERROR:
	aiMini4wdDebugPrintf("ERROR ret = %d\n", ret);
	aiMini4wdSetErrorStatus(2);
	while(1);

}
