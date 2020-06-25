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

#define WORD2BYTE(w)	((w>>0)&0xff), ((w>>8)&0xff)
#pragma pack(1)
uint8_t cUsbCommunicationDeviceDescriptor[] =
{
	//---------------------------------------------------------------------------
	// 0. Device Descriptor
	0x12,							// bLength
	0x01,							// bDescriptor Type = DEVICE(0x01)
	WORD2BYTE(0x200),				// bcdUSB
	USB_DEVICE_CLASS_CODE_CDC,		// bDeviceClass = USB_DEVICE_CLASS_CODE_CDC
	0x00,							// bDeviceSubClass = Unknown(0x00)
	0x00,							// bDeviceProtocol = Unknown(0x00)
	0x40,							// bMaxPacketSize0 = 64byte
	WORD2BYTE(0x1122),				// idVender
	WORD2BYTE(0x3344),				// idProduct
	WORD2BYTE(0x1984),				// bcdDevice
	0x00,							// iManufacturer
	0x00,							// iProduct
	0x00,							// iSerialNumber
	0x01,							// bNumConfigurations = 1
	//---------------------------------------------------------------------------
	// 1. Configuration Descriptor
	0x09,							// bLength
	0x02,							// bDescriptorType = CONFIGURATION(0x02)
	WORD2BYTE(0x0043),				// wTotalLength = 67
	0x02,							// bNumInterfaces = 2
	0x01,							// bConfigurationValue = 1
	0x00,							// iConfiguration = ?
	0xC0,							// bmAttributes = 0xC0(Self-powered)
	0x00,							// MaxPower = 0 (= 0mA)
	//---------------------------------------------------------------------------
	// 2. Interface Descriptor 0
	0x09,							// bLength
	0x04,							// bDescriptorType = INTERFACE(0x04)
	0x00,							// bInterfaceNumber = 0x00
	0x00,							// bAlternateSetting = 0x00
	0x01,							// bNumEndpoints = 1(IN)
	USB_INTERFACE_CLASS_CODE_CDC,	// bInterfaceClass = CDC
	USB_CDC_SUBCLASS_ACM,			// bInterfaceSubClass = ACM
	USB_CDC_PROTOCOL_COMMON_AT,		// bInterfaceProtocol = Unspecified
	0x00,							// iInterface
	//---------------------------------------------------------------------------
	// 2-1. Header Function Descriptor
	0x05,							// bLength
	0x24,							// bDescriptorType = CS_INTERFACE
	0x00,							// bDescriptorSubType = Header Func Desc
	WORD2BYTE(0x0110),				// bcdCDC = 1.1
	//---------------------------------------------------------------------------
	// 2-2. Call Management Functional Descriptor
	0x05,							// bLength
	0x24,							// bDescriptorType = CS_INTERFACE
	0x01,							// bDescriptorSubType = Call management func desc
	0x00,							// bmCapabilities
	0x01,							// bDataInterface
	//---------------------------------------------------------------------------
	// 2-3. ACM Function Descriptor
	0x04,							// bLength
	0x24,							// bDescriptorType = CS_INTERFACE
	0x02,							// bDescriptorSubType = ACM desc
	0x02,							// bmCapabilities
	//---------------------------------------------------------------------------
	// 2-4. Union Function Descriptor
	0x05,							// bLength
	0x24,							// bDescriptorType = CS_INTERFACE
	0x06,							// bDescriptorSubType = Union func desc
	0x00,							// bMasterInterface = Communication class interface
	0x01,							// bSlaveInterface0 = Data Class Interface
	//---------------------------------------------------------------------------
	// 2-5. Endpoint 2 Descriptor
	0x07,							// bLength
	0x05,							// bDescriptorType = ENDPOINT
	0x82,							// EndpointAddress = EP2/IN (0x02 | 0x80)
	0x03,							// bmAttributes = Interrupt
	WORD2BYTE(0x0008),				// wMaxPacketSize = 8 bytes
	0x00,							// bInterval

	//---------------------------------------------------------------------------
	// 3. Interface Descriptor 1
	0x09,							// bLength
	0x04,							// bDescriptorType = INTERFACE(0x04)
	0x01,							// bInterfaceNumber = 0x01
	0x00,							// bAlternateSetting = 0x00
	0x02,							// bNumEndpoints = 2(IN/OUT)
	0x0A,							// bInterfaceClass = CDC
	0x00,							// bInterfaceSubClass = ACM
	0x00,							// bInterfaceProtocol = Unspecified
	0x00,							// iInterface
	//---------------------------------------------------------------------------
	// 3-1. Endpoint 3 Descriptor
	0x07,							// bLength
	0x05,							// bDescriptorType = ENDPOINT
	0x03,							// EndpointAddress = EP3/OUT (0x03 | 0x00)
	0x02,							// bmAttributes = Bulk
	WORD2BYTE(0x0020),				// wMaxPacketSize = 64 bytes
	0x00,							// bInterval
	//---------------------------------------------------------------------------
	// 3-1. Endpoint 3 Descriptor
	0x07,							// bLength
	0x05,							// bDescriptorType = ENDPOINT
	0x81,							// EndpointAddress = EP1/IN (0x01 | 0x80)
	0x02,							// bmAttributes = Bulk
	WORD2BYTE(0x0020),				// wMaxPacketSize = 64 bytes
	0x00,							// bInterval
};
#pragma pack()

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _usb_cdc_control_transfer_cb(UsbDeviceRequest *req);

