/*
 * samd51_usb_device.h
 *
 * Created: 2019/06/16
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */

#ifndef SAMD51_USB_DEVICE_H_
#define SAMD51_USB_DEVICE_H_

#include <stdint.h>
#include <stddef.h>

#define AI_ERROR_USB_FAILED				(0xd5150001)

/*---------------------------------------------------------------------------*/
//J USB Device Request �̒�`
/*---------------------------------------------------------------------------*/
typedef struct UsbDeviceRequest_t
{
	union {
		uint8_t byte;
		struct {
			uint8_t target    : 5;
			uint8_t type      : 2;
			uint8_t direction : 1;
		}bm;
	} bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} UsbDeviceRequest;

#define USB_OUT								(0)
#define USB_IN								(1)

#define UsbBmRequestDirHostToDevice			(0)
#define UsbBmRequestDirDeviceToHost			(1)

#define UsbBmRequestTypeStandard			(0)
#define UsbBmRequestTypeClass				(1)
#define UsbBmRequestTypeVender				(2)

#define UsbBmRequestTargetDevice			(0)
#define UsbBmRequestTargetInterface			(1)
#define UsbBmRequestTargetEndpoint			(2)
#define UsbBmRequestTargetOther				(3)

/*---------------------------------------------------------------------------*/
typedef enum Samd51UsbEpType_t
{
	cEpDisabled = 0,
	cEpControl = 1,
	cEpIsochronous = 2,
	cEpBulk = 3,
	cEpInterrupt = 4,
	cEpDualBank = 5
} Samd51UsbEpType;


/*--------------------------------------------------------------------------*/
// USB�̊e��]���������̃R�[���o�b�N�֐���
/*--------------------------------------------------------------------------*/
//J Control �]������������̏������L�q
typedef void (*UsbControlTansferCallback)(UsbDeviceRequest *req);

//J OUT Transfer ���荞�݂��������Ƃ��̏���
typedef void (*UsbOutTranferDoneCb)(uint8_t *buf, uint32_t size);

//J IN Transfer ���荞�݂��������Ƃ��̏���
typedef void (*UsbInTransferDoneCb)(int error);

//J Host����̐ؒf���ɌĂ΂�鏈��
typedef void (*UsbCleanupCb)(void);

//J Wakeup���ɌĂ΂�鏈��
typedef void (*UsbWakeupCb)(void);

//J USB Reset�����������Ƃ��ɌĂ΂�鏈��
typedef void (*UsbResetCb)(void);

/*--------------------------------------------------------------------------*/
// USB�f�o�C�X�����p�֐�
/*--------------------------------------------------------------------------*/
//J MCU����USB���W���[��������������
int samd51_usb_device_initialize(void);
void samd51_usb_finalize(void);

//J HOST�Ƃ̐ؒf���ɌĂ΂��֐���o�^����
int samd51_register_cleanup_func(UsbCleanupCb cb);

//J USB Wakeup���ɌĂ΂��֐���o�^����
int samd51_register_wakeup_func(UsbWakeupCb cb);

//J USB�f�o�C�X���o�X�ɐڑ�����
int samd51_usb_device_attach(int attach);

//J �f�o�C�X��o�^����
int samd51_usb_setup_device(const uint8_t *desc, size_t desc_len, UsbControlTansferCallback class_request_cb);

//J Endpoint������������. IN/OUT�ŕʊ֐�
int samd51_usb_device_setup_IN_endpoint (int ep, Samd51UsbEpType type, UsbInTransferDoneCb in_cb, uint8_t *default_buf, size_t default_buf_len);
int samd51_usb_device_setup_OUT_endpoint(int ep, Samd51UsbEpType type, UsbOutTranferDoneCb out_cb, uint8_t *default_buf, size_t default_buf_len);

//J ����IN�g�[�N���ŗ�������o�^����
int samd51_usb_transfer_bulk_in(int ep, void *buf, size_t len);
int samd51_usb_transfer_bulk_in_with_own_buf(int ep, void *buf, size_t len);

//J ����OUT�]����������
int samd51_usb_transfer_bulk_out(int ep);

//J Control IN�͕ʊ֐��œ�����
int samd51_usb_transfer_control_in(void *buf, size_t len);

//J IN Transfer�̐��䌠�Ǘ�
void samd51_usb_lock_in_transfer(void);
void samd51_usb_unlock_in_transfer(void);


int samd51_usb_device_register_reset_callback(UsbResetCb resetCb);


#endif /* SAMD51_USB_DEVICE_H_ */