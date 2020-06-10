/*
 * odometer_reg_cb.h
 *
 * Created: 2020/06/09
 * Copyright ? 2020 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef ODOMETER_REG_CB_H_
#define ODOMETER_REG_CB_H_


extern void on_write_reg_reset(uint8_t val);
extern void on_write_reg_srom_version(uint8_t val);
extern void on_write_reg_debug_print_enable(uint8_t val);
extern void on_write_reg_led(uint8_t val);
extern void on_write_cpi_l(uint8_t val);
extern void on_write_cpi_h(uint8_t val);

#endif /* ODOMETER_REG_CB_H_ */