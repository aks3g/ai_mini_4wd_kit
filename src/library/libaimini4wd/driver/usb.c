/*
 * usb.c
 *
 * Created: 2020/07/03 15:54:51
 *  Author: kiyot
 */ 
#include <stdint.h>
#include <string.h>

#include <samd51_error.h>
#include <samd51_usb_device.h>

#include "usb_cdc.h"
#include "usb_mass_storage.h"


#define WORD2BYTE(w)	((w>>0)&0xff), ((w>>8)&0xff)
#pragma pack(1)
uint8_t cUsbCommunicationDeviceDescriptor[] =
{
	//---------------------------------------------------------------------------
	// 0. Device Descriptor
	0x12,							// bLength
	0x01,							// bDescriptor Type = DEVICE(0x01)
	WORD2BYTE(0x200),				// bcdUSB
	0xEF,							// bDeviceClass = USB_DEVICE_CLASS_CODE_CDC
	0x02,							// bDeviceSubClass = Common Class(0x02)
	0x01,							// bDeviceProtocol = IAD(0x01)
	0x40,							// bMaxPacketSize0 = 64byte
	WORD2BYTE(0x04D8),				// idVender
	WORD2BYTE(0xE836),				// idProduct
	WORD2BYTE(0x0001),				// bcdDevice
	0x00,							// iManufacturer
	0x00,							// iProduct
	0x00,							// iSerialNumber
	0x01,							// bNumConfigurations = 1
	//---------------------------------------------------------------------------
	// 1. Configuration Descriptor
	0x09,							// bLength
	0x02,							// bDescriptorType = CONFIGURATION(0x02)
	WORD2BYTE(0x0074),				// wTotalLength = 75
	0x03,							// bNumInterfaces = 2
	0x01,							// bConfigurationValue = 1
	0x00,							// iConfiguration = ?
	0xC0,							// bmAttributes = 0xC0(Self-powered)
	0x00,							// MaxPower = 0 (= 0mA)
	//---------------------------------------------------------------------------
	// 1-1. IAD
	0x08,							// bLength
	0x0B,							// bDescriptorType = IAD
	0x00,							// bFirstInterface = IF[0]
	0x02,							// bInterfaceCount = 2
	USB_DEVICE_CLASS_CODE_CDC,		// bFunctionClass = CDC
	USB_CDC_SUBCLASS_ACM,			// bFunctionSubClass = ACM
	0x00,							// bFunctionProtocol = Unknown
	0x00,							// iFunction = Unknown
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
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	// 4-1. IAD
	0x08,							// bLength
	0x0B,							// bDescriptorType = IAD
	0x02,							// bFirstInterface = IF[2]
	0x01,							// bInterfaceCount = 1
	0x08,							// bInterfaceClass = MASS STORAGE Class(0x08)
	0x06,							// bInterfaceSubClass = SCSI transparent command set(0x06)
	0x50,							// bInterfaceProtocol = BULK-ONLY TRANSPORT
	0x00,							// iFunction = Unknown
	//---------------------------------------------------------------------------
	// 5. Interface Descriptor
	0x09,							// bLength
	0x04,							// bDescriptorType = INTERFACE(0x04)
	0x02,							// bInterfaceNumber = 0x00
	0x00,							// bAlternateSetting = 0x00
	0x02,							// bNumEndpoints = 2(IN/OUT)
	0x08,							// bInterfaceClass = MASS STORAGE Class(0x08)
	0x06,							// bInterfaceSubClass = SCSI transparent command set(0x06)
	0x50,							// bInterfaceProtocol = BULK-ONLY TRANSPORT
	0x00,							// iInterface
	//---------------------------------------------------------------------------
	// 6. Bulk-in Endpoint
	0x07,							// bLength = 0x07
	0x05,							// bDescriptorType = ENDPOINT(0x05)
	0x84,							// bEndpointAddress = EP4/IN (0x04 | 0x80)
	0x02,							// bmAttributes = Bulk endpoint(0x02)
	WORD2BYTE(0x0040),				// wMaxPacketSize = 64bytes
	0x00,							// bInterval = 0x00 Do not apply to Bulk endpoint
	//---------------------------------------------------------------------------
	// 7. Bulk-out Endpoint
	0x07,							// bLength = 0x07
	0x05,							// bDescriptorType = ENDPOINT(0x05)
	0x05,							// bEndpointAddress = EP5/OUT (0x05 | 0x00)
	0x02,							// bmAttributes = Bulk endpoint(0x02)
	WORD2BYTE(0x0040),				// wMaxPacketSize = 64bytes
	0x00,							// bInterval = 0x00 Do not apply to Bulk endpoint
	//---------------------------------------------------------------------------
	// 8. Device Qualifier Descriptor
	0x0A,							// bLength = 10
	0x06,							// bDescriptorType = DeviceQualifier(0x06)
	WORD2BYTE(0x200),				// bcdUSB
	0xEF,							// bDeviceClass
	0x02,							// bDeviceSubClass
	0x01,							// bDeviceProtocol
	0x40,							// bMaxPacketSize0
	0x01,							// bNumConfigurations
	0x00							// Reserved


};
#pragma pack()

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _usb_control_transfer_cb(UsbDeviceRequest *req);

int initialize_usb(void)
{
	samd51_usb_device_initialize();
	samd51_usb_setup_device(cUsbCommunicationDeviceDescriptor, sizeof(cUsbCommunicationDeviceDescriptor), _usb_control_transfer_cb);

	usbCdcInitialize(2, 1, 3);
	usbMassStorageInitialize(4, 5);

	samd51_usb_device_attach(1);

	return AI_OK;
	
}


static void _usb_control_transfer_cb(UsbDeviceRequest *req)
{
	usbCdc_control(req);
	usbMass_control(req);
	
	return;
}
