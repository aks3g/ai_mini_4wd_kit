/*
 * mini4wd.c
 *
 * Created: 2019/03/10
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"

#include "mini4wd.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC const mp_rom_map_elem_t mini4wd_module_globals_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mini4wd) },

	//J Global Functions
	{ MP_ROM_QSTR(MP_QSTR_Machine), MP_ROM_PTR(&machine_type) },
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC MP_DEFINE_CONST_DICT(mini4wd_module_globals, mini4wd_module_globals_table);
const mp_obj_module_t mini4wd_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mini4wd_module_globals,
};
