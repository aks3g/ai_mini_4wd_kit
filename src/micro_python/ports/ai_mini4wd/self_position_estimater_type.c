/*
 * self_position_estimater_type.c
 *
 * Created: 2019/08/15
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/gc.h"

#include "mini4wd.h"

#include <ai_mini4wd.h>



typedef struct _spe_obj_t {
	mp_obj_base_t base;

	int      max_len;
	int      len;
	float    *probability;
	uint8_t  *feature;

	uint16_t *feature_count_table;
	float    feature_weight;

	int      unit_mm;
	float    milage_mm;
	
	int      latest_pos;
} spe_obj_t;

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t self_position_estimater_make_new(const mp_obj_type_t *type_in, size_t n_args, 
	size_t n_kw, const mp_obj_t *args)
{
	spe_obj_t *o = m_new_obj(spe_obj_t);
	
	memset(o, 0, sizeof(spe_obj_t));
	o->base.type = &spe_type;
	o->feature_weight = 0.001f;
	o->latest_pos = -1;
	
    mp_arg_check_num(n_args, n_kw, 2, 2, false);

	o->max_len = mp_obj_get_int(args[0]);
	o->unit_mm = mp_obj_get_int(args[1]);

	if (o->max_len == 0) {
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Zero is not allowd."));
	}

	o->probability = m_new(float,   o->max_len);
	o->feature     = m_new(uint8_t, o->max_len);
	o->feature_count_table = m_new(uint16_t,  256);

	if (o->probability == NULL || o->feature == NULL || o->feature_count_table == NULL) {
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_MemoryError, "Not enough memory."));
	}

	return MP_OBJ_FROM_PTR(o);
}

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t spe_push_map_item(mp_obj_t self_in, mp_obj_t feature)
{
	spe_obj_t *o = (spe_obj_t *)self_in;

    uint8_t f = (uint8_t)mp_obj_get_int(feature);
	if (o->len >= o->max_len) {
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_MemoryError, "Map is over."));
	}

	o->feature[o->len++] = f;

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(spe_push_map_item_obj, spe_push_map_item);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t spe_len(mp_obj_t self_in)
{
	spe_obj_t *o = (spe_obj_t *)self_in;

	return MP_OBJ_NEW_SMALL_INT(o->len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(spe_len_obj, spe_len);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t spe_prepare(mp_obj_t self_in)
{
	spe_obj_t *o = (spe_obj_t *)self_in;

	float init = 1.0f / o->len;

	for (int i=0 ; i< o->len ; ++i) {
		o->probability[i] = init;
	}

	o->milage_mm = 0;

	memset (o->feature_count_table, 0, sizeof(uint16_t) * 256);
	for (int i = 0 ; i<o->len ; ++i) {
		o->feature_count_table[o->feature[i]]++;
	}

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(spe_prepare_obj, spe_prepare);



/*---------------------------------------------------------------------------*/
static float _get_existance(spe_obj_t *spe, int map_index)
{
	int idx = (spe->len + map_index - (int)(spe->milage_mm / spe->unit_mm));
	while (idx < 0) {
		idx += spe->len;
	}

	return spe->probability[idx % spe->len];
}

static void _set_existance(spe_obj_t *spe, int map_index, float prob)
{
	int idx = (spe->len + map_index - (int)(spe->milage_mm / spe->unit_mm));
	while (idx < 0) {
		idx += spe->len;
	}

	spe->probability[idx % spe->len] = prob;

	return;
}

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t spe_estimate(mp_obj_t self_in, mp_obj_t delta_mm, mp_obj_t feature)
{
	spe_obj_t *o = (spe_obj_t *)self_in;

	float _delta_mm = mp_obj_get_float(delta_mm);
	uint8_t f = (uint8_t)mp_obj_get_int(feature);

	o->milage_mm += _delta_mm;

	if(o->feature_count_table[f] == 0) {
		return MP_OBJ_NEW_SMALL_INT(o->latest_pos);
	}

	int i=0;
	float max = 0.0f;
	int max_idx = -1;
	float weighted_existance = o->feature_weight / o->feature_count_table[f];
	for (i=0 ; i<o->len ; ++i) {

		float prob = _get_existance(o, i) * (1.0f - o->feature_weight);
		if (o->feature[i] == f) {
			prob += weighted_existance;
		}

		_set_existance(o, i, prob);

		if(prob > max) {
			max = prob;
			max_idx = i;
		}

	}

	o->latest_pos = max_idx;
	return MP_OBJ_NEW_SMALL_INT(max_idx);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(spe_estimate_obj, spe_estimate);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t spe_get_probability(mp_obj_t self_in, mp_obj_t index)
{
	spe_obj_t *o = (spe_obj_t *)self_in;

	int i = mp_obj_get_int(index);

	if (i > o->len) {
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "Out of range %d.", i));
	}

	if (i < 0) {
		float p=0;
		for (int j=0 ; j<o->len ; ++j) {
			p += o->probability[j];
		}
		return mp_obj_new_float(p);
	}

	return mp_obj_new_float(o->probability[i]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(spe_get_probability_obj, spe_get_probability);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t spe_set_weight(mp_obj_t self_in, mp_obj_t weight)
{
	spe_obj_t *o = (spe_obj_t *)self_in;

	o->feature_weight = mp_obj_get_float(weight);

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(spe_set_weight_obj, spe_set_weight);



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC const mp_rom_map_elem_t machine_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_push_map_item),	MP_ROM_PTR(&spe_push_map_item_obj) },
	{ MP_ROM_QSTR(MP_QSTR_len),				MP_ROM_PTR(&spe_len_obj) },
	{ MP_ROM_QSTR(MP_QSTR_prepare),			MP_ROM_PTR(&spe_prepare_obj) },
	{ MP_ROM_QSTR(MP_QSTR_estimate),		MP_ROM_PTR(&spe_estimate_obj) },
	{ MP_ROM_QSTR(MP_QSTR_set_weight),		MP_ROM_PTR(&spe_set_weight_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_probability),	MP_ROM_PTR(&spe_get_probability_obj) },
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void self_position_estimater_state_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
	spe_obj_t *o = (spe_obj_t *)self_in;

	aiMini4wdDebugPrintf("Self Position Estimater:\n");
	aiMini4wdDebugPrintf("  Max map size = %d\n", o->max_len);
	aiMini4wdDebugPrintf("  Actual map size = %d\n", o->len);
	aiMini4wdDebugPrintf("  Sampling Unit = %d[mm]\n", o->unit_mm);
	aiMini4wdDebugPrintf("  Feature weight = %f\n", o->feature_weight);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC MP_DEFINE_CONST_DICT(machine_locals_dict, machine_locals_dict_table);

const mp_obj_type_t spe_type = {
    { &mp_type_type },
    .name = MP_QSTR_SelfPositionEstimater,
    .print = self_position_estimater_state_print,
    .make_new = self_position_estimater_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_locals_dict,
};
