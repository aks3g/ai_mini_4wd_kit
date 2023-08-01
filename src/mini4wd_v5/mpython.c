
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"

#include "kernel/kernel_impl.h"
#include "kernel/task.h"

#include "kernel_cfg.h"
#include "ai_mini4wd_os.h"

#include <string.h>
#include <stdarg.h>

#include <ai_mini4wd.h>
#include <ai_mini4wd_error.h>
#include <ai_mini4wd_fs.h>
#include <ai_mini4wd_hid.h>
#include <ai_mini4wd_timer.h>
#include <ai_mini4wd_motor_driver.h>
#include <ai_mini4wd_sensor.h>
#include <ai_mini4wd_trace.h>

#include "io_scheduler.h"


extern int mpython_is_raw_repl_mode(void);
extern int pyexec_friendly_repl(void);
extern int pyexec_raw_repl(void);
extern int mpython_wrap_init(uint32_t stack_top, uint32_t stack_size, void *heap_start, void *heap_end);
extern int mpython_wrap_init_vfs(void);
extern void mpython_wrap_deinit(void);


static void  __fatal_error(const char *msg);



static char heap[128*1024];


static int sCount = 0;
static int sCount2 = 0;
static AiMini4wdSensorData sSensorData;
static volatile int sResetReq = 0;

/*---------------------------------------------------------------------------*/
static void _sensor_callback(AiMini4wdSensorData *data)
{
	sCount++;
	memcpy(&sSensorData, data, sizeof(sSensorData));

	iset_flg(SENSOR_FLG, 0x00000001UL);
}

/*---------------------------------------------------------------------------*/
static void _on_vbus_changed(int vbus)
{
	if (vbus != 0) {
		sResetReq = 1;
	}
}

/*---------------------------------------------------------------------------*/
static char *_skipWhiteSpace(char *str)
{
	while ((*str == ' ' || *str == '\t') && *str != '\0') {
		str++;
	}

	return str;
}

/*---------------------------------------------------------------------------*/
static int _searchParameter(AiMini4wdFile *fp, const char *key, char *value, size_t len)
{
	static char record[128];

	if ((key == NULL) || (value == NULL) || (len == 0)) {
		return AI_ERROR_INVALID;
	}

	aiMini4wdFsSeek(fp, 0);

	int ret = AI_ERROR_NOENT;
	while (aiMini4wdFsGets(fp, record, sizeof(record)) != NULL) {
		if(strlen(record) == 0) {
			continue;
		}
		if(record[0] == '#') {
			continue;
		}

		if (0 != strncmp(key, record, strlen(key))) {
			continue;
		}

		char *val_ptr = &(record[strlen(key)]);
		val_ptr = _skipWhiteSpace(val_ptr);
		if (val_ptr[0] != '=') {
			continue;
		}
		val_ptr++; // = を飛ばす
		val_ptr = _skipWhiteSpace(val_ptr);
		strncpy(value, val_ptr, len);
		ret = AI_OK;
		break;
	}

	aiMini4wdFsSeek(fp, 0);

	return ret;
}


