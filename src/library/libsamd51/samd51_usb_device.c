/*
 * samd51_usb_device.c
 *
 * Created: 2019/06/16
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <sam.h>

#include <samd51_error.h>

#include "samd51_usb_device.h"

#pragma pack(1)
typedef struct REG_USB_GENERAL_DEVICE_t
{
	// Common device
	uint8_t ctrla;			// 0x00
	uint8_t reserved;		// 0x01
	uint8_t syncbusy;		// 0x02
	uint8_t qosctrl;		// 0x03
	// for device
	uint8_t reserved2[4];	// 0x04 - 0x07
	uint16_t ctrlb;			// 0x08 - 0x09
	uint8_t dadd;			// 0x0A
	uint8_t reserved3;		// 0x0B
	uint8_t status;			// 0x0C
	// Common device
	uint8_t fsmstatus;		// 0x0D
	// for device
	uint8_t reserved4[2];	// 0x0E - 0x0F
	uint16_t fnum;			// 0x10 - 0x11
	uint8_t reserved5;		// 0x12
	uint8_t padding;		// 0x13
	uint16_t intenclr;		// 0x14 - 0x15
	uint8_t reserved6[2];	// 0x16 - 0x17
	uint16_t intenset;		// 0x18 - 0x19
	uint8_t reserved7[2];	// 0x1A - 0x1B
	uint16_t intflag;		// 0x1C - 0x1D
	uint8_t reserved8[2];	// 0x1E - 0x1F
	uint16_t epintsmry;		// 0x20 - 0x21
	uint8_t reserved9[2];	// 0x22 - 0x23
	// Common device
	uint32_t descadd;		// 0x24 - 0x27
	uint16_t padcal;		// 0x28 - 0x29
	uint8_t padding1[0xff - 0x29];
} REG_USB_GENERAL_DEVICE;
#pragma pack()

#pragma pack(1)
typedef struct REG_USB_DEVICE_EP_t
{
	uint8_t epcfg;
	uint8_t reserved[3];
	uint8_t epstatusclr;
	uint8_t epstatusset;
	uint8_t epstatus;
	uint8_t epintflag;
	uint8_t epintenclr;
	uint8_t epintenset;
	uint8_t reserved2[2];
	uint8_t padding[20];
} REG_USB_DEVICE_EP;
#pragma pack()


#define SAMD51_USB_EP_SIZE_8B							(0)
#define SAMD51_USB_EP_SIZE_16B							(1)
#define SAMD51_USB_EP_SIZE_32B							(2)
#define SAMD51_USB_EP_SIZE_64B							(3)
#define SAMD51_USB_EP_SIZE_128B							(4)
#define SAMD51_USB_EP_SIZE_256B							(5)
#define SAMD51_USB_EP_SIZE_512B							(6)
#define SAMD51_USB_EP_SIZE_1023B						(7)

#pragma pack(1)
typedef struct REG_USB_DEVICE_EP_DESCRIPTOR_t
{
	volatile uint32_t addr;
	union{
		struct {
			uint32_t byte_count : 14;
			uint32_t multi_packet_size : 14;
			uint32_t size : 3;
			uint32_t auto_zlp : 1;
		} bf;
		uint32_t dword;
	} pcksize;
	uint16_t extreg;
	uint8_t status_bk;
	uint8_t reserved[5];
} REG_USB_DEVICE_EP_DESCRIPTOR;
#pragma pack()


#pragma pack(1)
typedef struct REG_USB_DEVICE_t
{
	REG_USB_GENERAL_DEVICE reg;
	REG_USB_DEVICE_EP ep[8];
} REG_USB_DEVICE;
#pragma pack()


#define USB_REG					(*(volatile REG_USB_DEVICE *)0x41000000UL)

static volatile REG_USB_DEVICE_EP_DESCRIPTOR sEpDesc[8][2];
static volatile uint8_t sEp0OutDatapool[128];
static volatile uint8_t sEp0InDatapool[128];

#define SAMD51_USB_CTRLA_SWRST					(1 << 0)
#define SAMD51_USB_CTRLA_ENABLE					(1 << 1)
#define SAMD51_USB_CTRLA_RUNSTBY				(1 << 2)
#define SAMD51_USB_CTRLA_MODE					(1 << 7)

#define SAMD51_USB_SYNCBUSY_SWRST				(1 << 0)
#define SAMD51_USB_SYNCBUSY_ENABLE				(1 << 1)

#define SAMD51_USB_QOSCTRL_CQOS_pos				(0)
#define SAMD51_USB_QOSCTRL_DQOS_pos				(2)

#define SAMD51_USB_CTRLB_DETACH					(1 << 0)
#define SAMD51_USB_CTRLB_UDRSM					(1 << 1)
#define SAMD51_USB_CTRLB_SPDCONF_pos			(1 << 2)
#define SAMD51_USB_CTRLB_NREPLY					(1 << 4)
#define SAMD51_USB_CTRLB_GNAK					(1 << 9)
#define SAMD51_USB_CTRLB_LPMHDSK_pos			(1 << 10)

#define SAMD51_USB_DADD_ADDR_MASK				(0x7f)
#define SAMD51_USB_DADD_ADDEN					(1 << 7)

#define SAMD51_USB_STATUS_SPEED_pos				(2)
#define SAMD51_USB_STATUS_SPEED_mask			(3 << SAMD51_USB_STATUS_SPEED_pos)
#define SAMD51_USB_STATUS_LINESTATE_pos			(6)
#define SAMD51_USB_STATUS_LINESTATE_mask		(3 << SAMD51_USB_STATUS_LINESTATE_pos)

#define SAMD51_USB_FNUM_FNUM_pos				(3)
#define SAMD51_USB_FNUM_FNUM_mask				(0x7ff << SAMD51_USB_FNUM_FNUM_pos)
#define SAMD51_USB_FNUM_FNCERR					(1 << 15)

#define SAMD51_USB_INT_SUSPEND					(1 << 0)
#define SAMD51_USB_INT_SOF						(1 << 2)
#define SAMD51_USB_INT_EORST					(1 << 3)
#define SAMD51_USB_INT_WAKEUP					(1 << 4)
#define SAMD51_USB_INT_EORSM					(1 << 5)
#define SAMD51_USB_INT_UPRSM					(1 << 6)
#define SAMD51_USB_INT_RAMACER					(1 << 7)
#define SAMD51_USB_INT_LPMNYET					(1 << 8)
#define SAMD51_USB_INT_LPMSUSP					(1 << 9)

#define SAMD51_USB_EP_EPCFG_EPTYPE_IN_pos		(4)
#define SAMD51_USB_EP_EPCFG_EPTYPE_OUT_pos		(0)

#define SAMD51_USB_EP_EPSTATUS_DTGLOUT			(1 << 0)
#define SAMD51_USB_EP_EPSTATUS_DTGLIN			(1 << 1)
#define SAMD51_USB_EP_EPSTATUS_CURBK			(1 << 2)
#define SAMD51_USB_EP_EPSTATUS_STALLRQ0			(1 << 4)
#define SAMD51_USB_EP_EPSTATUS_STALLRQ1			(1 << 5)
#define SAMD51_USB_EP_EPSTATUS_BK0RDY			(1 << 6)
#define SAMD51_USB_EP_EPSTATUS_BK1RDY			(1 << 7)

#define SAMD51_USB_EP_EPINTFLAG_TRCPT0			(1 << 0)
#define SAMD51_USB_EP_EPINTFLAG_TRCPT1			(1 << 1)
#define SAMD51_USB_EP_EPINTFLAG_TRFAIL0			(1 << 2)
#define SAMD51_USB_EP_EPINTFLAG_TRFAIL1			(1 << 3)
#define SAMD51_USB_EP_EPINTFLAG_RXSTP			(1 << 4)
#define SAMD51_USB_EP_EPINTFLAG_STALL0			(1 << 5)
#define SAMD51_USB_EP_EPINTFLAG_STALL1			(1 << 6)

#define SAMD51_USB_EP_INTFLAG_STALL				(1 << 5)
#define SAMD51_USB_EP_INTFLAG_RXSTP				(1 << 4)
#define SAMD51_USB_EP_INTFLAG_TRFAIL			(1 << 2)
#define SAMD51_USB_EP_INTFLAG_TRCPT				(1 << 0)

#define SAMD51_USB_PADCAL_TRANSP_pos			(0)
#define SAMD51_USB_PADCAL_TRANSN_pos			(6)
#define SAMD51_USB_PADCAL_TRIM_pos				(12)


/*--------------------------------------------------------------------------*/
// Standard Request Processor
/*--------------------------------------------------------------------------*/
typedef enum UsbStandardCommand_t
{
	cGetStatus			= 0x00,
	cClearFeature		= 0x01,
	cSetFeature			= 0x03,
	cSetAddress			= 0x05,
	cGetDescriptor		= 0x06,
	cSetDescriptor		= 0x07,
	cGetConfiguration	= 0x08,
	cSetConfiguration	= 0x09,
	cGetInterface		= 0x0A,
	cSetInterface		= 0x0B,
	cSynchFrame			= 0x0C
} UsbStandardCommand;

