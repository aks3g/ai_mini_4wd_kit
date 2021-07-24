/*
 * usb_cdc.h
 *
 * Created: 2020/06/18 14:30:36
 *  Author: kiyot
 */ 


#ifndef USB_CDC_H_
#define USB_CDC_H_

#include "samd51_usb_device.h"

// Class Code
#define USB_DEVICE_CLASS_CODE_CDC					0x02
#define USB_INTERFACE_CLASS_CODE_CDC				0x02

// Subclass Code
#define USB_CDC_SUBCLASS_ACM						0x02

// Protocol Code
#define USB_CDC_PROTOCOL_COMMON_AT					0x01

// Class Specific Request Code
#define USB_CDC_REQ_SEND_ENCAPSULATED_COMMAND		0x00
#define USB_CDC_REQ_GET_ENCAPSULATED_RESPONSE		0x01
#define USB_CDC_REQ_SET_COMM_FEATURE				0x02
#define USB_CDC_REQ_GET_COMM_FEATURE				0x03
#define USB_CDC_REQ_CLEAR_COMM_FEATURE				0x04

#define USB_CDC_REQ_SET_AUX_LINE_STATE				0x10
#define USB_CDC_REQ_SET_HOOK_STATE					0x11
#define USB_CDC_REQ_PULSE_SETUP						0x12
#define USB_CDC_REQ_SEND_PULSE						0x13
#define USB_CDC_REQ_SET_PULSE_TIME					0x14
#define USB_CDC_REQ_RING_AUX_JACK					0x15

#define USB_CDC_REQ_SET_LINE_CODING					0x20
#define USB_CDC_REQ_GET_LINE_CODING					0x21
#define USB_CDC_REQ_SET_CONTROL_LINE_STATE			0x22
#define USB_CDC_REQ_SEND_BREAK						0x23

#define USB_CDC_REQ_SET_RINGER_PARAMS				0x30
#define USB_CDC_REQ_GET_RINGER_PARAMS				0x31
#define USB_CDC_REQ_SET_OPERATION_PARAMS			0x32
#define USB_CDC_REQ_GET_OPERATION_PARAMS			0x33
#define USB_CDC_REQ_SET_LINE_PARAMS					0x34
#define USB_CDC_REQ_GET_LINE_PARAMS					0x35
#define USB_CDC_REQ_DIAL_DIGITS						0x36

typedef void (*UsbCdcLinkUpCallback)(void);

int usbCdcInitialize(int comm_if_ep, int data_if_in_ep, int data_if_out_ep);
int usbCdcRegisterLinkUpCallback(UsbCdcLinkUpCallback cb);
void usbCdc_control(UsbDeviceRequest *req);

int usbCdc_tx(const uint8_t *buf, size_t len);
int usbCdc_try_rx(uint8_t *buf);
int usbCdc_rx(uint8_t *buf, size_t len);

int usbCdc_putc(const char c);
int usbCdc_puts(const char *str);
int usbCdc_isLinkedUp(void);

#endif /* USB_CDC_H_ */