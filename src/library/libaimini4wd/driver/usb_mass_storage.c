/*
 * usb_mass_storage.c
 *
 * Created: 2019/07/08
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */

#include <stdint.h>
#include <string.h>

#include <samd51_error.h>

#include "samd51_usb_device.h"
#include "qspi_diskio.h"
#include "scsi.h"

#include "usb_mass_storage.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#define CBW_SIGNATURE						(0x43425355)
#define CSW_SIGNATURE						(0x53425355)

#define CB_STATUS_PASSED					(0x00)
#define CB_STATUS_FAILED					(0x01)
#define CB_STATUS_PHASE_ERROR				(0x02)


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef enum MassStorageCommandProtocolState_t
{
	cStateReady = 0,
	cStateGotCBD = 1,
	cStateWaitForInDataReady = 2,
	cStateWaitDoneDataInTransfer = 3,
	cStateWaitDoneDataOutTransfer = 4,
	cStateWaitForOutDataProcessed = 5,
	cStateTriggerStatusTransport = 6,
	cStateWaitForDoneStatusTransport = 7
} MassStorageCommandProtocolState;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CBW
#pragma pack(1)
typedef struct CommandBlockWrapper_t
{
	uint32_t dCBWsignature;
	uint32_t dCBWTag;
	uint32_t dCBWDataTransferLength;
	union {
		uint8_t byte;
		struct {
			uint8_t reserved  : 6;
			uint8_t obsolete  : 1;
			uint8_t direction : 1;
		}bm;
	} bmCBWFlags;
	uint8_t  bCBWLNU;
	uint8_t  bCBWCBLength;
	uint8_t  CBWB[16];
} CBW;
#pragma pack()

/*--------------------------------------------------------------------------*/
// CSW
#pragma pack(1)
typedef struct CommandStatusWrapper_t
{
	uint32_t dCSWSignature;
	uint32_t dCSWTag;
	uint32_t dCSWDataResidue;
	uint8_t  bCSWStatus;
} CSW;
#pragma pack()

/*--------------------------------------------------------------------------*/
typedef struct UsbMassStorageContext_t
{
	MassStorageCommandProtocolState protocol_state;
	CBW current_cbw;
	
	uint8_t *out_buf;
	size_t   out_len;
	
	int in_ep;
	int out_ep;

} UsbMassStorageContext;


/*--------------------------------------------------------------------------*/
static UsbMassStorageContext sUmssCtx = {0};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void _usb_mass_storage_send_csw(int ep, uint32_t tag);

static void _uab_mass_request_bulk_only_mass_storage_reset(UsbDeviceRequest *req);
static void _uab_mass_request_get_max_lun(UsbDeviceRequest *req);

static void _usb_mass_storage_bulk_only_out(uint8_t *buf, uint32_t size);
static void _usb_mass_storage_bulk_only_in(int error);

