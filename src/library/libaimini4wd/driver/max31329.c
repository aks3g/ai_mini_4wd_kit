/*
 * max31329.c
 *
 * Created: 2023/03/21 5:23:42
 *  Author: kiyot
 */ 
#include <time.h>
#include <string.h>

#include <samd51_error.h>
#include <samd51_i2c.h>

#include "../include/ai_mini4wd.h"

#define MAX31329_ADR						(0xD0 >> 1)

#define MAX31329_REG_STATUS					(0x00)
#define MAX31329_REG_INT_EN					(0x01)
#define MAX31329_REG_RTC_RESET				(0x02)
#define MAX31329_REG_RTC_CONFIG				(0x03)
#define MAX31329_REG_RTC_CONFIG2			(0x04)
#define MAX31329_REG_RTC_TIMER_CONFIG		(0x05)
#define MAX31329_REG_RTC_SECONDS			(0x06)
#define MAX31329_REG_RTC_MINUTES			(0x07)
#define MAX31329_REG_RTC_HOURS				(0x08)
#define MAX31329_REG_RTC_DAY				(0x09)
#define MAX31329_REG_RTC_DATE				(0x0a)
#define MAX31329_REG_RTC_MONTH				(0x0b)
#define MAX31329_REG_RTC_YEAR				(0x0c)
#define MAX31329_REG_RTC_ALM1_SEC			(0x0d)
#define MAX31329_REG_RTC_ALM1_MIN			(0x0e)
#define MAX31329_REG_RTC_ALM1_HRS			(0x0f)
#define MAX31329_REG_RTC_ALM1_DAY_DATE		(0x10)
#define MAX31329_REG_RTC_ALM1_MON			(0x11)
#define MAX31329_REG_RTC_ALM1_YEAR			(0x12)
#define MAX31329_REG_RTC_ALM2_MIN			(0x13)
#define MAX31329_REG_RTC_ALM2_HRS			(0x14)
#define MAX31329_REG_RTC_ALM2_DAY_DATE		(0x15)
#define MAX31329_REG_RTC_TIMER_COUNT		(0x16)
#define MAX31329_REG_RTC_TIMER_INIT			(0x17)
#define MAX31329_REG_RTC_PWR_MGMT			(0x18)
#define MAX31329_REG_RTC_TRICKLE_REG		(0x19)
#define MAX31329_REG_RTC_RAM_HEAD			(0x22)
#define MAX31329_REG_RTC_RAM_TAIL			(0x61)


#define D_TRKCHG_EN_POS						(7)
#define D_TRICKLE_POS						(0)

#define D_TRICKLE_3k						(1)
#define D_TRICKLE_6k						(2)
#define D_TRICKLE_11k						(3)

typedef union Max31329_time_reg_t
{
	uint8_t bytes[7];
	struct  
	{
		uint8_t second   : 4;
		uint8_t second10 : 3;
		uint8_t pad06_7  : 1;
		uint8_t minute   : 4;
		uint8_t minute10 : 3;
		uint8_t pad07_7  : 1;
		uint8_t hour     : 4;
		uint8_t hour10   : 2;
		uint8_t f24_12   : 1;
		uint8_t day      : 3;
		uint8_t pad09_3_7: 5;
		uint8_t date     : 4;
		uint8_t date10   : 2;
		uint8_t pad0a_6_7: 2;
		uint8_t month    : 4;
		uint8_t month10  : 1;
		uint8_t pad0b_5_6: 2;
		uint8_t century  : 1;
		uint8_t year     : 4;
		uint8_t year10   : 4;
	} reg;
} Max31329_time_reg;


typedef union Max31329_timer_set_buf_t
{
	uint8_t bytes[8];
	struct {
		uint8_t addr;
		Max31329_time_reg timer_reg;
	} bf;
} Max31329_timer_set_buf_t;

static Max31329_timer_set_buf_t sTimeReg;
static SAMD51_SERCOM sSercom;