static void _usb_std_request_get_status(UsbDeviceRequest *req);
static void _usb_std_request_clear_feature(UsbDeviceRequest *req);
static void _usb_std_request_set_feature(UsbDeviceRequest *req);
static void _usb_std_request_set_address(UsbDeviceRequest *req);
static void _usb_std_request_get_descriptor(UsbDeviceRequest *req);
static void _usb_std_request_set_descriptor(UsbDeviceRequest *req);
static void _usb_std_request_get_configuration(UsbDeviceRequest *req);
static void _usb_std_request_set_configuration(UsbDeviceRequest *req);
static void _usb_std_request_get_interface(UsbDeviceRequest *req);
static void _usb_std_request_set_interface(UsbDeviceRequest *req);

static void _usb_dispatch_standard_request(UsbDeviceRequest *req);
static void _usb_dispatch_class_request(UsbDeviceRequest *req);
static void _usb_dispatch_vender_request(UsbDeviceRequest *req);

static int _usb_search_descriptor(const uint8_t *descroptor, uint16_t descriptor_len, uint8_t type, uint8_t index, uint16_t required_len, uint8_t **found_desc, uint16_t *len);


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct Samd51UsbDeviceContext_t
{
	const uint8_t *desc;
	size_t desc_len;
	UsbControlTansferCallback class_request_cb;
	UsbControlTansferCallback vender_request_cb;
	
	uint8_t reserved_device_address;
	uint8_t reserved[3];
} Samd51UsbDeviceContext;
#pragma pack()


