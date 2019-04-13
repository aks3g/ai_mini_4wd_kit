/*
 * sdhc_diskio.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SDHC_DISKIO_H_
#define SDHC_DISKIO_H_

#include "ff.h"
#include "diskio.h"

#ifdef __cplusplus
extern "C" {
#endif

DSTATUS sdhc_disk_initialize (void);
DSTATUS sdhc_disk_status (void);
DRESULT sdhc_disk_read (BYTE* buff, DWORD sector, UINT count);
DRESULT sdhc_disk_write (const BYTE* buff, DWORD sector, UINT count);
DRESULT sdhc_disk_ioctl (BYTE cmd, void* buff);
void sdhc_disk_timerproc (void);

#ifdef __cplusplus
}
#endif


#endif /* SDHC_DISKIO_H_ */