int max31329_probe(SAMD51_SERCOM sercom, uint32_t *epoc_time)
{
	uint8_t txbuf[2];
	uint8_t rxbuf[7];

	txbuf[0] = MAX31329_REG_STATUS;
	int ret = samd51_i2c_txrx(sercom, MAX31329_ADR, txbuf, 1, rxbuf, 1, NULL);
	if (ret != 0) {
		return ret;
	}

	// Select 11k ohome
	txbuf[0] = MAX31329_REG_RTC_TRICKLE_REG;
	txbuf[1] = (1<<D_TRKCHG_EN_POS) | (D_TRICKLE_11k << D_TRICKLE_POS);
	ret = samd51_i2c_txrx(sercom, MAX31329_ADR, txbuf, 2, NULL, 0, NULL);
	if (ret != 0) {
		return ret;
	}

	sSercom = sercom;

	sTimeReg.bf.addr = MAX31329_REG_RTC_SECONDS;
	ret = samd51_i2c_txrx(sSercom, MAX31329_ADR, sTimeReg.bytes, 1, sTimeReg.bf.timer_reg.bytes, sizeof(Max31329_time_reg), NULL);
	if (ret != 0) {
		return -1;
	}
	
	struct tm rtc_time;
	memset(&rtc_time, 0, sizeof(rtc_time));
	rtc_time.tm_hour = sTimeReg.bf.timer_reg.reg.hour   + sTimeReg.bf.timer_reg.reg.hour10 * 10;
	rtc_time.tm_min  = sTimeReg.bf.timer_reg.reg.minute + sTimeReg.bf.timer_reg.reg.minute10 * 10;
	rtc_time.tm_sec  = sTimeReg.bf.timer_reg.reg.second + sTimeReg.bf.timer_reg.reg.second10 * 10;
	rtc_time.tm_year = 2000 + (sTimeReg.bf.timer_reg.reg.year + sTimeReg.bf.timer_reg.reg.year10*10) - 1900;
	rtc_time.tm_mon  = sTimeReg.bf.timer_reg.reg.month + sTimeReg.bf.timer_reg.reg.month10 * 10 - 1;
	rtc_time.tm_mday = sTimeReg.bf.timer_reg.reg.date + sTimeReg.bf.timer_reg.reg.date10 * 10;

	*epoc_time = mktime(&rtc_time);

	return ret;
}

uint32_t max31329_get_time(void)
{
	return 0;
}

int max31329_set_time(struct tm *t)
{
	memset(&sTimeReg, 0, sizeof(sTimeReg));

	sTimeReg.bf.timer_reg.reg.second   = t->tm_sec % 10;
	sTimeReg.bf.timer_reg.reg.second10 = t->tm_sec / 10;

	sTimeReg.bf.timer_reg.reg.minute   = t->tm_min % 10;
	sTimeReg.bf.timer_reg.reg.minute10 = t->tm_min / 10;

	sTimeReg.bf.timer_reg.reg.hour   = t->tm_hour % 10;
	sTimeReg.bf.timer_reg.reg.hour10 = t->tm_hour / 10;

	sTimeReg.bf.timer_reg.reg.day    = t->tm_wday;
	
	sTimeReg.bf.timer_reg.reg.date   = t->tm_mday % 10;
	sTimeReg.bf.timer_reg.reg.date10 = t->tm_mday / 10;

	sTimeReg.bf.timer_reg.reg.month   = (t->tm_mon+1) % 10;
	sTimeReg.bf.timer_reg.reg.month10 = (t->tm_mon+1) / 10;

	sTimeReg.bf.timer_reg.reg.year    = t->tm_year % 10;
	sTimeReg.bf.timer_reg.reg.year10  = (t->tm_year / 10) % 10;

	sTimeReg.bf.addr = MAX31329_REG_RTC_SECONDS;
	int ret = samd51_i2c_txrx(sSercom, MAX31329_ADR, sTimeReg.bytes, sizeof(sTimeReg), NULL, 0, NULL);

	return ret;
}
