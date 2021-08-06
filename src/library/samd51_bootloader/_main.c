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
#include <samd51_usb_device.h>

#include "../libaimini4wd/include/internal/registry.h"
#include "../libaimini4wd/include/internal/usb_mass_storage.h"

#include "include/console.h"

#define FLASH_HEAD					(0x1E000)



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

static volatile int sUsbDetatched = 0;
static void _vbus_changed_cb(int vbus)
{	
	if (vbus == 0) {
		sUsbDetatched = 1;
	}
	
	return;
}

int main(void)
{
	uint8_t buf[512];

	int ret =aiMini4wdInitialize(0x80000000 | AI_MINI_4WD_INIT_FLAG_USE_DEBUG_PRINT | AI_MINI_4WD_INIT_FLAG_USE_USB_SERIAL);
	if (ret != AI_OK) {
		goto ERROR;
	}

	//J USBが接続されている場合Conosleモードに移行する
	if (samd51_gpio_input(SAMD51_GPIO_A23) != 0) {
		aiMini4wdRegisterOnVbusChangedCb(_vbus_changed_cb);
		aiMini4wdFsMountDrive(0);

		while (1) {
			usbMassStorageUpdate();

			// VBUSが抜けたらResetする
			if (sUsbDetatched != 0) {
				break;
			}

			int rx = aiMini4wdDebugTryGetc();
			if (rx <= 0) continue;
			
			char c = (char)(rx & 0xff);
			console_update(c);
		}
		
		aiMini4wdFsMountDrive(1);
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
	aiMini4wdSetLedPattern(0x5);
	aiMini4wdReset(FLASH_HEAD); // Never return.

ERROR:
	aiMini4wdDebugPrintf("ERROR ret = %d\n", ret);
	aiMini4wdSetErrorStatus(2);
	while(1);

}
