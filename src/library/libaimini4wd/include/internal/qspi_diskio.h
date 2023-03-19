/*
 * qspi_diskio.h
 *
 * Created: 2023/03/01 4:12:53
 *  Author: kiyot
 */ 


#ifndef QSPI_DISKIO_H_
#define QSPI_DISKIO_H_

#include "ff.h"
#include "diskio.h"

#ifdef __cplusplus
extern "C" {
#endif

DSTATUS qspi_disk_initialize (void);
DSTATUS qspi_disk_status (void);
DRESULT qspi_disk_read (BYTE* buff, DWORD sector, UINT count);
DRESULT qspi_disk_write (const BYTE* buff, DWORD sector, UINT count);
DRESULT qspi_disk_ioctl (BYTE cmd, void* buff);
void qspi_disk_timerproc (void);

#ifdef __cplusplus
}
#endif


#endif /* QSPI_DISKIO_H_ */