/*
 * usb_cdc.c
 *
 * Created: 2020/06/16 7:55:06
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <string.h>

#include <samd51_error.h>
#include <samd51_usb_device.h>
#include <samd51_uart.h>

#include "drivers/usb_dev.h"
#include "drivers/usb_cdc.h"


#pragma pack(1)
typedef struct LineCoding_t{
	uint32_t dwDTERate;
	uint8_t  bCharFormat;
	uint8_t  bParityType;
	uint8_t  bDataBits;
} LineCoding;
#pragma pack()

static LineCoding sLineCoding = {9600, 0, 0, 8};

static volatile uint32_t sDataIdx = 0;
static UsbDeviceRequest sSerialStateResponse;
static uint16_t sSerialState = 0x0003;

static int sCommInEp = 0;
static int sDataInEp = 0;
static int sDataOutEp = 0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _usb_communication_class_if_interrupt_in(int error);

static void _usb_cdc_bulk_out(uint8_t *buf, uint32_t size);
static void _usb_cdc_bulk_in (int error);

static void _usb_cdc_set_interrupt_data(int error);
static void _usb_cdc_reset_internal(void);
static void _usbCdc_cleanup(void);
static void _usbCdc_wakeup(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static uint8_t sInterruptInBuf[64];
static uint8_t sBulkOutBuf[64];
static uint8_t sBulkInBuf[64];

static SAMD51_UART_FIFO sCdcTxFifo;
static SAMD51_UART_FIFO sCdcRxFifo;
static uint8_t sTxBuf[2048];
static uint8_t sRxBuf[2048];
static uint8_t sTmpBuf[64];

static volatile uint8_t sCdcLinkUp = 0;
static volatile uint8_t sTxWorking = 0;

static UsbCdcLinkUpCallback sLinkUpCallback;

/*--------------------------------------------------------------------------*/
int usbCdcInitialize(int comm_if_ep, int data_if_in_ep, int data_if_out_ep)
{	
	sSerialStateResponse.bmRequestType.byte = 0xA1;
	sSerialStateResponse.bRequest = 0x20;
	sSerialStateResponse.wIndex = 0;
	sSerialStateResponse.wValue = 0;
	sSerialStateResponse.wLength = 2;

	sCommInEp  = comm_if_ep;
	sDataInEp  = data_if_in_ep;
	sDataOutEp = data_if_out_ep;

	samd51_uart_fifo_setup(&sCdcTxFifo, sTxBuf, sizeof(sTxBuf));
	samd51_uart_fifo_setup(&sCdcRxFifo, sRxBuf, sizeof(sRxBuf));
	
	//J Setup USB Device
	samd51_usb_device_setup_IN_endpoint (comm_if_ep, cEpInterrupt, _usb_communication_class_if_interrupt_in,  sInterruptInBuf, sizeof(sInterruptInBuf));

	samd51_usb_device_setup_IN_endpoint (data_if_in_ep,  cEpBulk, _usb_cdc_bulk_in,  sBulkInBuf,  sizeof(sBulkInBuf));
	samd51_usb_device_setup_OUT_endpoint(data_if_out_ep, cEpBulk, _usb_cdc_bulk_out, sBulkOutBuf, sizeof(sBulkOutBuf));

	samd51_register_wakeup_func(_usbCdc_wakeup);
	samd51_register_cleanup_func(_usbCdc_cleanup);

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int usbCdcRegisterLinkUpCallback(UsbCdcLinkUpCallback cb)
{
	sLinkUpCallback = cb;
	
	return AI_OK;
}


/*--------------------------------------------------------------------------*/
void usbCdc_control(UsbDeviceRequest *req)
{
	switch (req->bRequest)
	{
	case USB_CDC_REQ_GET_LINE_CODING:
		samd51_usb_transfer_control_in(&sLineCoding, 7);
		break;
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
		(void)req->wValue;
		samd51_usb_transfer_control_in(NULL, 0);
		sDataIdx = 0;
		_usb_cdc_set_interrupt_data(AI_OK);
		_usb_cdc_reset_internal();
		break;
	case USB_CDC_REQ_SET_LINE_CODING:
		(void)req->wValue;
		samd51_usb_transfer_control_in(NULL, 0);
		break;
	default:
		break;
	}
	return;
}

static void _usb_cdc_set_interrupt_data(int error)
{
	if (error == AI_OK) {
		if (sDataIdx < 3) {
			sDataIdx++;
		}
	}

	if (sDataIdx == 1) {
		samd51_usb_transfer_bulk_in(sCommInEp, &sSerialStateResponse, sizeof(sSerialStateResponse));
	}
	else if (sDataIdx == 2) {
		samd51_usb_transfer_bulk_in(sCommInEp, &sSerialState, sizeof(sSerialState));
	}

	return;
}

/*--------------------------------------------------------------------------*/
static void _usb_communication_class_if_interrupt_in(int error)
{
	_usb_cdc_set_interrupt_data(error);
	return;
}

/*--------------------------------------------------------------------------*/
static void _usb_cdc_bulk_out(uint8_t *buf, uint32_t size)
{
	//受信バッファに積む
	for (uint32_t i=0 ; i<size && !samd51_uart_fifo_is_full(&sCdcRxFifo) ; ++i) {
		samd51_uart_fifo_enqueue(&sCdcRxFifo, buf[i]);
	}

	samd51_usb_transfer_bulk_out(sDataOutEp);

	return;
}

/*--------------------------------------------------------------------------*/
static size_t _usbCdc_tx(SAMD51_UART_FIFO *fifo)
{
	size_t sRxSize = 0;
	while (!samd51_uart_fifo_is_empty(fifo) && sRxSize < 32) {
		sTmpBuf[sRxSize++] = samd51_uart_fifo_dequeue(fifo);
	}

	if (sRxSize && sCdcLinkUp) {
		samd51_usb_transfer_bulk_in(sDataInEp, sTmpBuf, sRxSize);
	} 
	
	return sRxSize;
}

/*--------------------------------------------------------------------------*/
static void _usb_cdc_bulk_in(int error)
{
	if (error == AI_OK) {
		if (samd51_uart_fifo_is_empty(&sCdcTxFifo)) {
			sTxWorking = 0;
			return ;
		}
	
		size_t size = _usbCdc_tx(&sCdcTxFifo);
		if (size == 0) sTxWorking = 0;
	}
	else {
		sTxWorking = 0;
	}

	return;
}


/*--------------------------------------------------------------------------*/
static void _usb_cdc_reset_internal(void)
{
	samd51_uart_fifo_setup(&sCdcTxFifo, sTxBuf, sizeof(sTxBuf));
	samd51_uart_fifo_setup(&sCdcRxFifo, sRxBuf, sizeof(sRxBuf));
	sCdcLinkUp = 1;
	sDataIdx = 0;
	
	if (sLinkUpCallback) {
		sLinkUpCallback();
	}
}

/*--------------------------------------------------------------------------*/
static void _usbCdc_cleanup(void)
{
	sCdcLinkUp = 0;
	samd51_uart_fifo_setup(&sCdcTxFifo, sTxBuf, sizeof(sTxBuf));
	samd51_uart_fifo_setup(&sCdcRxFifo, sRxBuf, sizeof(sRxBuf));
}

/*--------------------------------------------------------------------------*/
static void _usbCdc_wakeup(void)
{
	sCdcLinkUp = 1;
}

/*--------------------------------------------------------------------------*/
int usbCdc_tx(const uint8_t *buf, size_t len)
{
	if (sCdcLinkUp ==0) return AI_ERROR_NOT_READY;
		
	for (uint32_t i=0 ; i<len && !samd51_uart_fifo_is_full(&sCdcTxFifo) ; ++i) {
		samd51_uart_fifo_enqueue(&sCdcTxFifo, buf[i]);
	}

	samd51_usb_lock_in_transfer();
	if (!sTxWorking) {
		size_t size = _usbCdc_tx(&sCdcTxFifo);
		if (size != 0) {
			sTxWorking = 1;
		}
	}
	samd51_usb_unlock_in_transfer();

	return 0;
}

/*--------------------------------------------------------------------------*/
int usbCdc_try_rx(uint8_t *buf)
{
	if (sCdcLinkUp ==0) return AI_ERROR_NOT_READY;

	SAMD51_UART_FIFO *fifo = &sCdcRxFifo;
	if (samd51_uart_fifo_is_empty(fifo)) {
		return AI_ERROR_NOBUF;
	}
	*buf = samd51_uart_fifo_dequeue(fifo);

	return 0;
}

/*--------------------------------------------------------------------------*/
int usbCdc_rx(uint8_t *buf, size_t len)
{
	if (sCdcLinkUp ==0) return AI_ERROR_NOT_READY;

	SAMD51_UART_FIFO *fifo = &sCdcRxFifo;
	while (len--) {
		volatile int ret = 0;
		while ((ret = samd51_uart_fifo_is_empty(fifo)) && sCdcLinkUp);

		if (sCdcLinkUp == 0) return AI_ERROR_NOT_READY;

		*buf++ = samd51_uart_fifo_dequeue(fifo);
	}

	return 0;
}

/*--------------------------------------------------------------------------*/
int usbCdc_putc(const char c)
{
	if (sCdcLinkUp ==0) return AI_ERROR_NOT_READY;
	
	return usbCdc_tx((uint8_t *)&c, 1);
}

/*--------------------------------------------------------------------------*/
int usbCdc_puts(const char *str)
{
	if (sCdcLinkUp ==0) return AI_ERROR_NOT_READY;

	size_t len = strlen(str);
	return usbCdc_tx((uint8_t *)str, len);
}

/*--------------------------------------------------------------------------*/
int usbCdc_isLinkedUp(void)
{
	return sCdcLinkUp;
}

