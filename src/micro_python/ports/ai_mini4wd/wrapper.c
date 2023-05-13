/*
 * wrapper.c
 *
 * Created: 2023/05/02
 * Copyright 2023 Kiyotaka Akasaka. All rights reserved.
 */ 

#include <stdio.h>
#include <string.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "lib/utils/pyexec.h"

void mpython_wrap_null_hook(void) {}

void mpython_wrap_start_hook(void) __attribute__ ((weak, alias("mpython_wrap_null_hook")));
void mpython_wrap_end_hook(void)   __attribute__ ((weak, alias("mpython_wrap_null_hook")));

int mpython_is_raw_repl_mode(void)
{
	if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
		return 1;
	}
	else {
		return 0;
	}
}

static uint32_t _stack_ptr;
static uint32_t _stack_size;
int mpython_wrap_init(uint32_t stack_ptr, uint32_t stack_size, void *heap_start, void *heap_end)
{
	_stack_ptr = stack_ptr;
	_stack_size = stack_size;

	gc_init(heap_start, heap_end);
	mp_init();

	return 0;
}

void mpython_wrap_deinit(void)
{
    mp_deinit();
}

mp_uint_t gc_helper_get_regs_and_sp(mp_uint_t *regs);
void gc_collect(void) {
	mpython_wrap_start_hook();

	gc_collect_start();

	//Get Stack info from CPU
	mp_uint_t regs[10];
	mp_uint_t sp = gc_helper_get_regs_and_sp(regs);
	uint32_t stack_top = _stack_ptr + _stack_size;

	gc_collect_root((void**)sp, ((uint32_t)(stack_top) - sp) / sizeof(uint32_t));

	gc_collect_end();

	mpython_wrap_end_hook();
}


void nlr_jump_fail(void *val) {
    while (1);
}
