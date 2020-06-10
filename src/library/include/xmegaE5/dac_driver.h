/*
 * dac_driver.h
 *
 * Created: 2014/02/20 23:49:42
 *  Author: sazae7
 */ 


#ifndef DAC_DRIVER_H_
#define DAC_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define DAC_OK							(0)

#define DAC_INVALID_CHANNEL				(-1)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef enum {
	DAC_CH0 = 0,
	DAC_CH1 = 1
} DAC_CH;


/*---------------------------------------------------------------------------*/
typedef enum {
	DAC_VREF_INTERNAL_1V = 0,
	DAC_VREF_AVCC = 1,
	DAC_VREF_VREF_ON_PORTA = 2,
	DAC_VREF_VREF_ON_PORTD = 3	
} DAC_VREF_SELECT;

/*---------------------------------------------------------------------------*/
typedef struct {
	DAC_VREF_SELECT vrefSelect;
} DAC_INIT_OPT;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
uint8_t initialize_dac(DAC_INIT_OPT *opt);
uint8_t dac_enable(DAC_CH channel, uint8_t enable);
uint8_t dac_setValue(DAC_CH channel, uint16_t val);

uint8_t dac_calibrat(DAC_CH channel, uint8_t gain, uint8_t offset);

#ifdef __cplusplus
};
#endif

#endif /* DAC_DRIVER_H_ */