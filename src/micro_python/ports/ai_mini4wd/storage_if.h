/*
 * storage_if.h
 *
 * Created: 2019/06/07
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#ifndef STORAGE_IF_H_
#define STORAGE_IF_H_

#include <extmod/vfs_fat.h>

extern const struct _mp_obj_type_t ai_mini4wd_disk_type;
extern const struct _mp_obj_base_t ai_mini4wd_disk_obj;

void ai_mini4wd_fs_init_vfs(fs_user_mount_t *vfs);

#endif/*STORAGE_IF_H_*/
