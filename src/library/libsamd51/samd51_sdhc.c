/*
 * samd51_sdhc.c
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <sam.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

//J Debug INFO生成のために必要
#include <stdio.h>
#include <stdarg.h>

#include <samd51_error.h>
#include <samd51_sdhc.h>

//J ADMAの結果を返すCB
typedef void (*Samd51SdhcTransferDoneCallback)(int status);

typedef struct Samd51SdhcContext_t
{
	SdhcAdmaDescriptor adma_desc;

	Samd51SdhcTransactionDoneCallback transactionDoneCb;
	Samd51SdhcTransferDoneCallback transferDoneCb;
	uint8_t resp_buffer[17]; // 17 x 8 = 136bits max
	size_t resp_bits;
	
	volatile int transfer_completed;
	volatile int transfer_status;
} Samd51SdhcContext;

static volatile Samd51SdhcContext sSdhcCtx;


static int _sdhc_acmd41(void);
static int _sdhc_cmd1(void);
static void sTransferDone(int status);
static void _recover_cmd_line(SAMD51_SDHC sdhc);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int samd51_sdhc_initialize(SAMD51_SDHC sdhc)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);

	//J Context を初期化
	memset ((void *)&sSdhcCtx, 0x00, sizeof(sSdhcCtx));
	sSdhcCtx.transfer_completed = 1;

	//J Internal Clock をEnableして安定するのを待つ
	regs->clock_control.bm.internal_clock_enable = 1;
	while (regs->clock_control.bm.internal_clock_stable == 0);

	//J Interrupt ハンドラを有効にしておく
	NVIC_EnableIRQ(SDHC0_IRQn);
	
	//J 必要なInterrupt を有効にする
	regs->normal_interrupt_signal_enable.bm.transfer_complete = 1;
//	regs->normal_interrupt_signal_enable.bm.buffer_read_ready = 1;
//	regs->normal_interrupt_signal_enable.bm.buffer_write_ready = 1;
	regs->normal_interrupt_signal_enable.bm.command_complete = 1;
	regs->normal_interrupt_signal_enable.bm.dma_interrupt = 1;
	regs->normal_interrupt_signal_enable.bm.block_gap_event = 1;
	
	regs->normal_interrupt_status_enable.bm.transfer_complete = 1;
//	regs->normal_interrupt_status_enable.bm.buffer_read_ready = 1;
//	regs->normal_interrupt_status_enable.bm.buffer_write_ready = 1;
	regs->normal_interrupt_status_enable.bm.command_complete = 1;
	regs->normal_interrupt_status_enable.bm.dma_interrupt = 1;
	regs->normal_interrupt_status_enable.bm.block_gap_event = 1;


	regs->error_interrupt_signal_enable.bm.adma_error = 1;
	regs->error_interrupt_signal_enable.bm.auto_cmd12_error = 1;
	regs->error_interrupt_signal_enable.bm.command_crc_error = 1;
	regs->error_interrupt_signal_enable.bm.command_end_bit_error = 1;
	regs->error_interrupt_signal_enable.bm.command_index_error = 1;
	regs->error_interrupt_signal_enable.bm.command_timeout_error = 1;
	regs->error_interrupt_signal_enable.bm.current_limit_error = 1;
	regs->error_interrupt_signal_enable.bm.data_crc_error = 1;
	regs->error_interrupt_signal_enable.bm.data_end_bit_error = 1;
	regs->error_interrupt_signal_enable.bm.data_timeout_error = 1;

	regs->error_interrupt_status_enbale.bm.adma_error = 1;
	regs->error_interrupt_status_enbale.bm.auto_cmd12_error = 1;
	regs->error_interrupt_status_enbale.bm.command_crc_error = 1;
	regs->error_interrupt_status_enbale.bm.command_end_bit_error = 1;
	regs->error_interrupt_status_enbale.bm.command_index_error = 1;
	regs->error_interrupt_status_enbale.bm.command_timeout_error = 1;
	regs->error_interrupt_status_enbale.bm.current_limit_error = 1;
	regs->error_interrupt_status_enbale.bm.data_crc_error = 1;
	regs->error_interrupt_status_enbale.bm.data_end_bit_error = 1;
	regs->error_interrupt_status_enbale.bm.data_timeout_error = 1;



	regs->host_control2.bm.asynchronous_interrupt_enable = 1;

	samd51_sdhc_sd_bus_power_control(sdhc, SAMD51_SDHC_BUS_3V3);
	samd51_sdhc_enable_sd_clock(sdhc, 48000000, 24000000, 1);

	//J ADMAを使用
	regs->host_control1.bm.dma_select = 2;

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
void samd51_sdhc_finalize(SAMD51_SDHC sdhc)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);

	NVIC_DisableIRQ(SDHC0_IRQn);

	regs->software_reset.byte = 1;
}


/*--------------------------------------------------------------------------*/
int samd51_sdhc_is_card_inserted(SAMD51_SDHC sdhc)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);

	return regs->present_state.bm.card_inserted;
}

