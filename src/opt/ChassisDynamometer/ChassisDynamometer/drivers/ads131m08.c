/*
 * ads131m08.c
 *
 * Created: 2021/07/03 12:57:58
 *  Author: kiyot
 */ 
#include <stddef.h>
#include <stdint.h>
#include <fastmath.h>
#include <string.h>

#include <samd51_error.h>
#include <samd51_gpio.h>
#include <samd51_timer.h>
#include <samd51_spi.h>
#include <samd51_interrupt.h>

#include "drivers/ads131m08.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#define CMD_NULL					0x0000
#define CMD_RESET					0x0011
#define CMD_STANDBY					0x0022
#define CMD_WAKEUP					0x0033
#define CMD_LOCK					0x0555
#define CMD_UNLOCK					0x0655
#define CMD_RREG(addr, num)			(0xA000 | ((addr&0x3f) << 7) | (num & 0x7f))
#define CMD_WREG(addr, num)			(0x6000 | ((addr&0x3f) << 7) | (num & 0x7f))
#define RES_WREG(addr, num)			(0x4000 | ((addr&0x3f) << 7) | (num & 0x7f))


/*--------------------------------------------------------------------------*/
#define REG_IDX_ID					0x00
#define REG_IDX_STATUS				0x01

#define REG_IDX_MODE				0x02
#define REG_IDX_CLOCK				0x03
#define REG_IDX_GAIN1				0x04
#define REG_IDX_GAIN2				0x05
#define REG_IDX_CFG					0x06
#define REG_IDX_THRSHLD_MSB			0x07
#define REG_IDX_THRSHLD_LSB			0x08

#define REG_IDX_CHn_CFG(n)			(((n)*5) + 0 + 0x09)
#define REG_IDX_CHn_OCAL_MSB(n)		(((n)*5) + 1 + 0x09)
#define REG_IDX_CHn_OCAL_LSB(n)		(((n)*5) + 2 + 0x09)
#define REG_IDX_CHn_GCAL_MSB(n)		(((n)*5) + 3 + 0x09)
#define REG_IDX_CHn_GCAL_LSB(n)		(((n)*5) + 4 + 0x09)

#define REG_IDX_REGMAP_CRC			0x3E

#define DEFAULT_WORD_WIDTH			3
#define DEFAULT_FRAME_WORD_NUM		9

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef struct Ads131m08_context_t
{
	SAMD51_SERCOM sercom;
	SAMD51_GPIO_PORT reset;
	SAMD51_EIC_CHANNEL data_ready_ch;
	
	int word_width;
	int32_t frame_cache[DEFAULT_FRAME_WORD_NUM];
	ads131m8_data_captured_callback captured_cb;

	volatile int skip_frame;
	volatile uint32_t fram_count;
}Ads131m08_context;

static Ads131m08_context sCtx = {0};


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static int ads131m08_exec_command(uint16_t cmd, uint16_t *args, size_t args_size, uint32_t *response_words, size_t num_response_words);
static void _data_ready_cb(void);


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int ads131m08_set_captured_callback(ads131m8_data_captured_callback cb)
{
	sCtx.captured_cb = cb;

	return AI_OK;	
}

