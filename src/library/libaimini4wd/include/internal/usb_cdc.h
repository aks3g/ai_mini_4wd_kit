/*
 * usb_cdc.h
 *
 * Created: 2020/06/18 14:30:36
 *  Author: kiyot
 */ 


#ifndef USB_CDC_H_
#define USB_CDC_H_


int usbCdcInitialize(void);

int usbCdc_tx(uint8_t *buf, size_t len);
int usbCdc_try_rx(uint8_t *buf);
int usbCdc_rx(uint8_t *buf, size_t len);

int usbCdc_putc(char c);
int usbCdc_puts(char *str);


#endif /* USB_CDC_H_ */