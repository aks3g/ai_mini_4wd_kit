/*
 * usb_def.h
 *
 * Created: 2020/07/03 19:58:53
 *  Author: kiyot
 */ 


#ifndef USB_DEV_H_
#define USB_DEV_H_


int initialize_usb(const char *serial_number, const char *cdc_interface_name);


#endif /* USB_DEV_H_ */