/*--------------------------------------------------------------------------*/
int samd51_sdhc_sd_bus_power_control(SAMD51_SDHC sdhc, SAMD51_SDHC_BUS_VOLTAGE bus_voltage)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);
	if (bus_voltage != SAMD51_SDHC_BUS_3V3) {
		return AI_ERROR_INVALID;
	}

	regs->power_control.bm.sd_bus_voltage_select = bus_voltage;
	regs->power_control.bm.sd_bus_power = 1;

	return AI_OK;	
}

/*--------------------------------------------------------------------------*/
int samd51_sdhc_enable_sd_clock(SAMD51_SDHC sdhc, uint32_t BaseClock, uint32_t sdclkHz, int enable)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);

	// 3.2.1 SD Clock Supply Sequence
	if (enable) {
		regs->clock_control.bm.internal_clock_enable = 0;
		
		// 1. Calculate a divisor.
		(void) sdclkHz;
		
		// 2. Set SDCLK Frequency Select.
		//J 本来は計算で出すべき。48MHzのシステムクロックを1/2して使う = 24MHz
		uint8_t validDividerValueTable[] = {0, 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};
		uint32_t validDividerTalbe[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};

		int i=0;
		for (i=0 ; i<sizeof(validDividerValueTable) ; ++i) {
			if (sdclkHz >= (BaseClock / validDividerTalbe[i])) {
				break;
			}
		}
		
		if (i == sizeof(validDividerValueTable)) {
			return AI_ERROR_INVALID;
		}

		regs->clock_control.bm.sdclk_frequency_select = validDividerValueTable[i];

		//J SDClock が早い場合、Time outを長めに設定（CPUクロックが高いと死ぬ)
		//J 最低500ms以上必要
		float sdclk = (float)BaseClock / (float)validDividerTalbe[i];
		regs->timeout_control = 0x0e;

		for (uint32_t p=0 ; p<0xf ; ++p) {
			if ((((uint32_t)2 << (p+13)) * (1/sdclk)) >= 0.5f) {
				regs->timeout_control = p;
				break;
			}
		}

		// 3. Wait for Clock Stable
		regs->clock_control.bm.internal_clock_enable = 1;
		while (regs->clock_control.bm.internal_clock_stable == 0);
		
		// 4. Set SD Clock ON
		regs->clock_control.bm.sd_clock_enable = 1;
	}
	else {
		regs->clock_control.bm.sd_clock_enable = 0;
	}

	return AI_OK;	
}

/*--------------------------------------------------------------------------*/
int samd51_sdhc_send_cmd(SAMD51_SDHC sdhc, uint16_t cmd, uint32_t arg, Samd51SdhcTransactionDoneCallback cb)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);
	
	volatile uint32_t inhibits = 0;
	while ((inhibits = regs->present_state.bm.command_inhibit_cmd) != 0);
	while ((inhibits = regs->present_state.bm.command_inhibit_dat) != 0);

	sSdhcCtx.transactionDoneCb = cb;
	
	regs->argument1 = arg;
	regs->command.word = cmd;

	return AI_OK;
}

static volatile int sCbDone = 0;
static int sRespStatus = 0;
static void *sDefaultCbRespBuf = NULL;
static size_t sDefaultCbRespSize = 0;
static void _sdhc_default_cb(int status, const void *resp, const size_t resp_len)
{
	sCbDone = 1;

	sRespStatus = status;
	if (status != AI_OK) {
		return;
	}
	
	if (resp_len != 0 && (sDefaultCbRespBuf == NULL || sDefaultCbRespSize < resp_len)) {
		sRespStatus = AI_ERROR_NOBUF;
		return;
	}

	memcpy(sDefaultCbRespBuf, resp, resp_len);		

	return;
}


