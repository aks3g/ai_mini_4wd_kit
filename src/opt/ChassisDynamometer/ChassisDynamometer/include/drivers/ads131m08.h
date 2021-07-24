/*
 * ads131m08.h
 *
 * Created: 2021/07/04 11:14:56
 *  Author: kiyot
 */ 


#ifndef ADS131M08_H_
#define ADS131M08_H_
#include <samd51_gpio.h>
#include <samd51_interrupt.h>
#include <samd51_sercom.h>

typedef int (*ads131m8_data_captured_callback)(const int32_t *val, const size_t len);

int ads131m08_initialize(SAMD51_SERCOM sercom, SAMD51_GPIO_PORT reset, SAMD51_EIC_CHANNEL data_ready_ch);
int ads131m08_set_captured_callback(ads131m8_data_captured_callback cb);
int ads131m08_read(uint32_t *words_uV, size_t num_words);
int ads131m08_reg_read(uint8_t addr, uint32_t *value_word);
int ads131m08_reg_write(uint8_t addr, uint32_t  value_word);

#endif /* ADS131M08_H_ */