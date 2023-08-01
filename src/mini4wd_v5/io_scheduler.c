#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "ai_mini4wd_os.h"

#include <string.h>


#include <ai_mini4wd.h>
#include <ai_mini4wd_error.h>
#include <ai_mini4wd_fs.h>
#include <ai_mini4wd_hid.h>
#include <ai_mini4wd_timer.h>
#include <ai_mini4wd_motor_driver.h>
#include <ai_mini4wd_sensor.h>
#include <ai_mini4wd_trace.h>


#define IO_SCHECULER_FLAG_WRITE_DONE		(0x00000001UL << 0)
#define IO_SCHECULER_FLAG_OK				(0x00000001UL << 1)
#define IO_SCHECULER_FLAG_ERROR				(0x00000001UL << 2)

typedef enum IoJobType_t
{
	IO_SCHEDULER_READ,
	IO_SCHEDULER_WRITE,
	IO_SCHEDULER_WRITE_REQ,
	IO_SCHEDULER_SYNC,
	IO_SCHEDULER_OPEN,
	IO_SCHEDULER_CLOSE,
	IO_SCHEDULER_LSEEK,
} IoJobType;

typedef struct IoJob_t
{
	IoJobType type;
	AiMini4wdFile *fd;
	int status;
	union {
		struct {
			uint8_t *buf;
			size_t  size;
			size_t  size_out;
		} rw;
		struct {
			const char *path;
			const char *mode;
		} open;
		struct {
			size_t size;
		} lseek;
	} opt;
} IoJob;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//J library側の関数をこちらで上書き。Libの方では__weakで空関数に繋いである
void __rtos_write_done_hook(void)
{
	iset_flg(IO_FLG, IO_SCHECULER_FLAG_WRITE_DONE);
}

/*--------------------------------------------------------------------------*/
void __rtos_wait_write_done(void)
{
	FLGPTN ptn;
	wai_flg(IO_FLG, IO_SCHECULER_FLAG_WRITE_DONE, TWF_ORW, &ptn);
}

/*--------------------------------------------------------------------------*
 *  IO Scheduler Task
 *--------------------------------------------------------------------------*/
void io_scheduler_task(intptr_t exinf)
{
	int ret=0;
	ER err;
	uint32_t tick;
	while(1) {
		get_tim((SYSTIM *)&tick);
		aiMini4wdDebugTracePush(tick, 0x11000000);

		IoJob *pjob;
		err = rcv_dtq(IO_DTQ, (intptr_t *)&pjob);

		get_tim((SYSTIM *)&tick);
		aiMini4wdDebugTracePush(tick, 0x10000000);

		if (err != E_OK) continue;

		if (pjob->type == IO_SCHEDULER_READ) {
			ret = aiMini4wdFsRead(pjob->fd, pjob->opt.rw.buf, pjob->opt.rw.size);
			if (ret >= 0) {
				pjob->opt.rw.size_out = ret;
				ret = 0;
			}
		}
		else if (pjob->type == IO_SCHEDULER_WRITE) {
			ret = aiMini4wdFsWrite(pjob->fd, pjob->opt.rw.buf, pjob->opt.rw.size);
			if (ret >= 0) {
				pjob->opt.rw.size_out = ret;
				ret = 0;
			}
		}
		else if (pjob->type == IO_SCHEDULER_WRITE_REQ) {
			IoJob job;
			memcpy(&job, pjob, sizeof(job));
			//J Non-Blockingな処理なので、ここでFlagを上げてしまう
			pjob->status = ret;
			set_flg(IO_FLG, IO_SCHECULER_FLAG_OK);

			(void)aiMini4wdFsWrite(job.fd, job.opt.rw.buf, job.opt.rw.size);
			continue;
		}
		else if (pjob->type == IO_SCHEDULER_SYNC) {
			aiMini4wdFsTruncate(pjob->fd);
			aiMini4wdFsSync(pjob->fd);
		}
		else if (pjob->type == IO_SCHEDULER_OPEN) {
			pjob->fd = aiMini4wdFsOpen(pjob->opt.open.path, pjob->opt.open.mode);
			if (pjob->fd == NULL) {
				ret = AI_ERROR_FS_NO_FILE;
			}

		}
		else if (pjob->type == IO_SCHEDULER_CLOSE) {
			aiMini4wdFsClose(pjob->fd);
			ret = AI_OK;
		}
		else if (pjob->type == IO_SCHEDULER_LSEEK) {
			ret = aiMini4wdFsSeek(pjob->fd, pjob->opt.lseek.size);
		}
		else {
			continue;
		}

		pjob->status = ret;
		if(ret) {
			set_flg(IO_FLG, IO_SCHECULER_FLAG_ERROR);
		}
		else{
			set_flg(IO_FLG, IO_SCHECULER_FLAG_OK);
		}
	}
}

