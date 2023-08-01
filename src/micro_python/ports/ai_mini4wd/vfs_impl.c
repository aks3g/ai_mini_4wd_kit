#include <string.h>
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "lib/timeutils/timeutils.h"

#include <ai_mini4wd.h>
#include <ai_mini4wd_error.h>
#include <ai_mini4wd_fs.h>
#include <io_scheduler.h>

#include "vfs_impl.h"

STATIC mp_obj_t ai_mini4wd_vfs_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
	mp_arg_check_num(n_args, n_kw, 1, 1, false);

	// create new object
	fs_usr_mount_t *vfs = m_new_obj(fs_usr_mount_t);
	vfs->base.type = type;

	return MP_OBJ_FROM_PTR(vfs);
}


STATIC mp_obj_t ai_mini4wd_vfs_mkfs(mp_obj_t bdev_in) {
	return mp_const_none;
}

STATIC mp_obj_t ai_mini4wd_vfs_ilistdir_func(size_t n_args, const mp_obj_t *args)
{
	return mp_const_none;
}

STATIC mp_obj_t ai_mini4wd_vfs_remove(mp_obj_t vfs_in, mp_obj_t path_in)
{
	return mp_const_none;
}

STATIC mp_obj_t ai_mini4wd_vfs_rmdir(mp_obj_t vfs_in, mp_obj_t path_in)
{
	return mp_const_none;
}

STATIC mp_obj_t ai_mini4wd_vfs_rename(mp_obj_t vfs_in, mp_obj_t path_in, mp_obj_t path_out)
{
	return mp_const_none;
}

STATIC mp_obj_t ai_mini4wd_vfs_mkdir(mp_obj_t vfs_in, mp_obj_t path_o)
{
	return mp_const_none;
}

STATIC mp_obj_t ai_mini4wd_vfs_chdir(mp_obj_t vfs_in, mp_obj_t path_in)
{
	return mp_const_none;
}

STATIC mp_obj_t ai_mini4wd_vfs_getcwd(mp_obj_t vfs_in)
{
	return mp_const_none;
}

STATIC mp_obj_t ai_mini4wd_vfs_stat(mp_obj_t vfs_in, mp_obj_t path_in)
{
	return mp_const_none;
}

STATIC mp_obj_t ai_mini4wd_vfs_statvfs(mp_obj_t vfs_in, mp_obj_t path_in)
{
	return mp_const_none;
}

STATIC mp_obj_t vfs_ai_mini4wd_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs)
{
	return mp_const_none;
}

STATIC mp_obj_t vfs_ai_mini4wd_umount(mp_obj_t self_in)
{
	return mp_const_none;
}



/*------------------------------------------------------------------------------
 *
 */
typedef struct _pyb_file_obj_t {
	mp_obj_base_t base;
	AiMini4wdFile *fp;
} pyb_file_obj_t;

static int _convert_errno(uint32_t errcode)
{
	switch (errcode)
	{
	case AI_OK							:
		return 0;

	case AI_ERROR_FS_DISK				:
	case AI_ERROR_FS_INTERNAL			:
	case AI_ERROR_FS_MKFS_ABORTED		:
	case AI_ERROR_FS_TIMEOUT			:
	case AI_ERROR_FS_LOCKED				:
		return MP_EIO;

	case AI_ERROR_FS_NOT_READY			:
		return MP_EBUSY;

	case AI_ERROR_FS_NO_FILE			:
	case AI_ERROR_FS_NO_PATH			:
		return MP_ENOENT;

	case AI_ERROR_FS_DENIED				:
		return MP_EACCES;

	case AI_ERROR_FS_EXIST				:
		return MP_EEXIST;

	case AI_ERROR_FS_WRITE_PROTECRTED	:
		return MP_EROFS;

	case AI_ERROR_FS_INVALID_DRIVE		:
	case AI_ERROR_FS_NOT_ENABLED		:
	case AI_ERROR_FS_NO_FILESYSTEM		:
		return MP_ENODEV;

	case AI_ERROR_FS_NOT_ENOUGH_CORE	:
		return MP_ENOMEM;

	case AI_ERROR_FS_TOO_MANY_OPEN_FILES:
		return MP_EMFILE;

	case AI_ERROR_FS_INVALID_NAME		:
	case AI_ERROR_FS_INVALID_OBJECT		:
	case AI_ERROR_FS_INVALID_PARAMETER	:
	case AI_ERROR_FS_UNKNOWN			:
	default:
		return MP_EINVAL;
	}
}

