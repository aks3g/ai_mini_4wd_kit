/*
 * dynamo.c
 *
 * Created: 2021/06/30 5:26:16
 *  Author: kiyot
 */ 
#include <stddef.h>
#include <stdint.h>
#include <fastmath.h>
#include <string.h>

#include <samd51_error.h>
#include <samd51_gpio.h>
#include <samd51_adc.h>
#include <samd51_timer.h>
#include <samd51_spi.h>
#include <samd51_interrupt.h>

#include "drivers/ads131m08.h"
#include "drivers/dynamo.h"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef struct DynamoChannel_t
{
	SAMD51_TC tc;
	SAMD51_GPIO_PORT pwm0;
	SAMD51_GPIO_PORT pwm1;
	SAMD51_GPIO_PORT dir;
	SAMD51_EIC_CHANNEL intr;
	
	int16_t current_duty;
	int16_t target_rpm;
	int16_t target_current;	
	
	uint16_t cnt;
	uint16_t cnt_work;
	uint32_t last_timestamp;
	uint32_t delta;
} DynamoChannnel;

#define MAX_DYNAMO_NUM			4

static DynamoChannnel sCtx[MAX_DYNAMO_NUM] =
{
	// Dynamo 0
	{
		SAMD51_TC5,
		SAMD51_GPIO_B10,
		SAMD51_GPIO_B11,
		SAMD51_GPIO_B12,
		SAMD51_EIC_CHANNEL13,
		0,
		0,
		0,
		0,
		0,
	},
	// Dynamo 1
	{
		SAMD51_TC2,
		SAMD51_GPIO_A12,
		SAMD51_GPIO_A13,
		SAMD51_GPIO_B14,
		SAMD51_EIC_CHANNEL15,
		0,
		0,
		0,
		0,
		0,
	},
	// Dynamo 2
	{
		SAMD51_TC3,
		SAMD51_GPIO_A14,
		SAMD51_GPIO_A15,
		SAMD51_GPIO_A16,
		SAMD51_EIC_CHANNEL1,
		0,
		0,
		0,
		0,
		0,
	},
	// Dynamo 3
	{
		SAMD51_TCC0,
		SAMD51_GPIO_B16,
		SAMD51_GPIO_B17,
		SAMD51_GPIO_A18,
		SAMD51_EIC_CHANNEL3,
		0,
		0,
		0,
		0,
		0,
	},
};

#define AVERAGE_WINDOW_SIZE			25
typedef struct AdcCaptureContext_t
{
	int updated;
	
	uint32_t dcount;
	uint32_t dptr;
	int32_t work[2][8];
	int32_t average[8];

} AdcCaptureContext;

static AdcCaptureContext sAdcCtx;

static DynamoCallback sDynamoSensorCapturedCb = NULL;
static int16_t sBatteryVoltage = 0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _set_dir(SAMD51_GPIO_PORT pin, int dir);
static void _set_pwm_enable(int ch, SAMD51_GPIO_PORT pwm_pin);
static void _set_pwm_disable(int ch, SAMD51_GPIO_PORT pwm_pin);

static void _dynamo0_inter_cb(void);
static void _dynamo1_inter_cb(void);
static void _dynamo2_inter_cb(void);
static void _dynamo3_inter_cb(void);

