/*
 * spi_driver.h
 *
 * Created: 2014/08/21 21:42:16
 *  Author: sazae7
 */ 


#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define SPI_OK					(0)
#define SPI_INVALID_CLK			(-1)
#define SPI_INVALID_TYPE		(-2)
#define SPI_INVALID_MODE		(-3)
#define SPI_INVALID_ORDER		(-4)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef enum {
	SPI_DATA_ORDER_MSB_FIRST	= 0,
	SPI_DATA_ORDER_LSB_FIRST	= 1
} SPI_DATA_ORDER;

/*---------------------------------------------------------------------------*/
typedef enum {
	SPI_TRANSFER_MODE_0 = 0,	/* On Idle SCK Level = L, Rising Edge = sample, Falling Edge = setup  */
	SPI_TRANSFER_MODE_1 = 1,	/* On Idle SCK Level = L, Rising Edge = setup,  Falling Edge = sample */
	SPI_TRANSFER_MODE_2 = 2,	/* On Idle SCK Level = H, Rising Edge = setup,  Falling Edge = sample */
	SPI_TRANSFER_MODE_3 = 3		/* On Idle SCK Level = H, Rising Edge = sample, Falling Edge = setup  */
} SPI_TRANSFER_MODE;

/*---------------------------------------------------------------------------*/
typedef enum {
	SPI_CLK_DIV_2,
	SPI_CLK_DIV_4,
	SPI_CLK_DIV_8,
	SPI_CLK_DIV_16,
	SPI_CLK_DIV_32,
	SPI_CLK_DIV_64,
	SPI_CLK_DIV_128	
} SPI_CLK_SELECT;

/*---------------------------------------------------------------------------*/
typedef enum {
	SPI_TYPE_4WIRE = 0,
	SPI_TYPE_3WIRE = 1
} SPI_TYPE;


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
uint8_t initialize_spi_master(SPI_TYPE type, SPI_TRANSFER_MODE mode, SPI_DATA_ORDER order, SPI_CLK_SELECT clk);
uint8_t spi_tx(uint8_t data);
uint8_t spi_rx(void);

uint8_t spi_txrx_pio(uint8_t data);

#ifdef __cplusplus
};
#endif

#endif /* SPI_DRIVER_H_ */