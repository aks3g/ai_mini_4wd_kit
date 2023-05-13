/*
 * registry.c
 *
 * Created: 2019/09/17 23:18:03
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

#include <samd51_error.h>
#include <samd51_nvmctrl.h>

#include <sdk_version.h>

#include "include/internal/registry.h"

#define REGISTORY_ADDR			(0xFE000)


static volatile int sInitialized = 0;
static AiMini4wdRegistry sRegs;


/*--------------------------------------------------------------------------*/
int aiMini4wdRegistryLoad(void)
{
	sInitialized = 1;

	samd51_nvmctrl_read(REGISTORY_ADDR, &sRegs, sizeof(sRegs));
	if (sRegs.sdk_data.field.sdk_version != AI_MINI_4WD_VERSION) {
		samd51_nvmctrl_erase_block(REGISTORY_ADDR, 1);
		
		memset ((void *)&sRegs, 0x00, sizeof(sRegs));		
		
		sRegs.sdk_data.field.sdk_version = AI_MINI_4WD_VERSION;
		sRegs.sdk_data.field.tachometer_threshold1 = 1200;
		sRegs.sdk_data.field.tachometer_threshold2 = 1200;
		
		return AI_ERROR_INVALID;
	}

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
AiMini4wdRegistry *aiMini4wdRegistryGet(void)
{
	if(sInitialized == 0) {
		return NULL;
	}
	
	return &sRegs;
}

/*--------------------------------------------------------------------------*/
int aiMini4wdRegistryUpdate(void)
{
	int ret = 0;
	if(sInitialized == 0) {
		return AI_ERROR_NOT_READY;
	}

	ret = samd51_nvmctrl_erase_block(REGISTORY_ADDR, 1);
	if (ret != AI_OK) {
		return ret;
	}

	volatile uint32_t wait = 1000000;
	while (wait--);

	ret = samd51_nvmctrl_write_page(REGISTORY_ADDR, &sRegs, 1);
	if (ret != AI_OK) {
		return ret;
	}

	return AI_OK;
}

