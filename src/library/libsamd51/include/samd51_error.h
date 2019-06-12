/*
 * samd51_error.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef SAMD51_ERROR_H_
#define SAMD51_ERROR_H_

#define AI_OK							(0)

#define AI_ERROR_INVALID				(0xd5100001)
#define AI_ERROR_NOBUF					(0xd5100002)
#define AI_ERROR_NULL					(0xd5100003)
#define AI_ERROR_NODEV					(0xd5100004)
#define AI_ERROR_NOT_READY				(0xd5100005)
#define AI_NOT_IMPLEMENTED				(0xd5100006)
#define AI_ERROR_BUSY					(0xd5100007)
#define AI_ERROR_BAD_ADDRESS			(0xd5100008)

#define AI_ERROR_I2C_NACK				(0xd5110001)
#define AI_ERROR_I2C_BUSY				(0xd5110002)

#define AI_ERROR_DISK_NORESPONSE		(0xd5120001)
#define AI_ERROR_DISK_ERROR_RESPONSE	(0xd5120002)
#define AI_ERROR_DISC_NOT_SUPPORTED		(0xd5120003)
#define AI_ERROR_DISK_NOT_EXIST			(0xd5120004)
#define AI_ERROR_DISK_FAIL				(0xd5120005)
#define AI_ERROR_DISK_TIMEOUT			(0xd5120006)

#define AI_ERROR_FS_DISK				(0xd5130001)
#define AI_ERROR_FS_INTERNAL			(0xd5130002)
#define AI_ERROR_FS_NOT_READY			(0xd5130003)
#define AI_ERROR_FS_NO_FILE				(0xd5130004)
#define AI_ERROR_FS_NO_PATH				(0xd5130005)
#define AI_ERROR_FS_INVALID_NAME		(0xd5130006)
#define AI_ERROR_FS_DENIED				(0xd5130007)
#define AI_ERROR_FS_EXIST				(0xd5130008)
#define AI_ERROR_FS_INVALID_OBJECT		(0xd5130009)
#define AI_ERROR_FS_WRITE_PROTECRTED	(0xd513000a)
#define AI_ERROR_FS_INVALID_DRIVE		(0xd513000b)
#define AI_ERROR_FS_NOT_ENABLED			(0xd513000c)
#define AI_ERROR_FS_NO_FILESYSTEM		(0xd513000d)
#define AI_ERROR_FS_MKFS_ABORTED		(0xd513000e)
#define AI_ERROR_FS_TIMEOUT				(0xd513000f)
#define AI_ERROR_FS_LOCKED				(0xd5130010)
#define AI_ERROR_FS_NOT_ENOUGH_CORE		(0xd5130011)
#define AI_ERROR_FS_TOO_MANY_OPEN_FILES	(0xd5130012)
#define AI_ERROR_FS_INVALID_PARAMETER	(0xd5130013)
#define AI_ERROR_FS_UNKNOWN				(0xd5130014)


#define AI_ERROR_ADC_HIT_WINDOW_MONITOR (0xd5130001)
#define AI_ERROR_ADC_OVER_RUN			(0xd5130002)

#endif /* SAMD51_ERROR_H_ */