/*--------------------------------------------------------------------------*/
int samd51_sdhc_send_cmd_blocing(SAMD51_SDHC sdhc, uint16_t cmd, uint32_t arg, void *resp, size_t resp_len)
{
	sCbDone = 0;
	sDefaultCbRespBuf = resp;
	sDefaultCbRespSize = resp_len;
	
	int ret = samd51_sdhc_send_cmd(sdhc, cmd, arg, _sdhc_default_cb);
	if (ret != AI_OK) {
		return AI_ERROR_NODEV;
	}

	while (sCbDone == 0);

	if (sRespStatus != AI_OK) {
		return sRespStatus;
	}
	
	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_sdhc_card_initialization_and_identification(SAMD51_SDHC sdhc, Samd51_sdhcCardContext *ctx)
{
	int flagCMD8 = 0;
	if (ctx == NULL) {
		return AI_ERROR_NULL;
	}

	int inserted = samd51_sdhc_is_card_inserted(SAMD51_SDHC0);
	if (inserted) {
		ctx->status &= ~SAMD51_SDHC_CARD_STATUS_NODISK;
	}
	else {
		return AI_ERROR_DISK_NORESPONSE;
	}

	//J 3.6 Card Initialization and Identification
	//J 最初にCMD0 Reset を打つ
	int ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0, SDHC_CMD0, 0, NULL, 0);
	if (ret != AI_OK) {
		return ret;
	}

	//J CMD8で対応電圧確認
	ArgCmd8 argCmd8;
	{
		argCmd8.bm.supply_voltage = 1;
		argCmd8.bm.pattern = 0xAA;
	}
	ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0,SDHC_CMD8, argCmd8.dword, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
	if (ret == AI_OK) {
		flagCMD8 = 1;
	}
	else if (ret == AI_ERROR_DISK_NORESPONSE) {
		_recover_cmd_line(sdhc);
		flagCMD8 = 0;
	}
	else {
		return AI_ERROR_DISK_FAIL;
	}

	//J ACMD41が反応するか確認してSDIOをはじく
	int flagCMD1 = 0;
	int card_capacity_status = _sdhc_acmd41();
	if (card_capacity_status < 0) {
		_recover_cmd_line(sdhc);

		samd51_sdhc_enable_sd_clock(sdhc, 48000000, 400000, 1); //J MMCは400kbpsでないと動けない
		ret = _sdhc_cmd1();
		if (ret != AI_OK) {
			return ret;
		}

		flagCMD1 = 1;
	}

	//J CMD8に反応した場合SDSCv2/v3/SDHC/SDXC
	//J CMD8に反応しなかった場合SDSC1.10以下
	ctx->type = SAMD51_SDHC_UNKNOWN;
	if (flagCMD8) {
		ctx->type = SAMD51_SDHC_SDv2;
		ctx->isBlockDevice = (card_capacity_status == 1) ? 1 : 0;
	}
	else if (flagCMD1) {
		ctx->type = SAMD51_SDHC_MMC;
	}
	else {
		ctx->type = SAMD51_SDHC_SDv1;
	}

	ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0, SDHC_CMD2, 0, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
	if (ret == AI_OK) {
		memcpy (&ctx->cid, (void *)sSdhcCtx.resp_buffer, sizeof(ctx->cid));
	}
	else {
		_recover_cmd_line(sdhc);
		memset (&ctx->cid, 0, sizeof(ctx->cid));
	}


	volatile uint32_t rca = (ctx->type == SAMD51_SDHC_MMC) ? (0x1 << 16) : 0;
	ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0, SDHC_CMD3, rca, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
	if (ret == AI_OK) {
		SdcR6 *r6 = (SdcR6 *)sSdhcCtx.resp_buffer;
		ctx->rca = r6->new_published_rca;
	}
	else {
		_recover_cmd_line(sdhc);
	}

	ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0, SDHC_CMD9, ((uint32_t)ctx->rca)  << 16, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
	if (ret == AI_OK) {
		memcpy (&ctx->csd, (void *)sSdhcCtx.resp_buffer, sizeof(ctx->csd));
	}
	else {
		_recover_cmd_line(sdhc);
		memset (&ctx->csd, 0, sizeof(ctx->csd));
	}
	
	//J Cardが特定できている事を以て、初期化が出来ていると認識
	if (ctx->type != SAMD51_SDHC_UNKNOWN) {
		ctx->status &= ~SAMD51_SDHC_CARD_STATUS_NOINIT;
	}
	else {
		return AI_ERROR_DISK_FAIL;
	}

	//J Select SD Card
	if (ctx->rca != 0) {
		ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0, SDHC_CMD7, ((uint32_t)ctx->rca)  << 16, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
		if (ret != AI_OK) {
			return ret;
		}

		samd51_sdhc_changing_bus_width(sdhc, ctx->rca, SAMD51_SDHC_BUS_4BIT);
	}

	return AI_OK;
}

