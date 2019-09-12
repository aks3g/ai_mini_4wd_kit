/*
 * samd51_i2c.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SAMD51_I2C_H_
#define SAMD51_I2C_H_

#include <stdint.h>
#include <stddef.h>

#include <samd51_sercom.h>

typedef void (*SAMD51_I2C_DONE_CB)(int error);

int samd51_i2c_initialize(SAMD51_SERCOM sercom, uint32_t i2c_clock);
void samd51_i2c_finalize(SAMD51_SERCOM sercom);
int samd51_i2c_txrx(SAMD51_SERCOM sercom, const uint8_t slave_addr, const uint8_t *txbuf, const size_t txlen, uint8_t *rxbuf, const size_t rxlen, SAMD51_I2C_DONE_CB callback);

#endif /* SAMD51_I2C_H_ */