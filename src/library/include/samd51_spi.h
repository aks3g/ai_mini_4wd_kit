/*
 * samd51_spi.h
 *
 * Created: 2021/07/02 5:33:16
 *  Author: kiyot
 */ 


#ifndef SAMD51_SPI_H_
#define SAMD51_SPI_H_


#include <stdint.h>
#include <stddef.h>

#include <samd51_sercom.h>
#include <samd51_gpio.h>

typedef void (*SAMD51_SPI_TXRX_DONE_CB)(int error, uint8_t *rxbuf, size_t len);

int samd51_spi_initialize(SAMD51_SERCOM sercom, SAMD51_GPIO_PORT ss, uint32_t peripheral_clock, uint32_t sclk_clock);
void samd51_spi_finalize(SAMD51_SERCOM sercom);
int samd51_spi_txrx(SAMD51_SERCOM sercom, const uint8_t *txbuf, uint8_t *rxbuf, const size_t len, SAMD51_SPI_TXRX_DONE_CB callback);



#endif /* SAMD51_SPI_H_ */