/*--------------------------------------------------------------------------*/
int samd51_transfer(SAMD51_SDHC sdhc, Samd51_sdhcCardContext *ctx, int dir_read, uint32_t sector, void *buf, uint32_t length, int blocking)
{
	static const uint16_t cSdhcTransferCmdTable[4] =
	{
		SDHC_CMD24,
		SDHC_CMD25,
		SDHC_CMD17,
		SDHC_CMD18,
	};

	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);

	if ((length % 512) != 0) {
		return AI_ERROR_INVALID;
	}

	uint16_t count = length;
	if (ctx->isBlockDevice) {
		count = count / 512;
	}

	while (sSdhcCtx.transfer_completed == 0);

	//J Setup Descriptor Table 1個のデータのやり取りに限って良いと思う
	memset ((void *)&sSdhcCtx.adma_desc, 0x00, sizeof(sSdhcCtx.adma_desc));
	sSdhcCtx.adma_desc.address = (uint32_t)buf;
	sSdhcCtx.adma_desc.length = length;
	sSdhcCtx.adma_desc.attr_valid = 1;
	sSdhcCtx.adma_desc.attr_act = 2;
	sSdhcCtx.adma_desc.attr_end = 1;
	sSdhcCtx.adma_desc.attr_int = 1;

	//J ADMA System Addr
	uint32_t desc_ptr_addr = (uint32_t)&sSdhcCtx.adma_desc;
	regs->adma_system_address = (uint64_t)desc_ptr_addr;
	
	//J Set Block Size -> 初期値で良いのではないか
	regs->block_size.word = 0;
	regs->block_size.bm.transfer_block_size = 0x200;
	
	//J	Set Block count
	regs->block_count = count;
			
	//J 転送モードを設定
	uint16_t cmd = 0;
	regs->transfer_mode.word = 0;
	if (count > 1) {
		regs->transfer_mode.bm.multi_single_block_select = 1;		
		regs->transfer_mode.bm.auto_command_enable = SAMD51_SDHC_AUTO_CMD12;
		cmd |= 1;
	}

	if (dir_read) {
		regs->transfer_mode.bm.data_transfer_direction_select = 1;
		cmd |= 2;
	}
	regs->transfer_mode.bm.dma_enable = 1;
	regs->transfer_mode.bm.block_count_enable = 0;
	
	cmd = cSdhcTransferCmdTable[cmd];
 	
	//J Flagを準備
	sSdhcCtx.transfer_completed = 0;
	sSdhcCtx.transferDoneCb = sTransferDone;

	//J ここでコマンドを打ち込むとコマンドのレスポンスは戻ってくる
	SdcR1 respR1;
	memset (&respR1, 0, sizeof(respR1));
	samd51_sdhc_send_cmd_blocing(sdhc, cmd, sector, &respR1, sizeof(respR1));
		
	if (blocking) {
		//J 次に、Trnsfer Complete 割り込みかADAMA Error割り込みが来るのを待つ
		while (sSdhcCtx.transfer_completed == 0);
	}
	
	if (sSdhcCtx.transfer_status != AI_OK) {
		// Abort Operation	
	}
	else {
		
	}

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int samd51_sdhc_get_host_controller_version(SAMD51_SDHC sdhc)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);

	return regs->host_controller_version.bm.specificatoin_version_number;	
}


