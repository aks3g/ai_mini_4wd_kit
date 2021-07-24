/*
 * adc.c
 *
 * Created: 2021/07/05 6:12:05
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <console.h>
#include "drivers/ads131m08.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static int _adcr_function(const int argc, const char **argv)
{
	if (argc != 1) {
		consolePuts("Argument Error.\n");
	}
	
	uint8_t addr = (uint8_t)strtoul(argv[0], NULL, 16);
	uint32_t val = 0;	

	ads131m08_reg_read(addr, &val);

	consolePrintf("ADS138M08: Read Reg[0x%02x] = 0x%04x\n", addr, (uint16_t)val);
	
	return 0;
}

static const char *_adcr_help(void)
{
	return "adcr - Read register of ADS131M08.\n"
	"  adcr <reg addres>\n"
	"   <reg addres> : Register Address. 0x00 - 0x3f";
}

ConsoleCommand g_adcr_cmd =
{
	.name = "adcr",
	.func = _adcr_function,
	.help = _adcr_help,
	.link = NULL
};


/*--------------------------------------------------------------------------*/
static int _adcw_function(const int argc, const char **argv)
{
	if (argc != 2) {
		consolePuts("Argument Error.\n");
	}
	
	uint8_t addr = (uint8_t) strtoul(argv[0], NULL, 16);
	uint32_t val = (uint32_t)strtoul(argv[1], NULL, 16);

	ads131m08_reg_write(addr, val);

	consolePrintf("ADS138M08: Write Reg[0x%02x] = 0x%04x\n", addr, (uint16_t)val);

	return 0;
}

static const char *_adcw_help(void)
{
	return "adcw - Read register of ADS131M08.\n"
	"  adcw <reg addres>\n"
	"   <reg addres> : Register Address. 0x00 - 0x3f";
}

ConsoleCommand g_adcw_cmd =
{
	.name = "adcw",
	.func = _adcw_function,
	.help = _adcw_help,
	.link = NULL
};


/*--------------------------------------------------------------------------*/
static int _adccap_function(const int argc, const char **argv)
{	
	int32_t val[9];
	ads131m08_read((uint32_t *)val, sizeof(val)/sizeof(val[0]));

	consolePrintf("ADS138M08: Status = 0x%08x\n", val[0]);
	consolePrintf("ADS138M08: CH0 = %d[uV]\n", (int)val[1]);
	consolePrintf("ADS138M08: CH1 = %d[uV]\n", (int)val[2]);
	consolePrintf("ADS138M08: CH2 = %d[uV]\n", (int)val[3]);
	consolePrintf("ADS138M08: CH3 = %d[uV]\n", (int)val[4]);
	consolePrintf("ADS138M08: CH4 = %d[uV]\n", (int)val[5]);
	consolePrintf("ADS138M08: CH5 = %d[uV]\n", (int)val[6]);
	consolePrintf("ADS138M08: CH6 = %d[uV]\n", (int)val[7]);
	consolePrintf("ADS138M08: CH7 = %d[uV]\n", (int)val[8]);

	return 0;
}

static const char *_adccap_help(void)
{
	return "adccap - Capture ADC result from ADS131M08.";
}

ConsoleCommand g_adccap_cmd =
{
	.name = "adccap",
	.func = _adccap_function,
	.help = _adccap_help,
	.link = NULL
};