static void _rpm_counter_cb(void);
static int _adc_done_cb(const int32_t *val, const size_t len);
static void _battery_adc_done_cb(int status, int16_t result);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int dynamoInitialize(void)
{
	// base clock 1MHz t = 1us
	// Prescalse = 1/4
	// T = 4us x 65535 = 262140us = 262ms.
	samd51_tc_initialize_as_timer(SAMD51_TC0, 1000000, 100000, _rpm_counter_cb);
	samd51_tcc_initialize_as_freerun_counter(SAMD51_TCC1, SAMD51_TC_PRESCALE_DIV1);

	// base clock 1Mhz
	// Target PWM frequency 50kHz
	// Period 255
	// (1MHz / 255) x prescale = 50kHz
	// prescale = 50kHz / (1MHz / 255) = 12.75
	// DIV 8 or 16
	samd51_tc_initialize_as_pwm(sCtx[0].tc, SAMD51_TC_PRESCALE_DIV16);
	samd51_tc_initialize_as_pwm(sCtx[1].tc, SAMD51_TC_PRESCALE_DIV16);
	samd51_tc_initialize_as_pwm(sCtx[2].tc, SAMD51_TC_PRESCALE_DIV16);
	samd51_tcc_initialize_as_pwm(sCtx[3].tc, SAMD51_TC_PRESCALE_DIV16, 0xff, (1<<5) | (1<<4));

	samd51_external_interrupt_initialize(0);
	samd51_external_interrupt_setup(sCtx[0].intr, SAMD51_EIC_SENSE_FALL, 0, _dynamo0_inter_cb);
	samd51_external_interrupt_setup(sCtx[1].intr, SAMD51_EIC_SENSE_FALL, 0, _dynamo1_inter_cb);
	samd51_external_interrupt_setup(sCtx[2].intr, SAMD51_EIC_SENSE_FALL, 0, _dynamo2_inter_cb);
	samd51_external_interrupt_setup(sCtx[3].intr, SAMD51_EIC_SENSE_FALL, 0, _dynamo3_inter_cb);

	// Battery Voltage
	//J refer to Table 45-3. Averaging.
	SAMD51_ADC_POST_PROCESS_OPT adc0_opt;
	{
		adc0_opt.average_cnt = SAMD51_ADC_AVERAGE_16_SAMPLES;
		adc0_opt.average_div_power = 4;
	}
	samd51_adc_setup(0, SAMD51_ADC_SINGLE_SHOT, SAMD51_ADC_BIT_RES_12, SAMD51_ADC_REF_EXTERNAL_REFA, &adc0_opt);

	// capture every 4ms
	ads131m08_set_captured_callback(_adc_done_cb);

	return 0;
}

