/*
 * vfs_impl.h
 *
 * Created: 2023/07/25
 * Copyright 2023 Kiyotaka Akasaka. All rights reserved.
 */ 
#ifndef VFS_IMPL_H_
#define VFS_IMPL_H_

typedef struct fs_usr_mount_t
{
    mp_obj_base_t base;
    uint16_t flags;
} fs_usr_mount_t;

extern const mp_obj_type_t mp_ai_mini4wd_vfs_type;

#endif/*VFS_IMPL_H_*/