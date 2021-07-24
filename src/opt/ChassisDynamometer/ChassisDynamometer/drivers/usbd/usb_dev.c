/*
 * usb_dev.c
 *
 * Created: 2020/09/21 5:32:46
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <string.h>

#include <samd51_error.h>
#include <samd51_usb_device.h>

#include "drivers/usb_dev.h"
#include "drivers/usb_cdc.h"

#define WORD2BYTE(w)	((w>>0)&0xff), ((w>>8)&0xff)
#pragma pack(1)
uint8_t cUsbCommunicationDeviceDescriptor[] =
{
	//---------------------------------------------------------------------------
	// 0. Device Descriptor
	0x12,							// bLength
	0x01,							// bDescriptor Type = DEVICE(0x01)
	WORD2BYTE(0x200),				// bcdUSB
	0x02,							// bDeviceClass = USB_DEVICE_CLASS_CODE_CDC(0x02)
	0x00,							// bDeviceSubClass = (0x00)
	0x00,							// bDeviceProtocol = (0x00)
	0x40,							// bMaxPacketSize0 = 64byte
	WORD2BYTE(0x16C0),				// idVender
	WORD2BYTE(0x3344),				// idProduct
	WORD2BYTE(0x1984),				// bcdDevice
	0x01,							// iManufacturer
	0x02,							// iProduct
	0x03,							// iSerialNumber
	0x01,							// bNumConfigurations = 1
	//---------------------------------------------------------------------------
	// 1. Configuration Descriptor
	0x09,							// bLength
	0x02,							// bDescriptorType = CONFIGURATION(0x02)
	WORD2BYTE(0x004D),				// wTotalLength = 0x4D
	0x02,							// bNumInterfaces = 2
	0x01,							// bConfigurationValue = 1
	0x00,							// iConfiguration = ?
	0x80,							// bmAttributes = 0x80(Self-powered)
	0xFA,							// MaxPower = FA (= 500mA)
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
	0x04,							// iInterface
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
	0x04,							// iInterface
	//---------------------------------------------------------------------------
	// 3-1. Endpoint 3 Descriptor
	0x07,							// bLength
	0x05,							// bDescriptorType = ENDPOINT
	0x03,							// EndpointAddress = EP3/OUT (0x03 | 0x00)
	0x02,							// bmAttributes = Bulk
	WORD2BYTE(0x0020),				// wMaxPacketSize = 64 bytes
	0x00,							// bInterval
	//---------------------------------------------------------------------------
	// 3-2. Endpoint 3 Descriptor
	0x07,							// bLength
	0x05,							// bDescriptorType = ENDPOINT
	0x81,							// EndpointAddress = EP1/IN (0x01 | 0x80)
	0x02,							// bmAttributes = Bulk
	WORD2BYTE(0x0020),				// wMaxPacketSize = 64 bytes
	0x00,							// bInterval
	//---------------------------------------------------------------------------
	// 4.Device Qualifier Descriptor
	0x0A,							// bLength = 10
	0x06,							// bDescriptorType = DeviceQualifier(0x06)
	WORD2BYTE(0x200),				// bcdUSB
	0x00,							// bDeviceClass
	0x00,							// bDeviceSubClass
	0x00,							// bDeviceProtocol
	0x40,							// bMaxPacketSize0
	0x01,							// bNumConfigurations
	0x00,							// Reserved
	//---------------------------------------------------------------------------
	// 5. String Descriptor
	0x04,
	0x03,
	WORD2BYTE(0x0409),				// LANGID = English US(0x0409)
};
#pragma pack()

static size_t sUsbDescriptorSize=0;
static uint8_t sUsbCommunicationDeviceDescriptor[256];


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _usb_control_transfer_cb(UsbDeviceRequest *req);
static size_t _append_string_desc(const char *str);

int initialize_usb(const char *serial_number, const char *cdc_interface_name)
{
	size_t descriptor_size = 0;
	memcpy(sUsbCommunicationDeviceDescriptor, cUsbCommunicationDeviceDescriptor, sizeof(cUsbCommunicationDeviceDescriptor));
	sUsbDescriptorSize = sizeof(cUsbCommunicationDeviceDescriptor);
	descriptor_size = sUsbDescriptorSize;

	descriptor_size = _append_string_desc("aks3g");
	descriptor_size = _append_string_desc("DDC Analyzer");
	descriptor_size = _append_string_desc(serial_number);
	descriptor_size = _append_string_desc(cdc_interface_name);

	samd51_usb_device_initialize();
	samd51_usb_setup_device(sUsbCommunicationDeviceDescriptor, descriptor_size, _usb_control_transfer_cb);

	usbCdcInitialize(2, 1, 3);

	samd51_usb_device_attach(1);

	return AI_OK;
	
}

static void _usb_control_transfer_cb(UsbDeviceRequest *req)
{
	usbCdc_control(req);
	
	return;
}

static size_t _append_string_desc(const char *str)
{
	uint8_t *p = sUsbCommunicationDeviceDescriptor;

	p[sUsbDescriptorSize++] = strlen(str) * 2 + 2;
	p[sUsbDescriptorSize++] = 3; // String Descriptor
	
	size_t len = strlen(str);
	for (size_t i=0 ; i<len ; ++i) {
		p[sUsbDescriptorSize++] = str[i];
		p[sUsbDescriptorSize++] = 0;
	}
	return sUsbDescriptorSize;	
}