static int _send_job(IoJob *job)
{
	FLGPTN ptn;

	ER err = snd_dtq(IO_DTQ, (intptr_t)job);
	if (err != E_OK) {
		return AI_ERROR_NOT_READY;
	}

	//J 処理完了を待つ
	err = wai_flg(IO_FLG, IO_SCHECULER_FLAG_ERROR | IO_SCHECULER_FLAG_OK, TWF_ORW, &ptn);
	if (err != E_OK) {
		return AI_ERROR_NOT_READY;
	}

	return AI_OK;
}


/*--------------------------------------------------------------------------*/
int ioSchedulerRead(AiMini4wdFile *fd, void *buf, size_t size)
{
	if (fd==NULL || buf==NULL || size==0) {
		return AI_ERROR_NULL;
	}

	IoJob job;
	job.type = IO_SCHEDULER_READ;
	job.fd = fd;
	job.opt.rw.buf = buf;
	job.opt.rw.size = size;

	int ret = _send_job(&job);
	if (ret != AI_OK) {
		return ret;
	}

	if (job.status == 0) {
		return job.opt.rw.size_out;
	}
	else {
		return job.status;
	}
}

/*--------------------------------------------------------------------------*/
int ioSchedulerWrite(AiMini4wdFile *fd, const void *buf, size_t size)
{
	if (fd==NULL || buf==NULL || size==0) {
		return AI_ERROR_NULL;
	}

	IoJob job;
	job.type = IO_SCHEDULER_WRITE;
	job.fd = fd;
	job.opt.rw.buf = (void *)buf;
	job.opt.rw.size = size;

	int ret = _send_job(&job);
	if (ret != AI_OK) {
		return ret;
	}

	if (job.status == 0) {
		return job.opt.rw.size_out;
	}
	else {
		return job.status;
	}
}

/*--------------------------------------------------------------------------*/
int ioSchedulerWriteReq(AiMini4wdFile *fd, const void *buf, size_t size)
{
	if (fd==NULL || buf==NULL || size==0) {
		return AI_ERROR_NULL;
	}

	IoJob job;
	job.type = IO_SCHEDULER_WRITE_REQ;
	job.fd = fd;
	job.opt.rw.buf = (void *)buf;
	job.opt.rw.size = size;

	int ret = _send_job(&job);
	if (ret != AI_OK) {
		return ret;
	}

	return job.status;
}

/*--------------------------------------------------------------------------*/
int ioSchedulerLseek(AiMini4wdFile *fd, size_t offset)
{
	if (fd==NULL) {
		return AI_ERROR_NULL;
	}

	IoJob job;
	job.type = IO_SCHEDULER_LSEEK;
	job.fd = fd;
	job.opt.lseek.size = offset;

	int ret = _send_job(&job);
	if (ret != AI_OK) {
		return ret;
	}

	return job.status;
}

/*--------------------------------------------------------------------------*/
int ioSchedulerTell(AiMini4wdFile *fd)
{
	if (fd==NULL) {
		return AI_ERROR_NULL;
	}

	return aiMini4wdFsTell(fd);
}

/*--------------------------------------------------------------------------*/
int ioSchedulerSize(AiMini4wdFile *fd)
{
	if (fd==NULL) {
		return AI_ERROR_NULL;
	}

	return aiMini4wdFsSize(fd);
}

