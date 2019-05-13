/*
 * main.c
 *
 * Created: 2019/03/10
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */ 

#include <stdio.h>
#include <string.h>

// #include "sam.h"
#include <ai_mini4wd.h>
#include <ai_mini4wd_error.h>
#include <ai_mini4wd_fs.h>
#include <ai_mini4wd_hid.h>
#include <ai_mini4wd_timer.h>
#include <ai_mini4wd_sensor.h>


#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "lib/utils/pyexec.h"

static char *stack_top;
static char heap[128*1024];

void NORETURN __fatal_error(const char *msg);


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

/*---------------------------------------------------------------------------*/
volatile uint32_t sflag = 0;
static void _timer_cb_100ms(void)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static volatile int sCaptureDone = 0;
static volatile AiMini4wdSensorData *sData = NULL;
static int sCount = 0;
static void _sensor_callback(AiMini4wdSensorData *data)
{
	sCount++;
	if ((sData != NULL) && (sCaptureDone == 0)) {
		memcpy((void *)sData, data, sizeof(AiMini4wdSensorData));
		sCaptureDone = 1;
	}
}


int aiMini4wdNextSensorData(AiMini4wdSensorData *storage)
{
	sData = storage;
	sCaptureDone = 0;

	volatile int dummy_wait = 0;
	while(sCaptureDone == 0){
		dummy_wait++;
	}

	sCaptureDone = 0;
	sData = NULL;

	return sCount;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static volatile int sInternalTriggerFlag = 0;
static volatile int sExternalTriggerFlag = 0;

void aiMini4wdWaitExternalTrigger(void)
{
	sExternalTriggerFlag = 0;
	volatile int dummy = 0;
	while (sExternalTriggerFlag == 0) {
		dummy++;
	}
}

void aiMini4wdWaitInternalTrigger(void)
{
	sInternalTriggerFlag = 0;
	volatile int dummy = 0;
	while (sInternalTriggerFlag == 0) {
		dummy++;
	}

	return;
}

int aiMini4wdCheckExternalTrigger(void)
{
	if (sExternalTriggerFlag) {
		sExternalTriggerFlag = 0;
		
		return 1;
	}

	return 0;
}

static void _externalTrigger(void)
{
	sExternalTriggerFlag = 1;
}

static void _internalTrigger(void)
{
	sInternalTriggerFlag = 1;
}


static char *_skipWhiteSpace(char *str)
{
	while ((*str == ' ' || *str == '\t') && *str != '\0') {
		str++;
	}

	return str;
}

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

void _searchNewLogFilename(char *logfilename) {
	AiMini4wdFileInfo info;
	int cnt = 0;
	while (cnt < 1000) {
		sprintf (logfilename, "log%03d.txt", cnt);
		int ret = aiMini4wdFsStat(logfilename, &info);
		if (ret != AI_OK) {
			break; //J ファイルが見つからない = 作ってOK
		}

		cnt++;
	}

	return;
}



/*---------------------------------------------------------------------------*/
static AiMini4wdFile *sConsoleOut = NULL;
static AiMini4wdFile *sScriptFile = NULL;


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
int main(void)
{
	int ret = aiMini4wdInitialize(AI_MINI_4WD_INIT_FLAG_USE_DEBUG_PRINT);
	if (ret != 0) {
		aiMini4wdDebugPrintf("Failed to initialize mini4wd ai. ret = %08x\r\n", ret);
	}

	aiMini4wdDebugPrintf("AI System was Initialized. Build at %s %s\r\n", __DATE__, __TIME__);

	char logfilename[16];
	_searchNewLogFilename(logfilename);
	sConsoleOut = aiMini4wdFsOpen(logfilename, "w");
	if (sConsoleOut == NULL) {
		__fatal_error("Failed to open logfile.\r\n");
	}

	//J 52Hz で 60秒 = 5120行
	//J 1行80文字だとして、80 x 5120 = 409,600Byte
	aiMini4wdFsSeek(sConsoleOut, 1*1024*1024);
	aiMini4wdFsSeek(sConsoleOut, 0);
	aiMini4wdFsSync(sConsoleOut);

	sScriptFile = aiMini4wdFsOpen("test.py", "r");
	if (sScriptFile == NULL) {
		__fatal_error("Script Not found.\r\n");
	}

	ret = aiMini4WdTimerRegister100msCallback(_timer_cb_100ms);
	if (ret != 0) {
		__fatal_error("Failed to set Timer callback.");
	}

	ret = aiMini4wdSensorRegisterCapturedCallback(_sensor_callback);
	if (ret != 0) {
		__fatal_error("Failed to set Sensor callback.");
	}

	ret = aiMini4wdRegisterOnStartCb(_internalTrigger);
	if (ret != 0) {
		__fatal_error("Failed to set internal trigger callback.");
	}

	ret = aiMini4wdRegisterExtInterrupt(_externalTrigger);
	if (ret != 0) {
		__fatal_error("Failed to set external trigger callback.");
	}

	sScriptFile = aiMini4wdFsOpen("BOOT.INI", "r");
	if (sScriptFile == NULL) {
		__fatal_error("Failed to open boot.ini\r\n");
	}

	char filename[256];
	ret = _searchParameter(sScriptFile, "script", filename, sizeof(filename));
	if (ret != AI_OK) {
		strcpy(filename, "default.py");
	}

	aiMini4wdFsClose(sScriptFile);
	sScriptFile = aiMini4wdFsOpen(filename, "r");
	if (sScriptFile == NULL) {
		__fatal_error("Script Not found.\r\n");
	}

    int stack_dummy;
    stack_top = (char*)&stack_dummy;

#if MICROPY_ENABLE_GC
	gc_init(heap, heap + sizeof(heap));
#endif
	mp_init();
    
soft_reset:
	for (;;) {
		if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
			if ((ret = pyexec_raw_repl()) != 0) {
				break;
			}
			//J 強制的にFlushする
			aiMini4wdFsPutsFlush(sConsoleOut);
			aiMini4wdFsTruncate(sConsoleOut);
			aiMini4wdFsSync(sConsoleOut);

			//J LEDを起動状態にしておく
			aiMini4wdPrintLedPattern(0);
		} else {
			if ((ret = pyexec_friendly_repl()) != 0) {
				break;
			}
		}
    }
	aiMini4wdFsSync(sConsoleOut);

	aiMini4wdSetErrorStatus(2);
    goto soft_reset;

    mp_deinit();

    return 0;
}


