/*
 * adc_driver.h
 *
 * Created: 2013/09/10 20:13:33
 *  Author: sazae7
 */ 


#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define ADC_OK							(0)

#define ADC_INVALID_POINTER				(-1)
#define ADC_UNKNOWN_CAPTURE_MODE		(-2)
#define ADC_INVALID_BIT_RESOLUTION		(-3)
#define ADC_INVALID_SAMPLING_RATE		(-4)
#define ADC_INVALID_CHANNEL				(-5)


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef enum
{
	ADC_ONESHOT = 0,
	ADC_FREERUN
} ADC_MODE;

/*---------------------------------------------------------------------------*/
typedef enum
{
	ADC_CH0 = 0,
	ADC_CH1,
	ADC_CH2,
	ADC_CH3,
	ADC_CH4,
	ADC_CH5,
	ADC_CH6,
	ADC_CH7,
	ADC_CH8,
	ADC_CH9,
	ADC_CH10,
	ADC_CH11,
	ADC_CH12,
	ADC_CH13,
	ADC_CH14,
	ADC_CH15
} ADC_CHANNEL;

/*---------------------------------------------------------------------------*/
typedef enum
{
	ADC_RESOLUTION_8BIT = 0,	
	ADC_RESOLUTION_12BIT = 1
} ADC_BIT_MODE;

/*---------------------------------------------------------------------------*/
typedef enum{
	ADC_GAIN_1X = 0,
	ADC_GAIN_2X,
	ADC_GAIN_4X,
	ADC_GAIN_8X,
	ADC_GAIN_16X,
	ADC_GAIN_32X,
	ADC_GAIN_64X,
	ADC_GAIN_DIV2,
}ADC_GAIN;


/*---------------------------------------------------------------------------*/
typedef enum{
	VREF_INTERNAL_1V            = 0,
	VREF_INTERNAL_VCC_1_6       = 1,
	VREF_EXTERNAL_VREF_ON_PORTA = 2,
	VREF_EXTERNAL_VREF_ON_PORTD = 3,
	VREF_INTERNAL_VCC_2         = 4
}ADC_REFERNCE_SELECT;


/*---------------------------------------------------------------------------*/
typedef struct {
	ADC_REFERNCE_SELECT vrefSelect;
} ADC_INIT_OPT;


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
uint8_t initialize_adcModule(uint32_t sps, uint32_t sysclkHz, ADC_MODE mode, uint8_t bitResolution, ADC_INIT_OPT *opt);
uint8_t adc_selectChannel(ADC_CHANNEL ch, ADC_GAIN gain);
uint16_t adc_grabOneShot(void);
inline uint8_t adc_grabFreerun(void)
{
	/* 24.14.6 INTFLAGS ? Interrupt Flags register */
	while (ADCA.INTFLAGS == 0);
	ADCA.INTFLAGS = ADC_CH0IF_bm; // Clear

	return ADCA.CH0RES;
}

inline uint16_t adc_grabFreerunWORD(void)
{
	/* 24.14.6 INTFLAGS ? Interrupt Flags register */
	while (ADCA.INTFLAGS == 0);
	ADCA.INTFLAGS = ADC_CH0IF_bm; // Clear

	return ADCA.CH0RES;
}

#ifdef __cplusplus
};
#endif

#endif /* ADC_DRIVER_H_ */