static void _onUsbReset(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static uint8_t sBulkOutBuf[64];
static uint8_t sBulkInBuf[64];

/*--------------------------------------------------------------------------*/
int usbMassStorageInitialize(int in_ep, int out_ep)
{
	//J Init SD
	qspi_disk_initialize();
	
	//J Setup USB
	scsiInitialize(1);

	//J Setup USB Device
	samd51_usb_device_register_reset_callback(_onUsbReset);
	
	samd51_usb_device_setup_IN_endpoint (in_ep,  cEpBulk, _usb_mass_storage_bulk_only_in,  sBulkInBuf, sizeof(sBulkInBuf));
	samd51_usb_device_setup_OUT_endpoint(out_ep, cEpBulk, _usb_mass_storage_bulk_only_out, sBulkOutBuf, sizeof(sBulkOutBuf));

	sUmssCtx.in_ep = in_ep;
	sUmssCtx.out_ep = out_ep;

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static uint8_t s_scsi_parse_buf[4096]; //TODO

/*--------------------------------------------------------------------------*/
int usbMassStorageUpdate(void)
{
	if (sUmssCtx.protocol_state == cStateWaitForInDataReady) {
		size_t response_size = 0;
		int8_t ret = scsiParseCommands(sUmssCtx.current_cbw.CBWB, sUmssCtx.current_cbw.bCBWCBLength, s_scsi_parse_buf, sizeof(s_scsi_parse_buf), &response_size);
		//J 継続転送でない限りは次の転送完了後にStatus Transportを実施する
		if (ret != SCSI_ERROR_CONTINUE) {
			sUmssCtx.protocol_state = cStateTriggerStatusTransport;
		}
		else {
			sUmssCtx.protocol_state = cStateWaitDoneDataInTransfer;
		}

		if ((ret == SCSI_OK) || (ret == SCSI_ERROR_CONTINUE) || (ret == SCSI_ERROR_NOT_READY)) {
			samd51_usb_transfer_bulk_in_with_own_buf(sUmssCtx.in_ep, s_scsi_parse_buf, response_size);
		}
	}
	else if (sUmssCtx.protocol_state == cStateWaitForOutDataProcessed) {
		size_t response_size = 0;
		//J EPサイズの変更に対してロバストでない
		int8_t ret = scsiParseCommands(sUmssCtx.current_cbw.CBWB, sUmssCtx.current_cbw.bCBWCBLength, sBulkOutBuf, sizeof(sBulkOutBuf), &response_size);

		//J 継続転送出ない限りは次の転送完了後にStatus Transportを実施する
		if (ret != SCSI_ERROR_CONTINUE) {
			sUmssCtx.protocol_state = cStateTriggerStatusTransport;
		}
		else {
			sUmssCtx.protocol_state = cStateWaitDoneDataOutTransfer;
		}

		if (ret == SCSI_ERROR_CONTINUE){
			samd51_usb_transfer_bulk_out(sUmssCtx.out_ep); //もっとくれ
		}
		else if (ret == SCSI_OK || ret == SCSI_ERROR_NOT_READY){
			samd51_usb_transfer_bulk_out(sUmssCtx.out_ep); //EPを有効化

			//J CSW Status をセットしておく
			_usb_mass_storage_send_csw(sUmssCtx.in_ep, sUmssCtx.current_cbw.dCBWTag);
		}
		else {
		}
	}

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int usbMassStorageDiskBusy(void)
{
	if (sUmssCtx.protocol_state == cStateReady) {
		return 0;
	}
	else {
		return 1;	
	}
}

/*--------------------------------------------------------------------------*/
static void _usb_mass_storage_send_csw(int ep, uint32_t tag)
{
	CSW csw;
	int status = scsiAnyAttentionIsAvailable();
	
	csw.dCSWSignature = CSW_SIGNATURE;
	csw.dCSWTag = tag;
	csw.dCSWDataResidue = 0; // ?
	csw.bCSWStatus = status ? CB_STATUS_FAILED : CB_STATUS_PASSED;

	sUmssCtx.protocol_state = cStateWaitForDoneStatusTransport;
	samd51_usb_transfer_bulk_in(ep, &csw, sizeof(CSW));
	
	return;
}

/*--------------------------------------------------------------------------*/
/* USB Mass Storage Class用のCTRL転送処理 */
/*--------------------------------------------------------------------------*/
#define USB_MASS_CLASS_REQ_BULK_ONLY_MASS_STORAGE_RESET			(0xff)
#define USB_MASS_CLASS_REQ_GET_MAX_LUN							(0xfe)

/*--------------------------------------------------------------------------*/
static void _uab_mass_request_bulk_only_mass_storage_reset(UsbDeviceRequest *req)
{
	// スグにACKを返したい Status Stage用にEP0INも用意しておく
	samd51_usb_transfer_control_in(NULL, 0);

	return;
}

/*--------------------------------------------------------------------------*/
static void _uab_mass_request_get_max_lun(UsbDeviceRequest *req)
{
	uint8_t lun = 0;
	samd51_usb_transfer_control_in(&lun, req->wLength);

	return;
}

/*--------------------------------------------------------------------------*/
void usbMass_control(UsbDeviceRequest *req)
{
	switch (req->bRequest)
	{
	case USB_MASS_CLASS_REQ_BULK_ONLY_MASS_STORAGE_RESET:
		_uab_mass_request_bulk_only_mass_storage_reset(req);
		break;
	case USB_MASS_CLASS_REQ_GET_MAX_LUN:
		_uab_mass_request_get_max_lun(req);
		break;
	default:
		//J NAK返すべき？
		break;
	}
	return;
}

/*--------------------------------------------------------------------------*/
/* USB Mass StorageクラスのIN/OUT転送完了時の処理 */
/*--------------------------------------------------------------------------*/
static void _usb_mass_storage_bulk_only_out(uint8_t *buf, uint32_t size)
{
	if (sUmssCtx.protocol_state == cStateReady) {
		//J CBW コマンドが来ることが想定されるので、パースする
		CBW *cbw = (CBW *)buf;
		if (cbw->dCBWsignature != CBW_SIGNATURE) {
			return;
		}
		else {
			memcpy(&sUmssCtx.current_cbw, cbw, sizeof(CBW));
		}

		//J IN (to host)
		if (cbw->bmCBWFlags.bm.direction == USB_IN) {
			sUmssCtx.protocol_state = cStateWaitForInDataReady; // 割込みハンドラ以外の部分で頑張ってもらう
		}
		//J OUT (from host)
		else {
			//J 死にたい -> そもそもIN/OUT関係なく、即Statusを返す系コマンドかどうかだけでも知るべきかも？
			uint8_t more_transfer = scsiCheckMoreTransfer(cbw->CBWB, cbw->bCBWCBLength);
			if (more_transfer){
				sUmssCtx.protocol_state = cStateWaitDoneDataOutTransfer;
			}
			else {
				_usb_mass_storage_send_csw(sUmssCtx.in_ep, sUmssCtx.current_cbw.dCBWTag);
			}
		}
	}
	else if (sUmssCtx.protocol_state == cStateWaitDoneDataOutTransfer) {
		// Mainループで受信した内容を処理する
		sUmssCtx.out_buf = buf;
		sUmssCtx.out_len = size;
		sUmssCtx.protocol_state = cStateWaitForOutDataProcessed;

		//J このケースだけは、メインループで処理されるまで次のOUTを受け付けない
		return;
	}
	else if (sUmssCtx.protocol_state == cStateTriggerStatusTransport) {
		_usb_mass_storage_send_csw(sUmssCtx.in_ep, sUmssCtx.current_cbw.dCBWTag);
	}

	samd51_usb_transfer_bulk_out(sUmssCtx.out_ep);

	return;
}

/*--------------------------------------------------------------------------*/
static void _usb_mass_storage_bulk_only_in(int error)
{
	if (error != AI_OK) {
		sUmssCtx.protocol_state = cStateReady;
	}
	if (sUmssCtx.protocol_state == cStateTriggerStatusTransport) {
		_usb_mass_storage_send_csw(sUmssCtx.in_ep, sUmssCtx.current_cbw.dCBWTag);
	}
	else if (sUmssCtx.protocol_state == cStateWaitDoneDataInTransfer) {
		sUmssCtx.protocol_state = cStateWaitForInDataReady;
	}
	else if (sUmssCtx.protocol_state == cStateWaitForDoneStatusTransport) {
		sUmssCtx.protocol_state = cStateReady;
	}
	return;
}


static void _onUsbReset(void) {
	scsiInitialize(0);
	sUmssCtx.protocol_state = cStateReady;

	memset (&sUmssCtx.current_cbw, 0, sizeof(sUmssCtx.current_cbw));
}