static void _usb_communication_class_if_interrupt_in(int error);

static void _usb_cdc_bulk_out(uint8_t *buf, uint32_t size);
static void _usb_cdc_bulk_in (int error);

static void _usb_cdc_set_interrupt_data(int error);
static void _usb_cdc_reset_internal(void);
static void _usbCdc_cleanup(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static uint8_t sInterruptInBuf0[64];
static uint8_t sInterruptInBuf1[64];

static uint8_t sBulkOutBuf0[64];
static uint8_t sBulkOutBuf1[64];
static uint8_t sBulkInBuf0[64];
static uint8_t sBulkInBuf1[64];

static SAMD51_UART_FIFO sCdcTxFifo;
static SAMD51_UART_FIFO sCdcRxFifo;
static uint8_t sTxBuf[512];
static uint8_t sRxBuf[512];
static uint8_t sTmpBuf[64];

static volatile uint8_t sCdcLinkUp = 0;
static volatile uint8_t sTxWorking = 0;

/*--------------------------------------------------------------------------*/
int usbCdcInitialize(void)
{	
	sSerialStateResponse.bmRequestType.byte = 0xA1;
	sSerialStateResponse.bRequest = 0x20;
	sSerialStateResponse.wIndex = 0;
	sSerialStateResponse.wValue = 0;
	sSerialStateResponse.wLength = 2;
	
	samd51_uart_fifo_setup(&sCdcTxFifo, sTxBuf, sizeof(sTxBuf));
	samd51_uart_fifo_setup(&sCdcRxFifo, sRxBuf, sizeof(sRxBuf));
	
	//J Setup USB Device
	samd51_usb_device_initialize();
	samd51_usb_setup_device(cUsbCommunicationDeviceDescriptor, sizeof(cUsbCommunicationDeviceDescriptor), _usb_cdc_control_transfer_cb);

	samd51_usb_device_setup_IN_endpoint (2, cEpInterrupt, _usb_communication_class_if_interrupt_in,  sInterruptInBuf0,  sInterruptInBuf1, sizeof(sInterruptInBuf0), sizeof(sInterruptInBuf1));

	samd51_usb_device_setup_IN_endpoint (1, cEpBulk, _usb_cdc_bulk_in,  sBulkInBuf0,  sBulkInBuf1,  sizeof(sBulkInBuf0),  sizeof(sBulkInBuf1));
	samd51_usb_device_setup_OUT_endpoint(3, cEpBulk, _usb_cdc_bulk_out, sBulkOutBuf0, sBulkOutBuf1, sizeof(sBulkOutBuf0), sizeof(sBulkOutBuf1));

	samd51_register_cleanup_func(_usbCdc_cleanup);

	samd51_usb_device_attach(1);

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
static void _usb_cdc_control_transfer_cb(UsbDeviceRequest *req)
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
		samd51_usb_transfer_bulk_in(2, &sSerialStateResponse, sizeof(sSerialStateResponse));
	}
	else if (sDataIdx == 2) {
		samd51_usb_transfer_bulk_in(2, &sSerialState, sizeof(sSerialState));
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

	samd51_usb_transfer_bulk_out(1);

	return;
}

/*--------------------------------------------------------------------------*/
static void _usbCdc_tx(SAMD51_UART_FIFO *fifo)
{
	size_t sRxSize = 0;
	while (!samd51_uart_fifo_is_empty(fifo) && sRxSize < 32) {
		sTmpBuf[sRxSize++] = samd51_uart_fifo_dequeue(fifo);
	}

	if (sRxSize != 0) {
		samd51_usb_transfer_bulk_in(1, sTmpBuf, sRxSize);
	} 
}

/*--------------------------------------------------------------------------*/
static void _usb_cdc_bulk_in(int error)
{
	if (samd51_uart_fifo_is_empty(&sCdcTxFifo)) {
		sTxWorking = 0;
		return ;
	}
	
	_usbCdc_tx(&sCdcTxFifo);
	
	return;
}


/*--------------------------------------------------------------------------*/
static void _usb_cdc_reset_internal(void)
{
	samd51_uart_fifo_setup(&sCdcTxFifo, sTxBuf, sizeof(sTxBuf));
	samd51_uart_fifo_setup(&sCdcRxFifo, sRxBuf, sizeof(sRxBuf));
	sCdcLinkUp = 1;
	sDataIdx = 0;
}

/*--------------------------------------------------------------------------*/
static void _usbCdc_cleanup(void)
{
	sCdcLinkUp = 0;
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
		sTxWorking = 1;
		_usbCdc_tx(&sCdcTxFifo);
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