/*--------------------------------------------------------------------------*/
int ads131m08_read(uint32_t *words, size_t num_words)
{
	if (words == NULL) return AI_ERROR_NULL;
	if (num_words < DEFAULT_FRAME_WORD_NUM) return AI_ERROR_NOBUF;

	memcpy((void *)words, (void *)sCtx.frame_cache, sizeof(uint32_t) * DEFAULT_FRAME_WORD_NUM);

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int ads131m08_reg_read(uint8_t addr, uint32_t *value_word)
{
	int ret = 0;

	if (value_word == NULL) return AI_ERROR_NULL;

	sCtx.skip_frame = 1;
	{
		volatile uint32_t cnt = sCtx.fram_count;
		while (cnt == sCtx.fram_count);

		ret = ads131m08_exec_command(CMD_RREG((addr&0x3f), 0), NULL, 0, value_word, 1);
	}
	sCtx.skip_frame = 0;

	
	return ret;
}

/*--------------------------------------------------------------------------*/
int ads131m08_reg_write(uint8_t addr, uint32_t  value_word)
{
	uint32_t status[1];
	uint16_t val = (uint16_t)value_word;
	int ret = 0;

	sCtx.skip_frame = 1;
	{
		volatile uint32_t cnt = sCtx.fram_count;
		while (cnt == sCtx.fram_count);

		ret = ads131m08_exec_command(CMD_WREG((addr&0x3f), 0), &val, 1, status, 1);	
	}
	sCtx.skip_frame = 0;
	if (ret != 0) {
		return ret;
	}
	
	if (status[0] == RES_WREG((addr&0x3f), 0)) {
		return AI_OK;
	}
	else {
		return AI_ERROR_OUT_OF_RANGE;
	}
}

/*--------------------------------------------------------------------------*/
int ads131m08_initialize(SAMD51_SERCOM sercom, SAMD51_GPIO_PORT reset, SAMD51_EIC_CHANNEL data_ready_ch)
{
	// Assert Reset pin
	samd51_gpio_output(reset, 0);

	sCtx.sercom = sercom;
	sCtx.reset  = reset;
	sCtx.data_ready_ch = data_ready_ch;
	sCtx.word_width = DEFAULT_WORD_WIDTH;

	samd51_spi_initialize(sercom, SAMD51_GPIO_A10, 48000000, 1000000);

	{
		volatile uint32_t wait = 10000;
		while(wait--);
	}
	// Negate Reset pin
	samd51_gpio_output(reset, 1);

	// Reset all
	uint32_t status[10] = {0};
	ads131m08_exec_command(CMD_RESET, NULL, 0, status, 1);

	// Configure Data width 32bit(LSB Zero Padding mode)
	uint16_t data[1];
	data[0] = 0x0210;
	ads131m08_exec_command(CMD_WREG(0x02, 0), data, 1, status, 1);
	sCtx.word_width = 4;

	// Change sampling period = 4ms
	data[0] = 0xff02 | (0x7 << 2);
	ads131m08_exec_command(CMD_WREG(0x03, 0), data, 1, status, 1);

	// Configure Interrupt Pin
	samd51_external_interrupt_setup(data_ready_ch, SAMD51_EIC_SENSE_FALL, 0, _data_ready_cb);

	return AI_OK;	
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
volatile int sBusy = 0;
static void _rxdone(int error, uint8_t *rxbuf, size_t len)
{
	sBusy = 0;
	if (sCtx.word_width == DEFAULT_WORD_WIDTH) {
		volatile uint32_t *p = (volatile uint32_t *)rxbuf;
		*p = ((*p << 8) & 0xff00) | ((*p >> 8) & 0x00ff);
	}
	else if (sCtx.word_width == sizeof(uint32_t)) {
		volatile uint32_t *p = (volatile uint32_t *)rxbuf;
		for (int i=0 ; i<len/sizeof(uint32_t) ; ++i) {
			p[i] = ((p[i] << 16) & 0xff0000) | ((p[i] >> 16) & 0x0000ff) | ((p[i] >> 0) & 0x00ff00);
			p[i] = p[i] >> 8;
		}

	}
}

/*--------------------------------------------------------------------------*/
static int ads131m08_exec_command(uint16_t cmd, uint16_t *args, size_t args_word_size, uint32_t *response, size_t response_word_size)
{
	size_t cmd_frame_len = sizeof(uint16_t);
	uint8_t frame[20 * 3] = {0};
	frame[0] = (cmd >> 8) & 0x00ff;
	frame[1] = (cmd >> 0) & 0x00ff;
	frame[2] = 0;
	frame[3] = 0;

	if (args != NULL && args_word_size != 0) {
		for (size_t i=0 ; i<args_word_size ; ++i) {
			frame[(i+1)*sCtx.word_width + 0] = (args[i] >> 8) & 0x00ff;
			frame[(i+1)*sCtx.word_width + 1] = (args[i] >> 0) & 0x00ff;
		}
	}

	if (cmd == CMD_RESET) {
		cmd_frame_len = (sCtx.word_width * sizeof(uint8_t)) + (8 * sCtx.word_width * sizeof(uint8_t)) + (sCtx.word_width * sizeof(uint8_t));
	}
	else {
		cmd_frame_len = (args_word_size + 1) * sCtx.word_width;
	}

	volatile int ret = samd51_spi_txrx(sCtx.sercom, frame, NULL, cmd_frame_len, NULL);
	if (ret != AI_OK) {
		return ret;
	}

	sBusy = 1;
	ret = samd51_spi_txrx(sCtx.sercom, NULL, (uint8_t *)response, response_word_size * sCtx.word_width, _rxdone);
	while (sBusy != 0);
	
	return ret;
}

/*--------------------------------------------------------------------------*/
static void _full_frame_captured_cb(int error, uint8_t *rxbuf, size_t len)
{
	volatile uint32_t *p = (volatile uint32_t *)rxbuf;
	for (int i=0 ; i<len/sizeof(uint32_t) ; ++i) {
		p[i] = ((p[i] << 16) & 0xff0000) | ((p[i] >> 16) & 0x0000ff) | ((p[i] >> 0) & 0x00ff00);
		p[i] = p[i] << 8;
	}
	p[0] = p[0] >> 8;

	for (int i=1 ;i<DEFAULT_FRAME_WORD_NUM ; ++i) {
		sCtx.frame_cache[i] = sCtx.frame_cache[i] / 256;
		sCtx.frame_cache[i] = (int32_t)((float)sCtx.frame_cache[i] * (1200000.0f / (float)0x007fffff));
	}

	if (p[0] == 0x0002ff00 && sCtx.captured_cb) {
		sCtx.captured_cb(&sCtx.frame_cache[1], DEFAULT_FRAME_WORD_NUM-1);
	}

	return;
}

/*--------------------------------------------------------------------------*/
static void _data_ready_cb(void)
{
	sCtx.fram_count++;
	if (sCtx.skip_frame) return;
	
	(void)samd51_spi_txrx(sCtx.sercom, NULL, (uint8_t *)sCtx.frame_cache, DEFAULT_FRAME_WORD_NUM * sCtx.word_width, _full_frame_captured_cb);
}