STATIC void file_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
	(void)kind;
	mp_printf(print, "<io.%s %p>", mp_obj_get_type_str(self_in), MP_OBJ_TO_PTR(self_in));
}

STATIC mp_uint_t file_obj_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
	pyb_file_obj_t *self = MP_OBJ_TO_PTR(self_in);
	int res = ioSchedulerRead(self->fp, buf, size);
	if (res < 0) {
		*errcode = _convert_errno(res);
		return MP_STREAM_ERROR;
	}
	return (mp_uint_t)res;
}

STATIC mp_uint_t file_obj_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
	pyb_file_obj_t *self = MP_OBJ_TO_PTR(self_in);
	int res = ioSchedulerWrite(self->fp, buf, size);
	if (res < 0) {
		*errcode = _convert_errno(res);
		return MP_STREAM_ERROR;
	}
	else if ((size_t)res != size) {
		// The FatFS documentation says that this means disk full.
		*errcode = MP_ENOSPC;
		return MP_STREAM_ERROR;
	}

	return (mp_uint_t)res;
}


STATIC mp_obj_t file_obj___exit__(size_t n_args, const mp_obj_t *args) {
	(void)n_args;
	return mp_stream_close(args[0]);
}

STATIC mp_uint_t file_obj_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
	pyb_file_obj_t *self = MP_OBJ_TO_PTR(o_in);

	if (request == MP_STREAM_SEEK) {
		struct mp_stream_seek_t *s = (struct mp_stream_seek_t*)(uintptr_t)arg;

		switch (s->whence) {
			case 0: // SEEK_SET
				ioSchedulerLseek(self->fp, s->offset);
				break;

			case 1: // SEEK_CUR
				ioSchedulerLseek(self->fp, ioSchedulerTell(self->fp) + s->offset);
				break;

			case 2: // SEEK_END
				ioSchedulerLseek(self->fp, ioSchedulerSize(self->fp) + s->offset);
				break;
		}

		s->offset = ioSchedulerTell(self->fp);
		return 0;

	} else if (request == MP_STREAM_FLUSH) {
		int res = ioSchedulerSync(self->fp);
		if (res != 0) {
			*errcode = _convert_errno(res);
			return MP_STREAM_ERROR;
		}
		return 0;

	} else if (request == MP_STREAM_CLOSE) {
		// if fs==NULL then the file is closed and in that case this method is a no-op
		if (!aiMini4wdFsIsClosed(self->fp)) {
			int res = ioSchedulerClose(self->fp);
			if (res != 0) {
				*errcode = _convert_errno(res);
				return MP_STREAM_ERROR;
			}
		}
		return 0;

	} else {
		*errcode = MP_EINVAL;
		return MP_STREAM_ERROR;
	}
}


STATIC mp_obj_t file_open(fs_usr_mount_t *vfs, const mp_obj_type_t *type, mp_arg_val_t *args) {
	const char *fname = mp_obj_str_get_str(args[0].u_obj);
	const char *mode_s = mp_obj_str_get_str(args[1].u_obj);

	pyb_file_obj_t *o = m_new_obj_with_finaliser(pyb_file_obj_t);
	o->base.type = type;

	assert(vfs != NULL);
	o->fp = ioSchedulerOpen(fname, mode_s);
	if (o->fp == NULL) {
		m_del_obj(pyb_file_obj_t, o);
		mp_raise_OSError(MP_ENOENT);
	}

	return MP_OBJ_FROM_PTR(o);
}

STATIC const mp_arg_t file_open_args[] = {
	{ MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
	{ MP_QSTR_mode, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_QSTR(MP_QSTR_r)} },
	{ MP_QSTR_encoding, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
};
#define FILE_OPEN_NUM_ARGS MP_ARRAY_SIZE(file_open_args)

STATIC mp_obj_t file_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
	mp_arg_val_t arg_vals[FILE_OPEN_NUM_ARGS];
	mp_arg_parse_all_kw_array(n_args, n_kw, args, FILE_OPEN_NUM_ARGS, file_open_args, arg_vals);
	return file_open(NULL, type, arg_vals);
}



STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(file_obj___exit___obj, 4, 4, file_obj___exit__);