int aiMini4wdNextSensorData(AiMini4wdSensorData *storage)
{
	uint32_t ptn = 0;
 	wai_flg(SENSOR_FLG, 0x00000001UL, TWF_ORW, &ptn);
	memcpy(storage, &sSensorData, sizeof(sSensorData));

	sCount2 = sCount;
	return sCount;
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static volatile int sInternalTriggerFlag = 0;
static volatile int sExternalTriggerFlag = 0;

void aiMini4wdWaitExternalTrigger(void)
{
	//External trigger is not exist.
}

void aiMini4wdWaitInternalTrigger(void)
{
	uint32_t ptn = 0;
	clr_flg(SENSOR_FLG, 0x00000002UL);
	wai_flg(SENSOR_FLG, 0x00000002UL, TWF_ORW, &ptn);
	return;
}

int aiMini4wdCheckExternalTrigger(void)
{
	return 0;
}

static void _internalTrigger(void)
{
	iset_flg(SENSOR_FLG, 0x00000002UL);
}



/*---------------------------------------------------------------------------*
 * uPython Main Task
 *---------------------------------------------------------------------------*/
static char filename[64];
static AiMini4wdFile *sScriptFile = NULL;
AiMini4wdFile *console_out = NULL;

void upython_task(intptr_t exinf)
{
	int ret = 0;
	aiMini4wdInitialize(AI_MINI_4WD_INIT_FLAG_USE_TEST_TYPE_HW | AI_MINI_4WD_INIT_FLAG_USE_ODOMETER | AI_MINI_4WD_INIT_FLAG_USE_LED_INDICATOR);
	if (ret != 0) {
		aiMini4wdDebugPrintf("Failed to initialize mini4wd ai. ret = %08x\r\n", ret);
	}

	aiMini4wdRegisterOnVbusChangedCb(_on_vbus_changed);

	ret = aiMini4wdSensorRegisterCapturedCallback(_sensor_callback);
	if (ret != 0) {
		__fatal_error("Failed to set Sensor callback.");
	}

	ret = aiMini4wdRegisterOnStartCb(_internalTrigger);
	if (ret != 0) {
		__fatal_error(" Failed to set internal trigger callback.");
	}

	sScriptFile = aiMini4wdFsOpen("BOOT.INI", "r");
	if (sScriptFile == NULL) {
		__fatal_error("  Failed to open boot.ini\r\n");
	}

	//J Boot.ini の中からスクリプトファイル名を検索する
	ret = _searchParameter(sScriptFile, "script", filename, sizeof(filename));
	if (ret != AI_OK) {
		strcpy(filename, "default.py");
	}
	aiMini4wdFsClose(sScriptFile);
	sScriptFile = aiMini4wdFsOpen(filename, "r");


	//J Micro Pythonの初期化
	TCB *p_tcb = get_tcb_self(MPYTHON_TASK);

	mpython_wrap_init((uint32_t)p_tcb->tskctxb.sp, STACK_SIZE, heap, heap + sizeof(heap));
	mpython_wrap_init_vfs();

	//J ログファイルの作成とConsole outへの設定
	struct AiMini4wdTm  t;
	aiMini4wdRtcGetLocaltime(&t);

	sprintf(filename, "log_%04d%02d%02d_%02d-%02d-%02d.txt", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	console_out = aiMini4wdFsOpen(filename, "w");
	if (console_out==NULL) {
		return;
	}

	//J uPythonの標準出力をFileIOに繋ぐ
	ioSchedulerInitPutc(console_out);

	aiMini4wdDebugTraceClear();
	aiMini4wdDebugTraceControl(1);

soft_reset:
	for (;;) {
		if (mpython_is_raw_repl_mode()) {
			if ((ret = pyexec_raw_repl()) != 0) {
				break;
			}

			//J モーターは止める
			aiMini4wdMotorDriverDrive(0);
			dly_tsk(100);

			//J 強制的にFlushする
			ioSchedulerFinalizePutc();

			//J LEDを起動状態にしておく
			aiMini4wdPrintLedPattern(0x0);
		} else {
			if ((ret = pyexec_friendly_repl()) != 0) {
				break;
			}
		}
	}
	ioSchedulerFinalizePutc();

	aiMini4wdSetErrorStatus(2);

//goto soft_reset;

	while(1) {
		dly_tsk(1);
		if (sResetReq) {
//			mpython_wrap_deinit();
			dly_tsk(500);
			aiMini4wdReset(0); //Reset
		}
	}
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
// Receive single character
typedef enum AiMini4wdMicroPythonRawScriptReadState_t
{
	cAiMini4wdMicroPythonReadStateSendCtrlA  = 1,
	cAiMini4wdMicroPythonReadStateWaitForEof = 2,
	cAiMini4wdMicroPythonReadStateSentCtrlD  = 3,
	cAiMini4wdMicroPythonReadStateSentCtrlB  = 5,
} AiMini4wdMicroPythonRawScriptReadState;

#define CTRL_A		(1)
#define CTRL_B		(2)
#define CTRL_C		(3)
#define CTRL_D		(4)
static AiMini4wdMicroPythonRawScriptReadState sReadState = cAiMini4wdMicroPythonReadStateSendCtrlA;

int mp_hal_stdin_rx_chr(void)
{
	//J Load from file
	char ch = 0;

	if (sReadState == cAiMini4wdMicroPythonReadStateSendCtrlA) {
		ch = CTRL_A;
		sReadState = cAiMini4wdMicroPythonReadStateWaitForEof;
	}
	else if (sReadState == cAiMini4wdMicroPythonReadStateWaitForEof) {
		int ret = aiMini4wdFsRead(sScriptFile, &ch, 1);
		if (ret != 1) {
			ch = CTRL_D;
			sReadState = cAiMini4wdMicroPythonReadStateSentCtrlD;
		}
	}
	else if (sReadState == cAiMini4wdMicroPythonReadStateSentCtrlD) {
		ch = CTRL_B;
		sReadState = cAiMini4wdMicroPythonReadStateSentCtrlB;
	}
	else if (sReadState == cAiMini4wdMicroPythonReadStateSentCtrlB) {
		//J Wait for Reset
		while(sResetReq == 0) dly_tsk(1);
	}

	if (sResetReq != 0) {
		aiMini4wdReset(0x00000000);
	}

    return ch;
}

/*---------------------------------------------------------------------------*/
// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, uint32_t len)
{
	ioSchedulerPuts(str, len);
}

/*---------------------------------------------------------------------------*/
#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind) {
	nlr_buf_t nlr;
	if (nlr_push(&nlr) == 0) {
		mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
		qstr source_name = lex->source_name;
		mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
		mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
		mp_call_function_0(module_fun);
		nlr_pop();
		} else {
		// uncaught exception
		
		mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
//		mp_obj_print_exception(&mp_plat_print, nlr.ret_val);
	}
}
#endif

static void __fatal_error(const char *msg) {
	//J Fatalで落ちたときにディスクを無駄にしないように修正
	ioSchedulerPuts(msg, strlen(msg));
	dly_tsk(100);
	ioSchedulerFinalizePutc();
	dly_tsk(100);

	aiMini4wdFsClose(console_out);

	aiMini4wdSetErrorStatus(1);
	while(1) {
		dly_tsk(100);
	}
}



#define TRACE_DSP_ENTER			(0x10 << 24)
#define TRACE_DSP_LEAVE			(0x20 << 24)
#define TRACE_INH_ENTER			(0x30 << 24)
#define TRACE_INH_LEAVE			(0x40 << 24)
#define TRACE_EXC_ENTER			(0x50 << 24)
#define TRACE_EXC_LEAVE			(0x60 << 24)
#define TRACE_GC_START			(0x70 << 24)
#define TRACE_GC_END			(0x80 << 24)

extern ulong_t	_kernel_current_time;

void
log_dsp_enter(TCB *p_tcb)
{
	aiMini4wdDebugTracePush(TRACE_DSP_ENTER | _kernel_current_time, (uint32_t)p_tcb);
}

void
log_dsp_leave(TCB *p_tcb)
{
	aiMini4wdDebugTracePush(TRACE_DSP_LEAVE | (1<<24) | _kernel_current_time, (uint32_t)p_tcb);
	aiMini4wdDebugTracePush(TRACE_DSP_LEAVE | (2<<24) |_kernel_current_time, (uint32_t)p_tcb->tskctxb.sp);
	aiMini4wdDebugTracePush(TRACE_DSP_LEAVE | (3<<24) |_kernel_current_time, (uint32_t)p_tcb->tskctxb.pc);
}

void
log_inh_enter(INHNO inhno)
{
	aiMini4wdDebugTracePush(TRACE_INH_ENTER | _kernel_current_time, (uint32_t)inhno);
}

void
log_inh_leave(INHNO inhno)
{
	aiMini4wdDebugTracePush(TRACE_INH_LEAVE | _kernel_current_time, (uint32_t)inhno);
}

void
log_exc_enter(EXCNO excno)
{
	aiMini4wdDebugTracePush(TRACE_EXC_ENTER | _kernel_current_time, (uint32_t)excno);
}


uint32_t _get_sp( void )
{
	uint32_t sp=0;
	__asm volatile (
	"ITE     EQ"
	);
	__asm volatile (
	"MRSEQ   %[result], MSP" : [result]  "=r" (sp)
	);
	__asm volatile (
	"MRSNE   %[result], PSP" : [result]  "=r" (sp)
	);

	return sp;
}


void
log_exc_leave(EXCNO excno)
{
	uint32_t *sp = (uint32_t *)_get_sp();

	aiMini4wdDebugTracePush(TRACE_EXC_LEAVE | (1<<24) | _kernel_current_time, (uint32_t)excno);
	aiMini4wdDebugTracePush(TRACE_EXC_LEAVE | (2<<24) | _kernel_current_time, (uint32_t)sp);
	aiMini4wdDebugTracePush(TRACE_EXC_LEAVE | (3<<24) | _kernel_current_time, sp[0]);
	aiMini4wdDebugTracePush(TRACE_EXC_LEAVE | (4<<24) | _kernel_current_time, sp[1]);
	aiMini4wdDebugTracePush(TRACE_EXC_LEAVE | (5<<24) | _kernel_current_time, sp[2]);
}


void mpython_wrap_start_hook(void)
{
	aiMini4wdDebugTracePush(TRACE_GC_START | _kernel_current_time, 0);
}

void mpython_wrap_end_hook(void)
{
	aiMini4wdDebugTracePush(TRACE_GC_END | _kernel_current_time, 0);
}