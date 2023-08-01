/*
 * IO Scheduler
 */

#ifndef _IO_SCHEDULER_H
#define _IO_SCHEDULER_H

void io_scheduler_task(intptr_t exinf);

int ioSchedulerRead(AiMini4wdFile *fd, void *buf, size_t size);
int ioSchedulerWrite(AiMini4wdFile *fd, const void *buf, size_t size);
int ioSchedulerWriteReq(AiMini4wdFile *fd, const void *buf, size_t size);
int ioSchedulerInitPutc(AiMini4wdFile *file);
int ioSchedulerPuts(const char *str, size_t len);
int ioSchedulerFinalizePutc(void);

int ioSchedulerLseek(AiMini4wdFile *fd, size_t offset);
size_t ioSchedulerTell(AiMini4wdFile *fd);
size_t ioSchedulerSize(AiMini4wdFile *fd);
int ioSchedulerSync(AiMini4wdFile *fd);
int ioSchedulerClose(AiMini4wdFile *fd);

AiMini4wdFile *ioSchedulerOpen(const char *path, const char *mode);

#endif