/*--------------------------------------------------------------------------*/
int dynamoSetDuty(uint32_t ch, int16_t val)
{
	if (ch > MAX_DYNAMO_NUM) {
		return AI_ERROR_NODEV;
	}
	else if (val > 255 || val < -255) {
		return AI_ERROR_OUT_OF_RANGE;
	}
	

	if (val == 0) {
		_set_pwm_disable(ch, sCtx[ch].pwm0);
		_set_pwm_disable(ch, sCtx[ch].pwm1);
	}
	else if (val > 0) {
		if (sCtx[ch].current_duty <= 0) {
			_set_pwm_disable(ch, sCtx[ch].pwm0);
			_set_pwm_disable(ch, sCtx[ch].pwm1);
			_set_dir(sCtx[ch].dir, 0);
			_set_pwm_enable(ch, sCtx[ch].pwm0);
			_set_pwm_disable(ch, sCtx[ch].pwm1);
		}

		if (ch == 3) {
			samd51_tcc_set_pwm(sCtx[ch].tc, 4, val);
		}
		else {
			samd51_tc_set_pwm(sCtx[ch].tc, 0, val);
		}
	}
	else if (val < 0) {
		if (sCtx[ch].current_duty >= 0) {
			_set_pwm_disable(ch, sCtx[ch].pwm0);
			_set_pwm_disable(ch, sCtx[ch].pwm1);
			_set_dir(sCtx[ch].dir, 1);
			_set_pwm_disable(ch, sCtx[ch].pwm0);
			_set_pwm_enable(ch, sCtx[ch].pwm1);
		}

		if (ch == 3) {
			samd51_tcc_set_pwm(sCtx[ch].tc, 5, -val);
		}
		else {
			samd51_tc_set_pwm(sCtx[ch].tc, 1, -val);
		}
	}

	sCtx[ch].current_duty = val;

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int dynamoSetCurrent(uint32_t ch, int16_t current_mA)
{
	return 0;
}

/*--------------------------------------------------------------------------*/
int dynamoSetTorque(uint32_t ch, int16_t torque)
{
	return 0;
}

/*--------------------------------------------------------------------------*/
int32_t dynamoGetCurrent_mA(uint32_t ch)
{
	if (ch > 4) return 0;
	return 4 * sAdcCtx.average[(ch * 2) + 1] / 1000;
}

/*--------------------------------------------------------------------------*/
int32_t dynamoGetReverseEmf_mV(uint32_t ch)
{
	if (ch > 4) return 0;
	return sAdcCtx.average[ch * 2] / 1000;
}

/*--------------------------------------------------------------------------*/
float dynamoGetRpm(uint32_t ch)
{	
	if (sCtx[ch].delta == 0){
		return 0.0f;
	}
	
	// 1cnt = 1us’PˆÊ
	float rpm = 60.0f * 1000000.0f / (float)sCtx[ch].delta;
	return rpm;
}

/*--------------------------------------------------------------------------*/
float   dynamoGetVelocity_kmph(uint32_t ch)
{
	float rph = dynamoGetRpm(ch) * 60;

	return rph * (54.0f / 16.0f) * (13.5f * M_PI) / 1000000.0f;
}

/*--------------------------------------------------------------------------*/
int16_t dynamoGetBatteryVoltage_mV(void)
{
	return sBatteryVoltage;
}

/*--------------------------------------------------------------------------*/
int dynamoRegisterCapturedCallback(DynamoCallback cb)
{
	sDynamoSensorCapturedCb = cb;
	
	return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _set_dir(SAMD51_GPIO_PORT pin, int dir)
{
	samd51_gpio_output(pin, dir);
	return;
}

/*--------------------------------------------------------------------------*/
static void _set_pwm_enable(int ch, SAMD51_GPIO_PORT pwm_pin)
{
	if (ch == 3) {
		samd51_gpio_configure(pwm_pin, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_G);
	}
	else{
		samd51_gpio_configure(pwm_pin, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_FUNC_E);
	}
	return;
}
	
/*--------------------------------------------------------------------------*/
static void _set_pwm_disable(int ch, SAMD51_GPIO_PORT pwm_pin)
{
	(void)ch;
	samd51_gpio_configure(pwm_pin, SAMD51_GPIO_OUT, SAMD51_GPIO_PULLUP_DOWN, SAMD51_GPIO_MUX_DEFAULT);
	samd51_gpio_output(pwm_pin, 0);
	return;
}


/*--------------------------------------------------------------------------*/
static void _dynamo0_inter_cb(void)
{
	uint32_t cnt = samd51_tcc_get_counter(SAMD51_TCC1);

	sCtx[0].cnt_work++;
	sCtx[0].delta = (cnt - sCtx[0].last_timestamp) & 0x00ffffff;
	sCtx[0].last_timestamp = cnt;
}

/*--------------------------------------------------------------------------*/
static void _dynamo1_inter_cb(void)
{
	uint32_t cnt = samd51_tcc_get_counter(SAMD51_TCC1);

	sCtx[1].cnt_work++;
	sCtx[1].delta = (cnt - sCtx[1].last_timestamp) & 0x00ffffff;
	sCtx[1].last_timestamp = cnt;
}

/*--------------------------------------------------------------------------*/
static void _dynamo2_inter_cb(void)
{
	uint32_t cnt = samd51_tcc_get_counter(SAMD51_TCC1);

	sCtx[2].cnt_work++;
	sCtx[2].delta = (cnt - sCtx[2].last_timestamp) & 0x00ffffff;
	sCtx[2].last_timestamp = cnt;
}

/*--------------------------------------------------------------------------*/
static void _dynamo3_inter_cb(void)
{
	uint32_t cnt = samd51_tcc_get_counter(SAMD51_TCC1);

	sCtx[3].cnt_work++;
	sCtx[3].delta = (cnt - sCtx[3].last_timestamp) & 0x00ffffff;
	sCtx[3].last_timestamp = cnt;
}

/*--------------------------------------------------------------------------*/
static void _rpm_counter_cb(void)
{
	for (int i=0 ; i<4 ; ++i) {
		if (0 == sCtx[i].cnt_work){
			sCtx[i].delta = 0;			
		}
		sCtx[i].cnt_work = 0;
	}
}

static int _adc_done_cb(const int32_t *val, const size_t len)
{
	for (int i=0 ; i<8 ; ++i) {
		sAdcCtx.work[sAdcCtx.dptr][i] += val[i];
	}

	sAdcCtx.dcount++;
	if(sAdcCtx.dcount >= AVERAGE_WINDOW_SIZE) {
		sAdcCtx.dcount = 0;
		sAdcCtx.updated = 1;
		sAdcCtx.dptr = 1 - sAdcCtx.dptr;
		memset(sAdcCtx.work[sAdcCtx.dptr], 0, sizeof(int32_t) * 8);

		samd51_adc_convert(0, SAMD51_ADC_SINGLE_END, SAMD51_ADC_POS_AIN0, SAMD51_ADC_NEG_GND, _battery_adc_done_cb);
	}

	return 0;
}
void _battery_adc_done_cb(int status, int16_t result)
{
	sBatteryVoltage = 2 * (3300 * (int32_t)result) / (int32_t)4095;
	
	for (int i=0 ; i<8 ; ++i) {
		sAdcCtx.average[i] = sAdcCtx.work[1-sAdcCtx.dptr][i] / AVERAGE_WINDOW_SIZE;
	}
	
	if (sDynamoSensorCapturedCb) sDynamoSensorCapturedCb();
}


