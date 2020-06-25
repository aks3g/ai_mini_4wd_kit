/*
 * ai_mini4wd_fs.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef AI_MINI4WD_FS_H_
#define AI_MINI4WD_FS_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef void AiMini4wdFile;
typedef void AiMini4wdDir;

#define	AI_MINI4WD_FS_RDO	0x01	/* Read only */
#define	AI_MINI4WD_FS_HID	0x02	/* Hidden    */
#define	AI_MINI4WD_FS_SYS	0x04	/* System    */
#define AI_MINI4WD_FS_DIR	0x10	/* Directory */
#define AI_MINI4WD_FS_ARC	0x20	/* Archive   */

typedef struct AiMini4wdFileInfo_t {
	size_t size;
	uint8_t attr;
	char name[256];
} AiMini4wdFileInfo;

AiMini4wdFile *aiMini4wdFsOpen(const char *path, const char *mode);
void aiMini4wdFsClose(AiMini4wdFile *file);

int aiMini4wdFsRead(AiMini4wdFile *file, void *buf, size_t size);
int aiMini4wdFsWrite(AiMini4wdFile *file, const void *buf, size_t size);

int aiMini4wdFsSeek(AiMini4wdFile *file, int32_t offset);
int aiMini4wdFsSize(AiMini4wdFile *file);
int aiMini4wdFsEof(AiMini4wdFile *file);

int aiMini4wdFsSync(AiMini4wdFile *file);
int aiMini4wdFsStat(const char *path, AiMini4wdFileInfo *info);
int aiMini4wdFsTruncate(AiMini4wdFile *file);

int aiMini4wdFsPrintf(AiMini4wdFile *file, const char *str, ...);
int aiMini4wdFsPuts(AiMini4wdFile *file, const char *str, size_t len);
int aiMini4wdFsPutsFlush(AiMini4wdFile *file);
char *aiMini4wdFsGets(AiMini4wdFile *file, char *buf, size_t len);

AiMini4wdDir *aiMini4wdFsOpenDir(const char *path);
void aiMini4wdFsCloseDir(AiMini4wdDir *dir);
int aiMini4wdFsReadDir(AiMini4wdDir *dir, AiMini4wdFileInfo *file);

#ifdef __cplusplus
}
#endif

#endif /* AI_MINI4WD_FS_H_ */