STATIC const mp_rom_map_elem_t rawfile_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
	{ MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
	{ MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
	{ MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
	{ MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
	{ MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
	{ MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
	{ MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
	{ MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
	{ MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
	{ MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
	{ MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&file_obj___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(rawfile_locals_dict, rawfile_locals_dict_table);

STATIC const mp_stream_p_t textio_stream_p = {
	.read = file_obj_read,
	.write = file_obj_write,
	.ioctl = file_obj_ioctl,
	.is_text = true,
};

const mp_obj_type_t mp_type_textio = {
	{ &mp_type_type },
	.name = MP_QSTR_TextIOWrapper,
	.print = file_obj_print,
	.make_new = file_obj_make_new,
	.getiter = mp_identity_getiter,
	.iternext = mp_stream_unbuffered_iter,
	.protocol = &textio_stream_p,
	.locals_dict = (mp_obj_dict_t*)&rawfile_locals_dict,
};



// Factory function for I/O stream classes
STATIC mp_obj_t ai_mini4wd_builtin_open_self(mp_obj_t self_in, mp_obj_t path, mp_obj_t mode) {
	fs_usr_mount_t *self = MP_OBJ_TO_PTR(self_in);
	mp_arg_val_t arg_vals[FILE_OPEN_NUM_ARGS];
	arg_vals[0].u_obj = path;
	arg_vals[1].u_obj = mode;
	arg_vals[2].u_obj = mp_const_none;

	return file_open(self, &mp_type_textio, arg_vals);
}
MP_DEFINE_CONST_FUN_OBJ_3(ai_mini4wd_vfs_open_obj, ai_mini4wd_builtin_open_self);

//STATIC MP_DEFINE_CONST_FUN_OBJ_1(ai_mini4wd_vfs_del_obj, ai_mini4wd_vfs_del);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ai_mini4wd_vfs_mkfs_fun_obj, ai_mini4wd_vfs_mkfs);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(ai_mini4wd_vfs_mkfs_obj, MP_ROM_PTR(&ai_mini4wd_vfs_mkfs_fun_obj));
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ai_mini4wd_vfs_ilistdir_obj, 1, 2, ai_mini4wd_vfs_ilistdir_func);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ai_mini4wd_vfs_remove_obj, ai_mini4wd_vfs_remove);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ai_mini4wd_vfs_rmdir_obj, ai_mini4wd_vfs_rmdir);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ai_mini4wd_vfs_rename_obj, ai_mini4wd_vfs_rename);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ai_mini4wd_vfs_mkdir_obj, ai_mini4wd_vfs_mkdir);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ai_mini4wd_vfs_chdir_obj, ai_mini4wd_vfs_chdir);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ai_mini4wd_vfs_getcwd_obj, ai_mini4wd_vfs_getcwd);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ai_mini4wd_vfs_stat_obj, ai_mini4wd_vfs_stat);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ai_mini4wd_vfs_statvfs_obj, ai_mini4wd_vfs_statvfs);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_ai_mini4wd_mount_obj, vfs_ai_mini4wd_mount);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ai_mini4wd_vfs_umount_obj, vfs_ai_mini4wd_umount);


STATIC const mp_rom_map_elem_t ai_mini4wd_vfs_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_mkfs), MP_ROM_PTR(&ai_mini4wd_vfs_mkfs_obj) },
	{ MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&ai_mini4wd_vfs_open_obj) },
	{ MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&ai_mini4wd_vfs_ilistdir_obj) },
	{ MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&ai_mini4wd_vfs_mkdir_obj) },
	{ MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&ai_mini4wd_vfs_rmdir_obj) },
	{ MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&ai_mini4wd_vfs_chdir_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&ai_mini4wd_vfs_getcwd_obj) },
	{ MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&ai_mini4wd_vfs_remove_obj) },
	{ MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&ai_mini4wd_vfs_rename_obj) },
	{ MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&ai_mini4wd_vfs_stat_obj) },
	{ MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&ai_mini4wd_vfs_statvfs_obj) },
	{ MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&vfs_ai_mini4wd_mount_obj) },
	{ MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&ai_mini4wd_vfs_umount_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ai_mini4wd_vfs_locals_dict, ai_mini4wd_vfs_locals_dict_table);

const mp_obj_type_t mp_ai_mini4wd_vfs_type = {
	{ &mp_type_type },
	.name = MP_QSTR_VfsAiMini4wd,
	.make_new = ai_mini4wd_vfs_make_new,
	.locals_dict = (mp_obj_dict_t*)&ai_mini4wd_vfs_locals_dict,
};