/*--------------------------------------------------------------------------*/
int samd51_sdhc_get_host_controller_info(SAMD51_SDHC sdhc, char *info, size_t info_len)
{
	if (info == NULL) {
		return AI_ERROR_NULL;
	}
	if (info_len == 0) {
		return AI_ERROR_NOBUF;
	}
	
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);
	
	char *str = info;
	int size = 0;	
	size_t remaining_size = info_len - 1;
	memset (info, 0, info_len);

	//J バッファにデバッグ情報を書き込む
	size = snprintf (str, remaining_size, "SD Host Controller Capabilities for SDHC @ 0x%08x\r\n", (int)sdhc);
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Timeout Clock Frequency = %d\r\n", regs->capabilities.bm.timout_clock_frequency);
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Timeout Clock Unit = %d\r\n", regs->capabilities.bm.timeout_clock_unit);
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Base Clock Frequency For SD Clock = %d\r\n", regs->capabilities.bm.base_clock_frequency_for_sd_clock);
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Max Block Length = %d\r\n", regs->capabilities.bm.max_block_length);
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  8bit support for Embedded Device = %s\r\n", regs->capabilities.bm._8bit_support_for_embeddded_device != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;
	
	size = snprintf (str, remaining_size, "  ADMA2 Support = %s\r\n", regs->capabilities.bm.adma2_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;
	
	size = snprintf (str, remaining_size, "  High Speed Support = %s\r\n", regs->capabilities.bm.high_speed_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  SDMA Support = %s\r\n", regs->capabilities.bm.sdma_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Suspend/Resume Support = %s\r\n", regs->capabilities.bm.suspend_resume_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Voltage Support 3.3V Support = %s\r\n", regs->capabilities.bm.voltage_support_3v3 != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Voltage Support 3V Support = %s\r\n", regs->capabilities.bm.voltage_support_3v != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Voltage Support 1.8V Support = %s\r\n", regs->capabilities.bm.voltage_support_1v8 != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  64bit System Bus Support = %s\r\n", regs->capabilities.bm._64bit_system_bus_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Asynchronous Interrupt Support = %s\r\n", regs->capabilities.bm.asynchronous_interrupt_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Slot Type = %d\r\n", regs->capabilities.bm.slot_type);
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  SDR 50 Support = %s\r\n", regs->capabilities.bm.sdr50_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  SDR104 Support = %s\r\n", regs->capabilities.bm.sdr104_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  DDR 50 Support = %s\r\n", regs->capabilities.bm.ddr50_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Drive Type A Support = %s\r\n", regs->capabilities.bm.driver_type_a_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Drive Type B Support = %s\r\n", regs->capabilities.bm.driver_type_b_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Drive Type C Support = %s\r\n", regs->capabilities.bm.driver_type_c_support != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Timer Count for Re-Tuning = %d\r\n", regs->capabilities.bm.timer_count_for_re_tuning);
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Use Tuning for SDR50 = %s\r\n", regs->capabilities.bm.use_tuning_for_sdr50 != 0 ? "YES" : "NO");
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Re-Tuning Modes = %d\r\n", regs->capabilities.bm.re_tuning_modes);
	str += size;
	remaining_size -= size;

	size = snprintf (str, remaining_size, "  Clock Multiplier = %d\r\n", regs->capabilities.bm.clock_multiplier);
	str += size;
	remaining_size -= size;


	return AI_OK;
}

int samd51_sdhc_changing_bus_width(SAMD51_SDHC sdhc, uint16_t rca, SAMD51_SDHC_BUS_WIDTH bus_width)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);

	// Send ACMD6
	ArgCmd55 argCmd55;
	{
		argCmd55.dword = 0;
		argCmd55.bm.rca = rca;
	}
	ArgAcmd6 argAcmd6;
	{
		argAcmd6.dword =0;
		argAcmd6.bm.bus_width = (uint32_t)bus_width;
	}

	int ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0, SDHC_CMD55, argCmd55.dword, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
	if (ret != AI_OK) {
		return -1;
	}

	ret = samd51_sdhc_send_cmd_blocing(sdhc, SDHC_ACMD6, argAcmd6.dword, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
	if (ret != AI_OK) {
		return -1;
	}

	//J Change the Bus Width
	regs->host_control1.bm.data_transfer_width = (bus_width == SAMD51_SDHC_BUS_4BIT) ? 1 : 0;
	
	return AI_OK;
}