static Samd51UsbDeviceContext sCtx;

#pragma pack(1)
typedef struct Samd51UsbEndpointInfo_t
{
	uint8_t enabled;
	uint8_t epcfg;
	UsbOutTranferDoneCb out_cb;
	UsbInTransferDoneCb in_cb;
	
	uint32_t default_buf;	
} Samd51UsbEndpointInfo;
#pragma pack()

static UsbWakeupCb sWakeupCb = NULL;
static UsbCleanupCb sCleanupCb = NULL;
static UsbResetCb sResetCb[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

//J 8-EP x IN/OUT
static Samd51UsbEndpointInfo sUsbEpReservedConfig[8][2];

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
int samd51_usb_device_initialize(void)
{
	static int restart = 0;
	if (restart == 0) {
		memset(&sUsbEpReservedConfig, 0, sizeof(sUsbEpReservedConfig));
		memset(&sCtx, 0x00, sizeof(sCtx));
		memset((void *)sEpDesc, 0x00, sizeof(sEpDesc));
		restart = 1;
	}

	USB_REG.reg.descadd = (uint32_t)sEpDesc;
	
	NVIC_EnableIRQ(USB_0_IRQn);
	NVIC_EnableIRQ(USB_1_IRQn);
	NVIC_EnableIRQ(USB_2_IRQn);
	NVIC_EnableIRQ(USB_3_IRQn);

	NVIC_SetPriority(USB_0_IRQn, 6);
	NVIC_SetPriority(USB_1_IRQn, 6);
	NVIC_SetPriority(USB_2_IRQn, 6);
	NVIC_SetPriority(USB_3_IRQn, 6);


	//J Reset終了時に割込みかけてコンテキストの初期化をやらせる
	//J Suspend時に割込みをかけてDriverのCleanupを実施する
	USB_REG.reg.intenset = SAMD51_USB_INT_EORST | SAMD51_USB_INT_SUSPEND | SAMD51_USB_INT_WAKEUP;

	// Setup EP0
	sEpDesc[0][0].addr = (uint32_t)sEp0OutDatapool;
	sEpDesc[0][1].addr = (uint32_t)sEp0InDatapool;
	
	sEpDesc[0][0].pcksize.bf.size = SAMD51_USB_EP_SIZE_64B;
	sEpDesc[0][1].pcksize.bf.size = SAMD51_USB_EP_SIZE_64B;

	//J キャリブレーションデータの設定
	USB_REG.reg.padcal = (0x6 << 12) | (0x9 << 6) | (0x19 << 0);

	//J Full Speed設定
	USB_REG.reg.status |= (1 << 2);

	//J Enable USB Device
	USB_REG.reg.ctrla |=  (SAMD51_USB_CTRLA_ENABLE);
	while ((USB_REG.reg.syncbusy & SAMD51_USB_SYNCBUSY_ENABLE) == 0);
	
	return AI_OK;
}

/*--------------------------------------------------------------------------*/
void samd51_usb_finalize(void)
{
	NVIC_DisableIRQ(USB_0_IRQn);
	NVIC_DisableIRQ(USB_1_IRQn);
	NVIC_DisableIRQ(USB_2_IRQn);
	NVIC_DisableIRQ(USB_3_IRQn);
	
	USB_REG.reg.ctrla = SAMD51_USB_CTRLA_SWRST;
	while(USB_REG.reg.syncbusy & SAMD51_USB_CTRLA_SWRST);
}


/*---------------------------------------------------------------------------*/
int samd51_usb_setup_device(const uint8_t *desc, size_t desc_len, UsbControlTansferCallback class_request_cb)
{
	sCtx.desc = desc;
	sCtx.desc_len = desc_len;

	sCtx.class_request_cb = class_request_cb;
	
	return AI_OK;	
}

/*---------------------------------------------------------------------------*/
int samd51_usb_device_setup_IN_endpoint (int ep, Samd51UsbEpType type, UsbInTransferDoneCb in_cb, uint8_t *default_buf, size_t default_buf_len)
{
	if (ep >= 8 || ep < 1) {
		return AI_ERROR_INVALID;
	}

	if (default_buf_len < 64) {
		return AI_ERROR_NOBUF;
	}

	//J EPの状態はUSB Resetで初期化されるので一旦RAMで保持する
	sUsbEpReservedConfig[ep][USB_IN].enabled = 1;
	sUsbEpReservedConfig[ep][USB_IN].epcfg = (type << SAMD51_USB_EP_EPCFG_EPTYPE_IN_pos);
	sUsbEpReservedConfig[ep][USB_IN].out_cb = NULL;
	sUsbEpReservedConfig[ep][USB_IN].in_cb = in_cb;

	sUsbEpReservedConfig[ep][USB_IN].default_buf = (uint32_t)default_buf;

	sEpDesc[ep][USB_IN].addr = (uint32_t)default_buf;
	sEpDesc[ep][USB_IN].pcksize.bf.size = SAMD51_USB_EP_SIZE_64B; //Full Speed固定なのでこれでOK
	sEpDesc[ep][USB_IN].pcksize.bf.auto_zlp = 1;

	return AI_OK;
}

int samd51_usb_device_setup_OUT_endpoint(int ep, Samd51UsbEpType type, UsbOutTranferDoneCb out_cb, uint8_t *default_buf, size_t default_buf_len)
{
	if (ep >= 8 || ep < 1) {
		return AI_ERROR_INVALID;
	}

	if (default_buf_len < 64) {
		return AI_ERROR_NOBUF;
	}

	//J EPの状態はUSB Resetで初期化されるので一旦RAMで保持する
	sUsbEpReservedConfig[ep][USB_OUT].enabled = 1;
	sUsbEpReservedConfig[ep][USB_OUT].epcfg = (type << SAMD51_USB_EP_EPCFG_EPTYPE_OUT_pos);
	sUsbEpReservedConfig[ep][USB_OUT].out_cb = out_cb;
	sUsbEpReservedConfig[ep][USB_OUT].in_cb = NULL;

	sUsbEpReservedConfig[ep][USB_OUT].default_buf = (uint32_t)default_buf;


	sEpDesc[ep][USB_OUT].addr = (uint32_t)default_buf;
	sEpDesc[ep][USB_OUT].pcksize.bf.size = SAMD51_USB_EP_SIZE_64B; //Full Speed固定なのでこれでOK

	return AI_OK;	
}

/*---------------------------------------------------------------------------*/
int samd51_register_cleanup_func(UsbCleanupCb cb)
{
	sCleanupCb = cb;
	return AI_OK;
}

/*---------------------------------------------------------------------------*/
int samd51_register_wakeup_func(UsbWakeupCb cb)
{
	sWakeupCb = cb;
	return AI_OK;
}

/*---------------------------------------------------------------------------*/
int samd51_usb_device_register_reset_callback(UsbResetCb resetCb)
{
	for (int i=0 ; i<(sizeof(sResetCb)/sizeof(sResetCb[0])) ; ++i) {
		if (sResetCb[i] == NULL) {
			sResetCb[i] = resetCb;
			return AI_OK;
		}
	}

	return AI_ERROR_NOBUF;
}



/*---------------------------------------------------------------------------*/
int samd51_usb_device_attach(int attach)
{
	if (attach) {
		USB_REG.reg.ctrlb &= ~(SAMD51_USB_CTRLB_DETACH);
	}
	else {
		USB_REG.reg.ctrlb |=  (SAMD51_USB_CTRLB_DETACH);
	}

	return AI_OK;
}

/*---------------------------------------------------------------------------*/
int samd51_usb_transfer_control_in(void *buf, size_t len)
{
	if (buf != NULL && len > 0) {
		memcpy((void *)sEpDesc[0][1].addr, buf, len);
	}
	sEpDesc[0][1].pcksize.bf.multi_packet_size = 0;
	sEpDesc[0][1].pcksize.bf.byte_count = len;

	USB_REG.ep[0].epintflag = 0xff;
	USB_REG.ep[0].epstatusset = SAMD51_USB_EP_EPSTATUS_BK1RDY;

	return AI_OK;	
}

/*---------------------------------------------------------------------------*/
int samd51_usb_transfer_bulk_in(int ep, void *buf, size_t len)
{
	if (ep < 1 || ep >= 8) {
		return AI_ERROR_INVALID;
	}
	if (buf == NULL) {
		return AI_ERROR_NULL;
	}
	if (len > 64) {
		return AI_ERROR_NOBUF;
	}

	memcpy((void *)sUsbEpReservedConfig[ep][1].default_buf, buf, len);
	sEpDesc[ep][1].addr = sUsbEpReservedConfig[ep][1].default_buf;
	sEpDesc[ep][1].pcksize.bf.multi_packet_size = 0;
	sEpDesc[ep][1].pcksize.bf.byte_count = len;
	sEpDesc[ep][1].pcksize.bf.auto_zlp = 1;

	USB_REG.ep[ep].epintflag = 0xff;
	USB_REG.ep[ep].epstatusset = SAMD51_USB_EP_EPSTATUS_BK1RDY;

	return AI_OK;
}

/*---------------------------------------------------------------------------*/
int samd51_usb_transfer_bulk_in_with_own_buf(int ep, void *buf, size_t len)
{
	if (ep < 1 || ep >= 8) {
		return AI_ERROR_INVALID;
	}
	if (buf == NULL) {
		return AI_ERROR_NULL;
	}

	sEpDesc[ep][1].addr = (uint32_t)buf;
	sEpDesc[ep][1].pcksize.bf.multi_packet_size = 0;
	sEpDesc[ep][1].pcksize.bf.byte_count = len;
		
	sEpDesc[ep][1].pcksize.bf.auto_zlp = 0;

	USB_REG.ep[ep].epintflag = 0xff;
	USB_REG.ep[ep].epstatusset = SAMD51_USB_EP_EPSTATUS_BK1RDY;

	return AI_OK;
}





/*---------------------------------------------------------------------------*/
int samd51_usb_transfer_bulk_out(int ep)
{
	USB_REG.ep[ep].epstatusclr = SAMD51_USB_EP_EPSTATUS_BK0RDY;
	
	return AI_OK;
}

/*---------------------------------------------------------------------------*/
void samd51_usb_lock_in_transfer(void)
{
	__disable_irq();
}

/*---------------------------------------------------------------------------*/
void samd51_usb_unlock_in_transfer(void)
{
	__enable_irq();
}


/*---------------------------------------------------------------------------*/
/*
 - USB系の割込み
   USB_EORSM_DNRSM,
   USB_EORST_RST,
   USB_LPMSUSP_DDISC,
   USB_LPM_DCONN,
   USB_MSOF,
   USB_RAMACER,
   USB_SUSPEND,
   USB_UPRSM
   USB_WAKEUP

 - EP系の割込み
   USB_RXSTP_TXSTP_n,
   USB_STALL0_n,
   USB_STALL1_n,
   USB_TRFAIL0_n,
   USB_TRFAIL1_n,
*/


void USB_0_Handler(void)
{
	uint16_t intflag = USB_REG.reg.intflag;
	USB_REG.reg.intflag = 0xffff;

	//J USBリセット終了後の処理
	if (intflag & SAMD51_USB_INT_EORST) {
		int i=0;
		for (int i=0 ; i<(sizeof(sResetCb)/sizeof(sResetCb[0])) ; ++i) {
			if (sResetCb[i] != NULL) {
				sResetCb[i]();
			}
		}

		//J 各種エンドポイントの初期化をここでやっとく
		USB_REG.ep[0].epcfg = (cEpControl << SAMD51_USB_EP_EPCFG_EPTYPE_IN_pos) | (cEpControl << SAMD51_USB_EP_EPCFG_EPTYPE_OUT_pos);
		USB_REG.ep[0].epintenset =  SAMD51_USB_EP_EPINTFLAG_RXSTP | 
									SAMD51_USB_EP_EPINTFLAG_TRFAIL1 | 
									SAMD51_USB_EP_EPINTFLAG_TRFAIL0 | 
									SAMD51_USB_EP_EPINTFLAG_TRCPT1 | 
									SAMD51_USB_EP_EPINTFLAG_TRCPT0;
		USB_REG.ep[0].epstatusclr = SAMD51_USB_EP_EPSTATUS_BK1RDY;
		for (i=1 ; i<8 ; ++i) {
			if (sUsbEpReservedConfig[i][USB_OUT].enabled || sUsbEpReservedConfig[i][USB_IN].enabled) {
				USB_REG.ep[i].epcfg = sUsbEpReservedConfig[i][USB_OUT].epcfg | sUsbEpReservedConfig[i][USB_IN].epcfg;
				if (sUsbEpReservedConfig[i][USB_IN].enabled) {
					USB_REG.ep[i].epintenset =	SAMD51_USB_EP_EPINTFLAG_RXSTP | 
//												SAMD51_USB_EP_EPINTFLAG_TRFAIL1 | 
//												SAMD51_USB_EP_EPINTFLAG_TRFAIL0 | 
												SAMD51_USB_EP_EPINTFLAG_TRCPT1 
//												SAMD51_USB_EP_EPINTFLAG_TRCPT0;
												;
					USB_REG.ep[i].epstatusclr = SAMD51_USB_EP_EPSTATUS_BK1RDY;
				}
				// OUT
				else {
//					USB_REG.ep[i].epstatusset = SAMD51_USB_EP_EPSTATUS_BK0RDY;
					USB_REG.ep[i].epintenset =	SAMD51_USB_EP_EPINTFLAG_RXSTP | 
//												SAMD51_USB_EP_EPINTFLAG_TRFAIL1 | 
												SAMD51_USB_EP_EPINTFLAG_TRFAIL0 | 
//												SAMD51_USB_EP_EPINTFLAG_TRCPT1 | 
												SAMD51_USB_EP_EPINTFLAG_TRCPT0;
				}
			}
		}
	}

	if (intflag & SAMD51_USB_INT_SUSPEND) {
		if (sCleanupCb != NULL) sCleanupCb();

		if (USB_REG.reg.dadd != 0) {
			USB_REG.reg.ctrla =  (SAMD51_USB_CTRLA_SWRST);
			while ((USB_REG.reg.syncbusy & SAMD51_USB_CTRLA_SWRST) != 0);

			USB_REG.reg.ctrla =  0;
			while ((USB_REG.reg.syncbusy & SAMD51_USB_CTRLA_SWRST) != 0);

			samd51_usb_device_initialize();
			samd51_usb_device_attach(1);
		}
		USB_REG.reg.intenset = SAMD51_USB_INT_WAKEUP;

		//J IN転送中のSuspendはIN失敗で返す
		for (int i=0 ; i<8 ; ++i) {
			if (sEpDesc[i][1].pcksize.bf.byte_count != 0 && sUsbEpReservedConfig[i][USB_IN].in_cb != NULL) {
				sUsbEpReservedConfig[i][USB_IN].in_cb(AI_ERROR_USB_FAILED);
			}
		}

		return;
	}

	if (intflag & SAMD51_USB_INT_WAKEUP) {
		if (sWakeupCb) sWakeupCb();

		USB_REG.reg.intenclr = SAMD51_USB_INT_WAKEUP;
	}

	//J コントロール転送の処理 / 転送失敗時の処理
	int i=0;
	for (i=0 ; i<8 ; ++i) {
		volatile uint8_t epflag = USB_REG.ep[i].epintflag;
		USB_REG.ep[i].epintflag = SAMD51_USB_EP_INTFLAG_RXSTP | SAMD51_USB_EP_EPINTFLAG_TRFAIL1 | SAMD51_USB_EP_EPINTFLAG_TRFAIL0 | SAMD51_USB_EP_EPINTFLAG_STALL0 | SAMD51_USB_EP_EPINTFLAG_STALL1;

		if (SAMD51_USB_EP_INTFLAG_RXSTP & epflag) {
			USB_REG.ep[i].epstatusclr = SAMD51_USB_EP_EPSTATUS_BK0RDY;
			volatile UsbDeviceRequest *req = (UsbDeviceRequest *)sEpDesc[i][0].addr;
			if (req->bmRequestType.bm.type == UsbBmRequestTypeStandard) {
				_usb_dispatch_standard_request((UsbDeviceRequest *)req);
			}
			else if (req->bmRequestType.bm.type == UsbBmRequestTypeClass) {
				_usb_dispatch_class_request((UsbDeviceRequest *)req);
			}
			else if (req->bmRequestType.bm.type == UsbBmRequestTypeVender) {
				_usb_dispatch_vender_request((UsbDeviceRequest *)req);
			}
			else {
			
			}
		}
		else if (SAMD51_USB_EP_EPINTFLAG_TRFAIL1 & epflag) {

		}
		else if (SAMD51_USB_EP_EPINTFLAG_TRFAIL0 & epflag) {
			sEpDesc[i][0].status_bk = 0x00;
		}
	}

	return;
}

/*---------------------------------------------------------------------------*/
/* USB_SOF_HSOF */
void USB_1_Handler(void)
{
	
}

/*---------------------------------------------------------------------------*/
/* USB_TRCPT0_0, USB_TRCPT0_1, USB_TRCPT0_2, USB_TRCPT0_3, USB_TRCPT0_4, USB_TRCPT0_5, USB_TRCPT0_6, USB_TRCPT0_7 */
// USB OUT のみ？
void USB_2_Handler(void)
{
	int i=0;
	for (i=0 ; i<8 ; ++i) {
		volatile uint8_t epflag = USB_REG.ep[i].epintflag;
		if (SAMD51_USB_EP_EPINTFLAG_TRCPT0 & epflag) {
			USB_REG.ep[i].epintflag = SAMD51_USB_EP_EPINTFLAG_TRCPT0;

			if (sUsbEpReservedConfig[i][USB_OUT].out_cb != NULL) {
				sUsbEpReservedConfig[i][USB_OUT].out_cb((uint8_t *)sEpDesc[i][0].addr, sEpDesc[i][0].pcksize.bf.byte_count);
			}

			if (i == 0) {
				USB_REG.ep[i].epstatusclr = SAMD51_USB_EP_EPSTATUS_BK0RDY;
			}
		}
	 }
}

/*---------------------------------------------------------------------------*/
/* USB_TRCPT1_0, USB_TRCPT1_1, USB_TRCPT1_2, USB_TRCPT1_3, USB_TRCPT1_4, USB_TRCPT1_5, USB_TRCPT1_6, USB_TRCPT1_7 */
// USB IN のみ？
void USB_3_Handler(void)
{
	int i=0;
	for (i=0 ; i<8 ; ++i) {
		volatile uint8_t epflag = USB_REG.ep[i].epintflag;
		if (SAMD51_USB_EP_EPINTFLAG_TRCPT1 & epflag) {
			USB_REG.ep[i].epintflag = SAMD51_USB_EP_EPINTFLAG_TRCPT1;

			if (sUsbEpReservedConfig[i][USB_IN].in_cb != NULL) {
				sUsbEpReservedConfig[i][USB_IN].in_cb(AI_OK);
			}

			//J Set Addressの後のStatus Stage完了時の割込みで、HWにUSBアドレスを割り振る
			if (sCtx.reserved_device_address != 0) {
				USB_REG.reg.dadd = (uint8_t)(0x80 | sCtx.reserved_device_address);
				sCtx.reserved_device_address = 0;
			}
		}
	}
	
}



/*---------------------------------------------------------------------------*/
static void _usb_dispatch_standard_request(UsbDeviceRequest *req)
{
	if (req == NULL) {
		return;
	}

	volatile uint8_t std_request = req->bRequest;
	switch (std_request)
	{
	case cGetStatus:
		_usb_std_request_get_status(req);
		break;
	case cClearFeature:
		_usb_std_request_clear_feature(req);
		break;
	case cSetFeature:
		_usb_std_request_set_feature(req);
		break;
	case cSetAddress:
		_usb_std_request_set_address(req);
		break;
	case cGetDescriptor:
		_usb_std_request_get_descriptor(req);
		break;
	case cSetDescriptor:
		_usb_std_request_set_descriptor(req);
		break;
	case cGetConfiguration:
		_usb_std_request_get_configuration(req);
		break;
	case cSetConfiguration:
		_usb_std_request_set_configuration(req);
		break;
	case cGetInterface:
		_usb_std_request_get_interface(req);
		break;
	case cSetInterface:
		_usb_std_request_set_interface(req);
		break;
	case cSynchFrame:
		//DNI
		break;
	default:
		break;
	}

}

/*---------------------------------------------------------------------------*/
static void _usb_dispatch_class_request(UsbDeviceRequest *req)
{

	if (sCtx.class_request_cb) {
		sCtx.class_request_cb(req);
	}

	return;
}

/*---------------------------------------------------------------------------*/
static void _usb_dispatch_vender_request(UsbDeviceRequest *req)
{

	if (sCtx.vender_request_cb) {
		sCtx.vender_request_cb(req);
	}

	return;
}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_get_status(UsbDeviceRequest *req)
{

}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_clear_feature(UsbDeviceRequest *req)
{

}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_set_feature(UsbDeviceRequest *req)
{

}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_set_address(UsbDeviceRequest *req)
{
	sCtx.reserved_device_address = (uint8_t)req->wValue & 0x7f;
	
	//J Status Stage用に長さ0のBulk INを送り出す
	samd51_usb_transfer_control_in(NULL, 0);

	return;
}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_get_descriptor(UsbDeviceRequest *req)
{

	volatile uint16_t descriptor_type  = (req->wValue >> 8) & 0xff;
	volatile uint16_t descriptor_index = (req->wValue >> 0) & 0xff;
	volatile uint16_t required_len  = req->wLength;

	uint8_t *desc = NULL;
	uint16_t actual_descriptor_len = 0;
	int ret = _usb_search_descriptor(sCtx.desc, sCtx.desc_len, descriptor_type, descriptor_index, required_len, &desc, &actual_descriptor_len);
	if (ret != AI_OK) {
		desc = NULL;
		actual_descriptor_len = 0;
	}

	samd51_usb_transfer_control_in (desc, actual_descriptor_len);

	return;
}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_set_descriptor(UsbDeviceRequest *req)
{
	return;
}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_get_configuration(UsbDeviceRequest *req)
{

}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_set_configuration(UsbDeviceRequest *req)
{
	samd51_usb_transfer_control_in(NULL, 0);
}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_get_interface(UsbDeviceRequest *req)
{

}

/*---------------------------------------------------------------------------*/
static void _usb_std_request_set_interface(UsbDeviceRequest *req)
{	
	samd51_usb_transfer_control_in(NULL, 0);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#pragma pack(1)
typedef union UsbDescritorHead_t
{
	uint8_t array[0];
	struct {
		uint8_t bLength;
		uint8_t bDescriptorType;
		uint8_t bytes[0];
	} desc;
} UsbDescritprHead;
#pragma pack()

/*---------------------------------------------------------------------------*/
static int _usb_search_descriptor(const uint8_t *descroptor, uint16_t descriptor_len, uint8_t type, uint8_t index, uint16_t required_len, uint8_t **found_desc, uint16_t *len)
{
	if (descroptor == NULL) {
		return AI_ERROR_NULL;
	}
	
	if ((found_desc == NULL) || (len == NULL)) {
		return AI_ERROR_NOBUF;
	}
	
	//J descriptor を走査して対象となるdescriptor の先頭を見つける
	uint8_t current_index = 0;
	UsbDescritprHead *head = (UsbDescritprHead *)descroptor;
	while (((uint32_t)head - (uint32_t)descroptor) < descriptor_len) {
		if (head->desc.bDescriptorType == type) {
			if (current_index == index) {
				*found_desc = head->array;
				*len = head->desc.bLength;

				//J Configuration Descriptor が全長で要求された場合は全長を返す
				if (type == 0x02) {
					uint16_t configuration_descriptor_len = head->desc.bytes[0] | (((uint16_t)head->desc.bytes[1])<< 8);
					if (configuration_descriptor_len <= required_len) {
						*len = configuration_descriptor_len;
					}
				}
				
				return 0;
			}
			else {
				current_index++;
			}
		}
		head = (UsbDescritprHead *)((uint32_t)head + (uint32_t)head->desc.bLength);
	}

	return AI_ERROR_NOT_FOUND;
}
