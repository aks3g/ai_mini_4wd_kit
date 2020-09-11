/*
 * machine_learning.c
 *
 * Created: 2019/08/15
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/gc.h"

#include "mini4wd.h"

#include <ai_mini4wd.h>

typedef struct _machine_learning_obj_t {
    mp_obj_base_t base;

	int len;
	float *policy_of_velocity;
	float *score_table;
	float *ave_velocity;

	int *velocity_cnt;
	int *score_table_cnt;

	float leaning_step;
	int propagation_window;
	float time_constant_s;
	int unit_mm;
	float sigma;
} machine_learning_obj_t;

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t machine_learning_make_new(const mp_obj_type_t *type_in, size_t n_args, 
	size_t n_kw, const mp_obj_t *args)
{
	mp_arg_check_num(n_args, n_kw, 6, 6, false);

	machine_learning_obj_t *o = m_new_obj(machine_learning_obj_t);

	memset(o, 0, sizeof(machine_learning_obj_t));
	o->base.type = &machine_learning_type;

	o->len = mp_obj_get_int(args[0]);
	o->propagation_window = mp_obj_get_int(args[1]);
	o->leaning_step = mp_obj_get_float(args[2]);
	o->time_constant_s = mp_obj_get_float(args[3]);
	o->unit_mm = mp_obj_get_int(args[4]);
	float init_velocity = mp_obj_get_float(args[5]);
	o->sigma = mp_obj_get_float(args[6]);

	o->policy_of_velocity = m_new(float,   o->len);

	o->velocity_cnt       = m_new(int,     o->len);
	o->ave_velocity       = m_new(float,   o->len);

	o->score_table_cnt    = m_new(int,     o->len);
	o->score_table        = m_new(float,   o->len);

	for (int i=0 ; i<o->len ; ++i) {
		o->policy_of_velocity[i] = init_velocity;
		o->score_table_cnt[i] = 0;
		o->velocity_cnt[i] = 0;
		o->ave_velocity[i] = 0;
		o->score_table[i] = 0;
	}

	return MP_OBJ_FROM_PTR(o);
}

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t ml_set_reward(size_t n, const mp_obj_t *args)
{
	machine_learning_obj_t *o = (machine_learning_obj_t *)args[0];

//	mp_arg_check_num(n_args, n_kw, 3, 3, false);

	int pos = mp_obj_get_int(args[1]);
	float evaluation_value = mp_obj_get_float(args[2]);
	float velocity         = mp_obj_get_float(args[3]);

	float delta_mm = o->time_constant_s * velocity * (1000000.0f / 3600.0f);
	int idx = delta_mm / o->unit_mm;

	if (pos < 0 || pos >= o->len) {
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Out of range."));
	}

	for (int i=0;i<o->propagation_window ; ++i) {
		int p = (o->len + pos - idx - i) % o->len;
		o->score_table[p] += evaluation_value;
		o->score_table_cnt[p]++;
	}

	o->ave_velocity[pos] += velocity;
	o->velocity_cnt[pos]++;

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_set_reward_obj, 4, ml_set_reward);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t ml_get_policy(mp_obj_t self_in, mp_obj_t pos)
{
	machine_learning_obj_t *o = (machine_learning_obj_t *)self_in;

	int p = mp_obj_get_int(pos);
	if (p < 0 || p >= o->len) {
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Out of range."));
	}

	return mp_obj_new_float(o->policy_of_velocity[p]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ml_get_policy_obj, ml_get_policy);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t ml_set_policy(mp_obj_t self_in, mp_obj_t pos, mp_obj_t policy)
{
	machine_learning_obj_t *o = (machine_learning_obj_t *)self_in;

	int p = mp_obj_get_int(pos);
	float velocity = mp_obj_get_float(policy);

	if (p < 0 || p >= o->len) {
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Out of range."));
	}

	o->policy_of_velocity[p] = velocity;

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ml_set_policy_obj, ml_set_policy);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t ml_update(mp_obj_t self_in, mp_obj_t algorithm)
{
	machine_learning_obj_t *o = (machine_learning_obj_t *)self_in;
	int _algorithm = mp_obj_get_int(algorithm);
	(void)_algorithm;

	for (int i=0 ; i<o->len ; ++i) {
		o->ave_velocity[i] = o->ave_velocity[i] / o->velocity_cnt[i];
		o->score_table[i] = o->score_table[i] / o->score_table_cnt[i];
	}

	for (int i=0 ; i<o->len ; ++i) {
		if (o->velocity_cnt[i] == 0 || o->score_table_cnt[i] == 0) {
		}
		else {
			float delta = (o->policy_of_velocity[i] - o->ave_velocity[i]);
			o->policy_of_velocity[i] = o->policy_of_velocity[i] + 
				(float)exp(-(double)(delta * delta)/(double)(2*o->sigma*o->sigma)) * (o->score_table[i] + o->leaning_step);
		}
	}

	memset(o->ave_velocity, 0x00, sizeof(o->ave_velocity[0])*o->len);
	memset(o->velocity_cnt, 0x00, sizeof(o->velocity_cnt[0])*o->len);
	memset(o->score_table,  0x00, sizeof(o->score_table[0])*o->len);
	memset(o->score_table_cnt, 0x00, sizeof(o->score_table_cnt[0])*o->len);

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ml_update_obj, ml_update);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC const mp_rom_map_elem_t machine_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_set_reward),	MP_ROM_PTR(&ml_set_reward_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_policy),	MP_ROM_PTR(&ml_get_policy_obj) },
	{ MP_ROM_QSTR(MP_QSTR_set_policy),	MP_ROM_PTR(&ml_set_policy_obj) },
	{ MP_ROM_QSTR(MP_QSTR_update),	MP_ROM_PTR(&ml_update_obj) },
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void machine_learning_state_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
	;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC MP_DEFINE_CONST_DICT(machine_locals_dict, machine_locals_dict_table);

const mp_obj_type_t machine_learning_type = {
    { &mp_type_type },
    .name = MP_QSTR_MachineLearning,
    .print = machine_learning_state_print,
    .make_new = machine_learning_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_locals_dict,
};