void SDHC0_Handler(void)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(SAMD51_SDHC0);

	//J Handle Errors
	if (regs->normal_interrupt_status.bm.error_interrupt) {
		regs->normal_interrupt_status.word |= (1 << 15);

		if (regs->error_interrupt_status.bm.adma_error) {
			regs->error_interrupt_status.word |= (1 << 9);

			if (sSdhcCtx.transferDoneCb != NULL) {
				sSdhcCtx.transferDoneCb(AI_ERROR_DISK_FAIL);
				sSdhcCtx.transferDoneCb = NULL;
			}
		}

		if (regs->error_interrupt_status.bm.command_timeout_error) {
			regs->error_interrupt_status.word |= (1 << 0);
		
			if (sSdhcCtx.transactionDoneCb != NULL) {
				sSdhcCtx.transactionDoneCb(AI_ERROR_DISK_NORESPONSE, NULL, 0);
				sSdhcCtx.transactionDoneCb = NULL;
			}
		}

		if (regs->error_interrupt_status.bm.data_timeout_error) {
			regs->error_interrupt_status.word |= (1 << 4);

			if (sSdhcCtx.transferDoneCb != NULL) {
				sSdhcCtx.transferDoneCb(AI_ERROR_DISK_TIMEOUT);
				sSdhcCtx.transferDoneCb = NULL;
			}
		}

	}

	//J Command Completed -> callback 
	if (regs->normal_interrupt_status.bm.command_complete) {
		regs->normal_interrupt_status.word |= (1 << 0);
	
		static size_t resp_bits_table[4] = {0, 128, 32, 32};
		sSdhcCtx.resp_bits = resp_bits_table[regs->command.bm.response_type_select];

		size_t resp_bytes = (sSdhcCtx.resp_bits + 7) / 8;
		if (resp_bytes) {
			memcpy((void *)sSdhcCtx.resp_buffer, (void *)regs->response, resp_bytes);
		}

		if (sSdhcCtx.transactionDoneCb != NULL) {
			sSdhcCtx.transactionDoneCb(AI_OK, (void *)sSdhcCtx.resp_buffer, resp_bytes);
			sSdhcCtx.transactionDoneCb = NULL;
		}
	}

	//J Transfer Completed -> callback
	if (regs->normal_interrupt_status.bm.transfer_complete) {
		regs->normal_interrupt_status.word |= (1 << 1);

		if (sSdhcCtx.transferDoneCb != NULL) {
			sSdhcCtx.transferDoneCb(AI_OK);
			sSdhcCtx.transferDoneCb = NULL;
		}
	}
	



	return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static int _sdhc_acmd41(void)
{
	int card_capacity_status = 0;
	int completed = 0;
	// Initialize Card
	ArgCmd55 argCmd55;
	{
		argCmd55.dword = 0;
		argCmd55.bm.rca = 0;
	}
	
	ArgAcmd41 argCmd41;
	{
		argCmd41.dword = 0;
		argCmd41.bm.ocr = 0xff80;
		argCmd41.bm.s18r = 0;
		argCmd41.bm.xpc= 1;
		argCmd41.bm.hcs = 1;
	}

	do {
		int ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0, SDHC_CMD55, argCmd55.dword, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
		if (ret != AI_OK) {
			return -1;
		}
		ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0, SDHC_ACMD41, argCmd41.dword, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
		if (ret != AI_OK) {
			return -1;
		}
		else if (ret == AI_OK) {
			SdcR3 *r3 = (SdcR3*)sSdhcCtx.resp_buffer;
			if (r3->bm.busy != 0) {
				completed = 1;
				card_capacity_status = r3->bm.card_capacity_status;	
			}
		}
	} while (!completed);

	return card_capacity_status;
}

static int _sdhc_cmd1(void)
{
	//J CMD1を試す(MMC)
	volatile int completed = 0;
	ArgAcmd41 argCmd1;
	{
		argCmd1.dword = 0;
		argCmd1.bm.ocr = 0xff80;
		argCmd1.bm.s18r = 0;
		argCmd1.bm.xpc= 0;
		argCmd1.bm.hcs = 0;
	}

	do {
		int ret = samd51_sdhc_send_cmd_blocing(SAMD51_SDHC0, SDHC_CMD1, argCmd1.dword, (void *)sSdhcCtx.resp_buffer, sizeof(sSdhcCtx.resp_buffer));
		if (ret != AI_OK) {
			_recover_cmd_line(SAMD51_SDHC0);
			return AI_ERROR_DISK_NORESPONSE;
		}
		else if (ret == AI_OK) {
			volatile SdcR3 *r3 = (SdcR3*)sSdhcCtx.resp_buffer;
			if (r3->bm.busy != 0) {
				completed = 1;
//				card_capacity_status = r3->bm.card_capacity_status;
			}
		}
	} while (!completed);
	

	return AI_OK;
}



static void sTransferDone(int status)
{
	sSdhcCtx.transfer_status = status;
	sSdhcCtx.transfer_completed = 1;
	
	return;
	
}

static void _recover_cmd_line(SAMD51_SDHC sdhc)
{
	volatile SdHostControllerRegs *regs = (SdHostControllerRegs*)(sdhc);

	regs->software_reset.bm.software_reset_for_cmd_line = 1;
	
	while (regs->present_state.bm.command_inhibit_cmd != 0);	
	
	return;
}

