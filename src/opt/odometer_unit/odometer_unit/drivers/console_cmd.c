/*
 * console_cmd.c
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <spi_driver.h>
#include <gpio_driver.h>

#include "drivers/console_cmd.h"
#include "drivers/adns9800.h"
#include "odometer_reg.h"


static void _console_cmd_help(char **args, int argn);
static void _console_cmd_echo(char **args, int argn);
static void _console_cmd_dump(char **args, int argn);
static void _console_cmd_reset(char **args, int argn);
static void _console_cmd_srom(char **args, int argn);
static void _console_cmd_read(char **args, int argn);
static void _console_cmd_write(char **args, int argn);
static void _console_cmd_reg_read(char **args, int argn);
static void _console_cmd_reg_write(char **args, int argn);

console_cmd_def console_cmd_set[NUM_CMDS]
=
{
	{"help",		"Show help",						_console_cmd_help},
	{"echo",		"Echo line",						_console_cmd_echo},
	{"dump",		"Dump internal registers",			_console_cmd_dump},
	{"reset",		"Reset mouse module",				_console_cmd_reset},
	{"srom",		"Show/Set SROM version",			_console_cmd_srom},
	{"read",		"Read from mouse module",			_console_cmd_read},
	{"write",		"Write from mouse module",			_console_cmd_write},
	{"regr",		"Read internal regs",				_console_cmd_reg_read},
	{"regw",		"Write internal regs",				_console_cmd_reg_write},
	{NULL,			NULL,								NULL},
};

static void _console_cmd_help(char **args, int argn)
{
	printf ("Supported command:\n");
	for (int i=0 ; i<NUM_CMDS ; ++i) {
		if (console_cmd_set[i].name == NULL) {
			break;
		}
		printf ("  %s : %s\n", console_cmd_set[i].name, console_cmd_set[i].help);
	}

	return;
}


static void _console_cmd_echo(char **args, int argn)
{
	int i=0;
	for (i=0 ; i<argn ; ++i) {
		printf ("%s ", args[i]);
	}
	putchar('\n');

	return;
}

static void _console_cmd_dump(char **args, int argn)
{
	printf ("Dump internal register.\n");
	printf ("   | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	printf ("---+------------------------------------------------\n");
	for (int i=0 ; i<REG_SIZE ; ++i) {
		if (i%16 == 0) {
			printf ("%02x |", (i/16) << 4);
		}

		uint8_t value = 0;
		reg_read(i, &value);
		printf (" %02x", value);

		if (i%16 == 15) {
			printf ("\n");
		}
	}

	return;
}

static void _console_cmd_reset(char **args, int argn)
{
	printf ("Reset mouse module.\n");
	reg_write(REG_RESET, 0x01);

	return;
}

static void _console_cmd_srom(char **args, int argn)
{
	if (argn != 0) {
		uint8_t srom_version = strtol(args[0], NULL, 16);
		if (srom_version == 0xA4 || srom_version == 0xA5 || srom_version == 0xA6) {
			printf ("Select SROM version : %02x\n", srom_version);		
			reg_write(REG_SROM_VERSION, srom_version);
		}
		else {
			printf ("Invalid SROM version : %s\n", args[0]);
		}
	}
	else {
		uint8_t srom_version = 0;
		reg_read(REG_SROM_VERSION, &srom_version);
		printf ("Current SROM version : %02x\n", srom_version);
	}
	return;
}


static void _console_cmd_read(char **args, int argn)
{
	if (argn == 0) {
		printf ("Argument Error : Select register offset.\n");
		return;
	}
	
	uint8_t offset = strtol(args[0], NULL, 16);
	uint8_t value = 0;
	adns9800_read(offset, &value);

	printf ("Read adns-9800 reg[%02x] = 0x%02x\n", offset, value);

	return;
}

static void _console_cmd_write(char **args, int argn)
{
	if (argn != 2) {
		printf ("Argument Error : Select register offset and value\n");
		return;
	}
	
	uint8_t offset = strtol(args[0], NULL, 16);
	uint8_t value  = strtol(args[1], NULL, 16);
	
	adns9800_write(offset, value);

	printf ("Write adns-9800 reg[%02x] = 0x%02x\n", offset, value);	
	return;
}

static void _console_cmd_reg_read(char **args, int argn)
{
	if (argn == 0) {
		printf ("Argument Error : Select register offset.\n");
		return;
	}
	
	uint8_t offset = strtol(args[0], NULL, 16);
	uint8_t value = 0;
	reg_read(offset, &value);

	printf ("Read reg[%02x] = 0x%02x\n", offset, value);
	
	return;
}


static void _console_cmd_reg_write(char **args, int argn)
{
	if (argn != 2) {
		printf ("Argument Error : Select register offset and value\n");
		return;
	}

	uint8_t offset = strtol(args[0], NULL, 16);
	uint8_t value  = strtol(args[1], NULL, 16);

	reg_write(offset, value);

	printf ("Write reg[%02x] = 0x%02x\n", offset, value);

	return;
}