/*--------------------------------------------------------------------------*/
int ioSchedulerSync(AiMini4wdFile *fd)
{
	if (fd==NULL) {
		return AI_ERROR_NULL;
	}

	IoJob job;
	job.type = IO_SCHEDULER_SYNC;
	job.fd = fd;

	int ret = _send_job(&job);
	if (ret != AI_OK) {
		return ret;
	}

	return job.status;
}

/*--------------------------------------------------------------------------*/
int ioSchedulerClose(AiMini4wdFile *fd)
{
	if (fd==NULL) {
		return AI_ERROR_NULL;
	}

	IoJob job;
	job.type = IO_SCHEDULER_CLOSE;
	job.fd = fd;

	int ret = _send_job(&job);
	if (ret != AI_OK) {
		return ret;
	}

	return job.status;
}

/*--------------------------------------------------------------------------*/
AiMini4wdFile *ioSchedulerOpen(const char *path, const char *mode)
{
	FLGPTN ptn;

	if (path==NULL || mode==NULL) {
		return AI_ERROR_NULL;
	}

	IoJob job;
	job.type = IO_SCHEDULER_OPEN;
	job.fd = NULL;
	job.opt.open.path=path;
	job.opt.open.mode=mode;

	//J IO Schedulerに、Writeを要求
	ER err = snd_dtq(IO_DTQ, (intptr_t)&job);
	if (err != E_OK) {
		return AI_ERROR_NOT_READY;
	}

	//J 処理完了を待つ
	err = wai_flg(IO_FLG, IO_SCHECULER_FLAG_ERROR | IO_SCHECULER_FLAG_OK, TWF_ORW, &ptn);
	if (err != E_OK) {
		return AI_ERROR_NOT_READY;
	}
	return job.fd;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static AiMini4wdFile *sConsoleOut = NULL;
static char sPutsBuf[2][4096];
static int sActiveBufIndex = 0;
static size_t sBufferdSize = 0;

/*--------------------------------------------------------------------------*/
int ioSchedulerInitPutc(AiMini4wdFile *file)
{
	sConsoleOut = file;

	return 0;
}


/*--------------------------------------------------------------------------*/
int ioSchedulerPuts(const char *str, size_t len)
{
	if (sConsoleOut == NULL) {
		return AI_ERROR_NULL;
	}

	while (len > 0) {
		size_t cpySize = ((len + sBufferdSize) >  sizeof(sPutsBuf[sActiveBufIndex])) ? sizeof(sPutsBuf[sActiveBufIndex])-sBufferdSize : len;
		memcpy(&(sPutsBuf[sActiveBufIndex][sBufferdSize]), str, cpySize);		

		sBufferdSize += cpySize;
		if (sBufferdSize >= sizeof(sPutsBuf[sActiveBufIndex])) {
			sBufferdSize = 0;
			ioSchedulerWriteReq(sConsoleOut, sPutsBuf[sActiveBufIndex], sizeof(sPutsBuf[sActiveBufIndex]));

			sActiveBufIndex = 1-sActiveBufIndex;
		}
		str += cpySize;
		len -= cpySize;
	}

	return 0;
}

/*--------------------------------------------------------------------------*/
int ioSchedulerFinalizePutc(void)
{
	FLGPTN ptn;

	if (sConsoleOut == NULL) {
		return AI_ERROR_NULL;
	}

	//J バッファに残っている文字列をFlushする
	ioSchedulerWrite(sConsoleOut, sPutsBuf[sActiveBufIndex], sBufferdSize);

	IoJob job;
	job.type = IO_SCHEDULER_SYNC;
	job.fd = sConsoleOut;

	//J IO Schedulerに、SYNCを要求
	ER err = snd_dtq(IO_DTQ, (intptr_t)&job);
	if (err != E_OK) {
		return AI_ERROR_NOT_READY;
	}

	//J 処理完了を待つ
	err = wai_flg(IO_FLG, IO_SCHECULER_FLAG_ERROR | IO_SCHECULER_FLAG_OK, TWF_ORW, &ptn);
	if (err != E_OK) {
		return AI_ERROR_NOT_READY;
	}

	sConsoleOut = NULL;

	return job.status;
}