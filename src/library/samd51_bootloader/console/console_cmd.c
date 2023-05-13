/*
 * console_cmd.c
 *
 * Created: 2020/06/21 5:27:13
 *  Author: kiyot
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <samd51_gpio.h>

#include <ai_mini4wd.h>
#include <ai_mini4wd_fs.h>
#include <ai_mini4wd_hid.h>
#include <ai_mini4wd_timer.h>
#include <ai_mini4wd_trace.h>


#include "console_cmd.h"
#include "../libaimini4wd/include/internal/registry.h"


static void _console_cmd_help(char **args, int argn);
static void _console_cmd_echo(char **args, int argn);
static void _console_cmd_ls(char **args, int argn);
static void _console_cmd_cat(char **args, int argn);
static void _console_cmd_w32(char **args, int argn);
static void _console_cmd_w8(char **args, int argn);
static void _console_cmd_r32(char **args, int argn);
static void _console_cmd_r8(char **args, int argn);
static void _console_cmd_led(char **args, int argn);
static void _console_cmd_ir(char **args, int argn);
static void _console_cmd_mnt(char **args, int argn);
static void _console_cmd_umnt(char **args, int argn);
static void _console_cmd_dump_registry(char **args, int argn);
static void _console_cmd_fmt(char **args, int argn);
static void _console_cmd_sensor(char **args, int argn);
static void _console_cmd_rtc(char **args, int argn);
static void _console_cmd_fs_test(char **args, int argn);
static void _console_cmd_trace(char **args, int argn);

console_cmd_def console_cmd_set[NUM_CMDS]
=
{
	{"help",		"Show help",						_console_cmd_help},
	{"echo",		"Echo line",						_console_cmd_echo},
	{"ls",			"List files",						_console_cmd_ls},
	{"cat",			"Concatenate FILE",					_console_cmd_cat},
	{"w32",			"Write 32bit value",				_console_cmd_w32},
	{"w8",			"Write 8bit value",					_console_cmd_w8},
	{"r32",			"Read 32bit value",					_console_cmd_r32},
	{"r8",			"Read 8bit value",					_console_cmd_r8},
	{"led",			"Control Led",						_console_cmd_led},
	{"ir",			"Check IR state",					_console_cmd_ir},
	{"reg",			"Dump AI mini4wd Registry",			_console_cmd_dump_registry},
	{"mnt",			"Mount FS (Unmount from UMSS)",		_console_cmd_mnt},
	{"umnt",		"Unmount FS (Mount to UMSS)",		_console_cmd_umnt},
	{"fmt",			"Format File system",				_console_cmd_fmt},
	{"sensor",		"show sensor values",				_console_cmd_sensor},
	{"rtc",			"get/set RTC value",				_console_cmd_rtc},
	{"fs",			"File system test",					_console_cmd_fs_test},
	{"trace",		"trace control",					_console_cmd_trace},
	{NULL,			NULL,								NULL},
};

static void _console_cmd_help(char **args, int argn)
{
	aiMini4wdDebugPrintf ("Supported command:\n");
	for (int i=0 ; i<NUM_CMDS ; ++i) {
		if (console_cmd_set[i].name == NULL) {
			break;
		}
		aiMini4wdDebugPrintf ("  %10s : %s\n", console_cmd_set[i].name, console_cmd_set[i].help);
	}

	return;
}


static void _console_cmd_echo(char **args, int argn)
{
	int i=0;
	for (i=0 ; i<argn ; ++i) {
		aiMini4wdDebugPrintf ("%s ", args[i]);
	}
	aiMini4wdDebugPutc('\n');

	return;
}

static void _console_cmd_ls(char **args, int argn)
{
	char path[128] = "";
	if (argn != 0) {
		strncpy(path, args[0], sizeof(path)-1);
	}

	AiMini4wdDir *dirobj = aiMini4wdFsOpenDir(path);
	AiMini4wdFileInfo info;
	int ret = 0;

	aiMini4wdDebugPrintf("%c %10s %s\n" , ' ', "size", "name");
	while(1) {
		ret = aiMini4wdFsReadDir(dirobj, &info);
		if (ret != 0 || info.name[0] == 0)	break;

		aiMini4wdDebugPrintf("%c %10d %s\n" ,(info.attr & AI_MINI4WD_FS_DIR) ? 'd' : ' ', info.size, info.name);
	}
	aiMini4wdFsCloseDir(dirobj);
	
	return;
}


static void _console_cmd_cat(char **args, int argn)
{
	char *path = NULL;
	if (argn == 0) {
		aiMini4wdDebugPrintf("Select File.\n");
		return;
	}

	path = args[0];

	AiMini4wdFile *fd = aiMini4wdFsOpen(path, "r");
	if (fd == NULL) {
		aiMini4wdDebugPrintf("File not found.\n");
		return;
	}
	char buf[256];
	while(1) {
		char *line = aiMini4wdFsGets(fd, buf, sizeof(buf));
		if (line == NULL) break;
		
		aiMini4wdDebugPuts(buf, strlen(buf));
	}
	aiMini4wdFsClose(fd);
	aiMini4wdDebugPrintf("\n");

	return;
}

static void _console_cmd_w32(char **args, int argn)
{
	if (argn != 2) {
		aiMini4wdDebugPrintf("Usage : w32 [address] [data]");
		return;
	}

	uint32_t *addr = (uint32_t *)strtoul(args[0], NULL, 0);
	uint32_t  data = (uint32_t  )strtoul(args[1], NULL, 0);

	aiMini4wdDebugPrintf("Write 0x%08x to 0x%08x\n", data, addr);

	*addr = data;

	return;
}


static void _console_cmd_w8(char **args, int argn)
{
	if (argn != 2) {
		aiMini4wdDebugPrintf("Usage : w8 [address] [data]");
		return;
	}

	uint8_t *addr = (uint8_t *)strtoul(args[0], NULL, 0);
	uint8_t  data = (uint8_t  )strtoul(args[1], NULL, 0);

	aiMini4wdDebugPrintf("Write 0x%02x to 0x%08x\n", data, addr);

	*addr = data;

	return;
}


static void _console_cmd_r32(char **args, int argn)
{
	if (argn == 0) {
		aiMini4wdDebugPrintf("Usage : r32 [address] [length(opt)]");
		return;
	}
	
	uint32_t *addr = (uint32_t *)strtoul(args[0], NULL, 0);

	if ((uint32_t)addr & 0x03) {
		aiMini4wdDebugPrintf("Invalid address 0x%08x (Not aligned 4-byte)\n", (uint32_t)addr);
		return;
	}

	size_t len = 1;
	if (argn >= 2) {
		len = (size_t)strtoul(args[1], NULL, 0);
	}

	aiMini4wdDebugPrintf("        | 00000000 00000004 00000008 0000000c\n");
	aiMini4wdDebugPrintf("--------+------------------------------------\n");

	for (size_t i=0 ; i<len ; ++i) {
		if (i == 0) {
			aiMini4wdDebugPrintf("%08x| ", ((uint32_t)addr & ~0xF));
			size_t n = (uint32_t)(addr) & 0xf;
			while (n--) {
				aiMini4wdDebugPrintf("   ");
			}
		}
		else if ((((uint32_t)addr + (i*4)) & 0xf) == 0) {
			aiMini4wdDebugPrintf("\n%08x| ", (((uint32_t)addr + (i*4)) & ~0xF));
		}

		uint32_t val = *(addr + i);
		aiMini4wdDebugPrintf("%08X ", val);
	}
	aiMini4wdDebugPrintf("\n");
}


static void _console_cmd_r8(char **args, int argn)
{
	if (argn == 0) {
		aiMini4wdDebugPrintf("Usage : r8 [address] [length(opt)]");
		return;
	}

	uint8_t *addr = (uint8_t *)strtoul(args[0], NULL, 0);

	size_t len = 1;
	if (argn >= 2) {
		len = (size_t)strtoul(args[1], NULL, 0);
	}

	aiMini4wdDebugPrintf("        | 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");
	aiMini4wdDebugPrintf("--------+------------------------------------------------\n");

	for (size_t i=0 ; i<len ; ++i) {
		if (i == 0) {
			aiMini4wdDebugPrintf("%08x| ", ((uint32_t)addr & ~0xF));
			size_t n = (uint32_t)(addr) & 0xf;
			while (n--) {
				aiMini4wdDebugPrintf("   ");
			}
		}
		else if ((((uint32_t)addr + i) & 0xf) == 0) {
			aiMini4wdDebugPrintf("\n%08x| ", (((uint32_t)addr + i) & ~0xF));
		}

		uint8_t val = *(addr + i);
		aiMini4wdDebugPrintf("%02X ", val);
	}
	aiMini4wdDebugPrintf("\n");

	return;
}

static void _console_cmd_led(char **args, int argn)
{
	if (argn != 2) {
		aiMini4wdDebugPrintf("Usage : led [index=0|1|2|3] [on|off]\n");
		return;
	}

	uint32_t pos = strtoul(args[0], NULL, 0);
	if (pos > 3) {
		aiMini4wdDebugPrintf("Usage : led [index=0|1|2|3] [on|off]\n");
		return;
	}

	if (strncmp(args[1], "on" , 2) == 0) {
		aiMini4wdSetLedPattern((1<<pos));
	}
	else if (strncmp(args[1], "off" , 3) == 0) {
		aiMini4wdClearLedPattern((1<<pos));
	} 
	else {
		aiMini4wdDebugPrintf("Usage : led [index=0|1|2|3] [on|off]\n");
		return;
	}

	return;
}

static void _checkExtTrig(void){
	aiMini4wdDebugPrintf("Ext trig.\n");	
}

static void _nop(void){
}


static void _console_cmd_ir(char **args, int argn)
{
	if (strncmp(args[0], "on" , 2) == 0) {
		aiMini4wdDebugPrintf("Set ext trig callback.\n");
		aiMini4wdRegisterExtInterrupt(_checkExtTrig);
	}
	else if (strncmp(args[0], "off" , 3) == 0) {
		aiMini4wdDebugPrintf("Remove ext trig callback.\n");
		aiMini4wdRegisterExtInterrupt(_nop);
	}
	else {
		aiMini4wdDebugPrintf("Usage : ir [on|off]\n");
		return;
	}

	return;
}

static void _console_cmd_dump_registry(char **args, int argn)
{
	AiMini4wdRegistry *regstry = aiMini4wdRegistryGet();
	if (argn == 0) {
		aiMini4wdDebugPrintf("SDK Version = 0x%08x\n", regstry->sdk_data.field.sdk_version);
		aiMini4wdDebugPrintf("  FW signature = %c%c%c%c\n",
		regstry->sdk_data.field.fw.signature[0],
		regstry->sdk_data.field.fw.signature[1],
		regstry->sdk_data.field.fw.signature[2],
		regstry->sdk_data.field.fw.signature[3]);

		aiMini4wdDebugPrintf("  FW version   = 0x%08x\n", regstry->sdk_data.field.fw.version);
		aiMini4wdDebugPrintf("  FW size      = %d\n", regstry->sdk_data.field.fw.size);
		aiMini4wdDebugPrintf("  FW signature = %02x %02x %02x %02x\n",
		regstry->sdk_data.field.fw.hash[0],
		regstry->sdk_data.field.fw.hash[1],
		regstry->sdk_data.field.fw.hash[2],
		regstry->sdk_data.field.fw.hash[3]);

		aiMini4wdDebugPrintf("  FW file hash = %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
		regstry->sdk_data.field.fw.file_hash[0],
		regstry->sdk_data.field.fw.file_hash[1],
		regstry->sdk_data.field.fw.file_hash[2],
		regstry->sdk_data.field.fw.file_hash[3],
		regstry->sdk_data.field.fw.file_hash[4],
		regstry->sdk_data.field.fw.file_hash[5],
		regstry->sdk_data.field.fw.file_hash[6],
		regstry->sdk_data.field.fw.file_hash[3],
		regstry->sdk_data.field.fw.file_hash[8],
		regstry->sdk_data.field.fw.file_hash[9],
		regstry->sdk_data.field.fw.file_hash[10],
		regstry->sdk_data.field.fw.file_hash[11],
		regstry->sdk_data.field.fw.file_hash[12],
		regstry->sdk_data.field.fw.file_hash[13],
		regstry->sdk_data.field.fw.file_hash[14],
		regstry->sdk_data.field.fw.file_hash[15]);

		aiMini4wdDebugPrintf("  tachometer threshold1 = %u[mv]\n", regstry->sdk_data.field.tachometer_threshold1);
		aiMini4wdDebugPrintf("  tachometer threshold2 = %u[mv]\n", regstry->sdk_data.field.tachometer_threshold2);
	}
	else if (strncmp(args[0], "-b", 2) == 0) {
		aiMini4wdDebugPrintf(" SDK area\n");
		aiMini4wdDebugPrintf("  | 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");
		aiMini4wdDebugPrintf("--+------------------------------------------------\n");

		uint8_t* addr = regstry->sdk_data.bytes;

		for (size_t i=0 ; i<256 ; ++i) {
			if ((i & 0xf) == 0) {
				aiMini4wdDebugPrintf("%02x| ", (((uint32_t)i) & ~0xF));
			}

			uint8_t val = *(addr + i);
			aiMini4wdDebugPrintf("%02X ", val);

			if ((i & 0xf) == 0xf) {
				aiMini4wdDebugPrintf("\n");
			}

		}
		aiMini4wdDebugPrintf("\n");

		aiMini4wdDebugPrintf(" User area\n");
		aiMini4wdDebugPrintf("  | 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");
		aiMini4wdDebugPrintf("--+------------------------------------------------\n");

		addr = regstry->user_data;

		for (size_t i=0 ; i<256 ; ++i) {
			if ((i & 0xf) == 0) {
				aiMini4wdDebugPrintf("%02x| ", (((uint32_t)i) & ~0xF));
			}

			uint8_t val = *(addr + i);
			aiMini4wdDebugPrintf("%02X ", val);

			if ((i & 0xf) == 0xf) {
				aiMini4wdDebugPrintf("\n");
			}
		}
		aiMini4wdDebugPrintf("\n");
	}
	


}

static void _console_cmd_mnt(char **args, int argn)
{
	int ret = 0;
	ret = aiMini4wdFsMountDrive(1);
	
	if (ret == 0) {
		aiMini4wdDebugPrintf("Mount SDHC.\n");
	}
}

static void _console_cmd_umnt(char **args, int argn)
{
	int ret = 0;
	ret = aiMini4wdFsMountDrive(0);

	if (ret == 0) {
		aiMini4wdDebugPrintf("Unmount SDHC.\n");
	}
}


static void _console_cmd_fmt(char **args, int argn)
{
	aiMini4wdDebugPrintf("Format File System ? (y|n)\n");
	int c = aiMini4wdDebugGetc();
	if (c == (int)'y') {
		aiMini4wdDebugPrintf("Start format. ");
		aiMini4wdDiskFormat();
		aiMini4wdDebugPrintf(" done\n");
	}
	else {
		aiMini4wdDebugPrintf("skip format\n");
	}
}

static void _console_cmd_sensor(char **args, int argn)
{
}

static void _console_cmd_rtc(char **args, int argn)
{
	if (argn == 0) {
		uint32_t rtc_val = aiMini4wdRtcGetTimer();
		struct tm *ptn;
		ptn = localtime((time_t *)&rtc_val);
		
		aiMini4wdDebugPrintf("%04d/%02d/%02d %02d:%02d:%02d\n", 1900 + ptn->tm_year, ptn->tm_mon + 1, ptn->tm_mday, ptn->tm_hour, ptn->tm_min, ptn->tm_sec);	
	}
	else if (argn == 2){
		struct tm new_time;
		int year, mon, day, hour, min, sec;
		sscanf(args[0], "%04d/%02d/%02d", &year, &mon, &day);
		sscanf(args[1], "%02d:%02d:%02d", &hour, &min, &sec);

		new_time.tm_hour = hour;
		new_time.tm_min  = min;
		new_time.tm_sec  = sec;
		new_time.tm_mday = day;
		new_time.tm_mon  = mon - 1;
		new_time.tm_year = year - 1900;
		
		volatile time_t epoc = mktime(&new_time);

		aiMini4wdDebugPrintf("epoc = %u\n", (uint32_t)epoc);
		aiMini4wdRtcSetTimer(epoc);
	}
}

static void _console_cmd_fs_test(char **args, int argn) {
	AiMini4wdFile *fp;
	
	fp=aiMini4wdFsOpen("test.txt", "w");
	
	volatile int ret = aiMini4wdFsSeek(fp, 1*1024*1024);
	if (ret != 0) {
		aiMini4wdDebugPrintf("Error at aiMini4wdFsSeek() = %08x\n", (uint32_t)ret);
	}

	ret = aiMini4wdFsSeek(fp, 0);
	if (ret != 0) {
		aiMini4wdDebugPrintf("Error at aiMini4wdFsSeek() = %08x\n", (uint32_t)ret);
	}

	ret = aiMini4wdFsSync(fp);
	if (ret != 0) {
		aiMini4wdDebugPrintf("Error at aiMini4wdFsSync() = %08x\n", (uint32_t)ret);
	}

	ret = aiMini4wdFsWrite(fp, "test message\n", strlen("test message\n"));
	if (ret != 0) {
		aiMini4wdDebugPrintf("Error at aiMini4wdFsWrite() = %08x\n", (uint32_t)ret);
	}

	for (int i=0; i<1000 ; ++i) {
		ret = aiMini4wdFsPuts(fp, "Test\n", strlen( "Test\n"));
	}

	ret = aiMini4wdFsPutsFlush(fp);
	if (ret != 0) {
		aiMini4wdDebugPrintf("Error at aiMini4wdFsWrite() = %08x\n", (uint32_t)ret);
	}	ret = aiMini4wdFsTruncate(fp);


	aiMini4wdFsClose(fp);
}

static sTraceIdx=0;
static void log_reader(uint32_t idx, uint32_t tick, uint32_t log)
{
	aiMini4wdDebugPrintf("%4d\t%08x\t%08x\n", idx, tick, log);
}

static void _console_cmd_trace(char **args, int argn) {
	if (argn == 0) {
		
	}
	else if (strcmp(args[0], "enable") == 0) {
		aiMini4wdDebugPrintf("Enable Debug trace\n");
		aiMini4wdDebugTraceControl(1);
	}
	else if (strcmp(args[0], "disable") == 0) {
		aiMini4wdDebugPrintf("Disable Debug trace\n");
		aiMini4wdDebugTraceControl(0);
	}
	else if (strcmp(args[0], "clear") == 0) {
		aiMini4wdDebugPrintf("Clear Debug trace\n");
		aiMini4wdDebugTraceClear();
	}
	else if (strcmp(args[0], "push") == 0) {
		aiMini4wdDebugTracePush(aiMini4WdTimerGetSystemtick(), 0xdeadbeef);
	}
	else if (strcmp(args[0], "pop") == 0) {
		aiMini4wdDebugPrintf("--Start of Debug Trace--\n");
		sTraceIdx = 0;
		aiMini4wdDebugTracePop(log_reader);
		aiMini4wdDebugPrintf("--End of Debug Trace--\n");
	}
}
