/*
 * gpio_driver.h
 *
 * Created: 2013/09/08 23:11:03
 *  Author: sazae7
 */ 


#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define GPIO_OK							(0)

#define GPIO_NOT_FOUND_THE_PORT			(-1)
#define GPIO_NOT_FOUNT_THE_PIN			(-2)
#define GPIO_INVALID_DIRECTION			(-3)
#define GPIO_INVALID_PIN_MODE			(-4)
#define GPIO_NULLPTR					(-5)
#define GPIO_INVALID_INTERRUPT_MODE		(-6)
#define GPIO_INVALID_CALLBACK			(-7)


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef enum {
	GPIO_PORTA = (0),
	GPIO_PORTD = (1),
	GPIO_PORTC = (2),
	GPIO_PORTR = (3),
	GPIO_PORT_MAX = (4)
} GPIO_PORT;

/*---------------------------------------------------------------------------*/
typedef enum {
	GPIO_PIN0 = 0,
	GPIO_PIN1 = 1,
	GPIO_PIN2 = 2,
	GPIO_PIN3 = 3,
	GPIO_PIN4 = 4,
	GPIO_PIN5 = 5,
	GPIO_PIN6 = 6,
	GPIO_PIN7 = 7,
	GPIO_PIN_MAX  = 8
} GPIO_PIN;

/*---------------------------------------------------------------------------*/
typedef enum {
	GPIO_OUT,
	GPIO_IN
} GPIO_DIR;

/*---------------------------------------------------------------------------*/
typedef enum {
	GPIO_PUSH_PULL,
	GPIO_OPEN_DRAIN
} GPIO_MODE;

/*---------------------------------------------------------------------------*/
typedef enum {
	INT_BOTHE_EDGE = 0,
	INT_RISING = 1,
	INT_FALLING = 2,
	INT_LEVEL = 3,
	INT_SETTING_MAX = 4,
} INTERRUPT_MODE;

/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef void (*gpio_interrupt_callback)(void);


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
uint8_t initialize_gpio(GPIO_PORT port, GPIO_PIN pin, GPIO_DIR dir, GPIO_MODE mode);
uint8_t gpio_output(GPIO_PORT port, GPIO_PIN pin, uint8_t out);
uint8_t gpio_output_toggle(GPIO_PORT port, GPIO_PIN pin);
uint8_t gpio_input(GPIO_PORT port, GPIO_PIN pin, uint8_t *in);

uint8_t gpio_configeInterrupt(GPIO_PORT port, GPIO_PIN pin, INTERRUPT_MODE mode, gpio_interrupt_callback callback);

#ifdef __cplusplus
};
#endif

#endif /* GPIO_DRIVER_H_ */