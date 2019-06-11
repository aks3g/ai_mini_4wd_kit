/*
 * storage_if.c
 *
 * Created: 2019/06/07
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"

#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"

#include "storage_if.h"

#include "samd51_sdhc.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
extern Samd51_sdhcCardContext *aiMini4wdGetSdhcCardContextFromSDK_only_for_microPython_porting(void);


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
mp_uint_t _ai_mini4wd_disk_read(uint8_t *dest, uint32_t block_num, uint32_t num_blocks)
{
	Samd51_sdhcCardContext *ctx = aiMini4wdGetSdhcCardContextFromSDK_only_for_microPython_porting();
	
	int ret = samd51_transfer(SAMD51_SDHC0, ctx, 1, block_num, (void *)dest, num_blocks * 512, 1);
	
	return ret;
}


/*---------------------------------------------------------------------------*/
mp_uint_t _ai_mini4wd_disk_write(const uint8_t *src, uint32_t block_num, uint32_t num_blocks)
{
	Samd51_sdhcCardContext *ctx = aiMini4wdGetSdhcCardContextFromSDK_only_for_microPython_porting();
	
	int ret = samd51_transfer(SAMD51_SDHC0, ctx, 0, block_num, (void *)src, num_blocks * 512, 0);
	
	return ret;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
const mp_obj_base_t ai_mini4wd_disk_obj = {&ai_mini4wd_disk_type};

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t ai_mini4wd_disk_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&ai_mini4wd_disk_obj;
}

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t ai_mini4wd_disk_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
	mp_uint_t ret = 0;
	mp_buffer_info_t bufinfo;

	mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
	ret = _ai_mini4wd_disk_read(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / 512);
	return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ai_mini4wd_disk_readblocks_obj, ai_mini4wd_disk_readblocks);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t ai_mini4wd_disk_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
	mp_uint_t ret = 0;
	mp_buffer_info_t bufinfo;

	mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
	ret = _ai_mini4wd_disk_write(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / 512);
	return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ai_mini4wd_disk_writeblocks_obj, ai_mini4wd_disk_writeblocks);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t ai_mini4wd_disk_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
	mp_int_t cmd = mp_obj_get_int(cmd_in);
	switch (cmd) {
	case BP_IOCTL_INIT:
		return MP_OBJ_NEW_SMALL_INT(0); // success
	case BP_IOCTL_DEINIT:
		return MP_OBJ_NEW_SMALL_INT(0); // success
	case BP_IOCTL_SYNC:
		return MP_OBJ_NEW_SMALL_INT(0); // success
	case BP_IOCTL_SEC_COUNT:
		///TODO サイズを取得するAPIをSDK側で作って呼ぶようにする
		return MP_OBJ_NEW_SMALL_INT(4096*1024 / 512);
	case BP_IOCTL_SEC_SIZE:
		//TODO セクタサイズを取得するAPIを実装する
		return MP_OBJ_NEW_SMALL_INT(512);
	default: // unknown command
		return MP_OBJ_NEW_SMALL_INT(-1); // error
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ai_mini4wd_disk_ioctl_obj, ai_mini4wd_disk_ioctl);



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC const mp_rom_map_elem_t ai_mini4wd_disk_locals_dict_table[] = {
	// block device protocol
	{ MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&ai_mini4wd_disk_readblocks_obj) },
	{ MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&ai_mini4wd_disk_writeblocks_obj) },
	{ MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&ai_mini4wd_disk_ioctl_obj) }
};

STATIC MP_DEFINE_CONST_DICT(ai_mini4wd_disk_locals_dict, ai_mini4wd_disk_locals_dict_table);


const mp_obj_type_t ai_mini4wd_disk_type = {
    { &mp_type_type },
    .name = MP_QSTR_ai_mini4wd_disk,
    .make_new = ai_mini4wd_disk_make_new,
    .locals_dict = (mp_obj_dict_t*)&ai_mini4wd_disk_locals_dict,
};



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void ai_mini4wd_fs_init_vfs(fs_user_mount_t *vfs) {
	vfs->base.type = &mp_fat_vfs_type;
	vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
	vfs->fatfs.drv = vfs;
	vfs->readblocks[0] = (mp_obj_t)&ai_mini4wd_disk_readblocks_obj;
	vfs->readblocks[1] = (mp_obj_t)&ai_mini4wd_disk_obj;
	vfs->readblocks[2] = (mp_obj_t)_ai_mini4wd_disk_read; // native version
	vfs->writeblocks[0] = (mp_obj_t)&ai_mini4wd_disk_writeblocks_obj;
	vfs->writeblocks[1] = (mp_obj_t)&ai_mini4wd_disk_obj;
	vfs->writeblocks[2] = (mp_obj_t)_ai_mini4wd_disk_write; // native version
	vfs->u.ioctl[0] = (mp_obj_t)&ai_mini4wd_disk_ioctl_obj;
	vfs->u.ioctl[1] = (mp_obj_t)&ai_mini4wd_disk_obj;

	return;
}