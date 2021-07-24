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
//J USB Device Request の定義
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
// USBの各種転送完了時のコールバック関数類
/*--------------------------------------------------------------------------*/
//J Control 転送が入った後の処理を記述
typedef void (*UsbControlTansferCallback)(UsbDeviceRequest *req);

//J OUT Transfer 割り込みが入ったときの処理
typedef void (*UsbOutTranferDoneCb)(uint8_t *buf, uint32_t size);

//J IN Transfer 割り込みが入ったときの処理
typedef void (*UsbInTransferDoneCb)(int error);

//J Hostからの切断時に呼ばれる処理
typedef void (*UsbCleanupCb)(void);

//J Wakeup時に呼ばれる処理
typedef void (*UsbWakeupCb)(void);

//J USB Resetがかかったときに呼ばれる処理
typedef void (*UsbResetCb)(void);

/*--------------------------------------------------------------------------*/
// USBデバイス実装用関数
/*--------------------------------------------------------------------------*/
//J MCU内のUSBモジュールを初期化する
int samd51_usb_device_initialize(void);
void samd51_usb_finalize(void);

//J HOSTとの切断時に呼ばれる関数を登録する
int samd51_register_cleanup_func(UsbCleanupCb cb);

//J USB Wakeup時に呼ばれる関数を登録する
int samd51_register_wakeup_func(UsbWakeupCb cb);

//J USBデバイスをバスに接続する
int samd51_usb_device_attach(int attach);

//J デバイスを登録する
int samd51_usb_setup_device(const uint8_t *desc, size_t desc_len, UsbControlTansferCallback class_request_cb);

//J Endpointを初期化する. IN/OUTで別関数
int samd51_usb_device_setup_IN_endpoint (int ep, Samd51UsbEpType type, UsbInTransferDoneCb in_cb, uint8_t *default_buf, size_t default_buf_len);
int samd51_usb_device_setup_OUT_endpoint(int ep, Samd51UsbEpType type, UsbOutTranferDoneCb out_cb, uint8_t *default_buf, size_t default_buf_len);

//J 次のINトークンで流す情報を登録する
int samd51_usb_transfer_bulk_in(int ep, void *buf, size_t len);
int samd51_usb_transfer_bulk_in_with_own_buf(int ep, void *buf, size_t len);

//J 次のOUT転送を許可する
int samd51_usb_transfer_bulk_out(int ep);

//J Control INは別関数で動かす
int samd51_usb_transfer_control_in(void *buf, size_t len);

//J IN Transferの制御権管理
void samd51_usb_lock_in_transfer(void);
void samd51_usb_unlock_in_transfer(void);


int samd51_usb_device_register_reset_callback(UsbResetCb resetCb);


#endif /* SAMD51_USB_DEVICE_H_ */