void nlr_jump_fail(void *val) {
    while (1);
}

void NORETURN __fatal_error(const char *msg) {
	//J Fatalで落ちたときにディスクを無駄にしないように修正
	aiMini4wdFsPuts(sConsoleOut, msg, strlen(msg));
	aiMini4wdFsPutsFlush(sConsoleOut);
	aiMini4wdFsTruncate(sConsoleOut);
	aiMini4wdFsSync(sConsoleOut);


	aiMini4wdSetErrorStatus(1);
	aiMini4wdDebugPrintf("[FATAL] %s\r\n", msg);
    while (1);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);


void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
//  gc_dump_info();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
// Receive single character
typedef enum AiMini4wdMicroPythonRawScriptReadState_t
{
	cAiMini4wdMicroPythonReadStateSendCtrlA = 1,
	cAiMini4wdMicroPythonReadStateWaitForEof = 2,
	cAiMini4wdMicroPythonReadStateSentCtrlD = 3,
	cAiMini4wdMicroPythonReadStateSentCtrlB = 5,
} AiMini4wdMicroPythonRawScriptReadState;

#define CTRL_A		(1)
#define CTRL_B		(2)
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
		while(1);
	}

    return ch;
}

/*---------------------------------------------------------------------------*/
// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len)
{
	aiMini4wdDebugPuts(str, len);

	//J Save to File
	aiMini4wdFsPuts(sConsoleOut, str, len);

	return;
}