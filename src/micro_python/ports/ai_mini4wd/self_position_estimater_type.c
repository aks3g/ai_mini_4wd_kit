/*
 * localization_type.c
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



typedef struct _localization_obj_t {
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
} localization_obj_t;

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t localization_make_new(const mp_obj_type_t *type_in, size_t n_args, 
	size_t n_kw, const mp_obj_t *args)
{
	localization_obj_t *o = m_new_obj(localization_obj_t);
	
	memset(o, 0, sizeof(localization_obj_t));
	o->base.type = &localization_type;
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
STATIC mp_obj_t localization_push_map_item(mp_obj_t self_in, mp_obj_t feature)
{
	localization_obj_t *o = (localization_obj_t *)self_in;

    uint8_t f = (uint8_t)mp_obj_get_int(feature);
	if (o->len >= o->max_len) {
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_MemoryError, "Map is over."));
	}

	o->feature[o->len++] = f;

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(localization_push_map_item_obj, localization_push_map_item);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t localization_len(mp_obj_t self_in)
{
	localization_obj_t *o = (localization_obj_t *)self_in;

	return MP_OBJ_NEW_SMALL_INT(o->len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(localization_len_obj, localization_len);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t localization_prepare(mp_obj_t self_in)
{
	localization_obj_t *o = (localization_obj_t *)self_in;

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
STATIC MP_DEFINE_CONST_FUN_OBJ_1(localization_prepare_obj, localization_prepare);



/*---------------------------------------------------------------------------*/
static float _get_existance(localization_obj_t *localization, int map_index)
{
	int idx = (localization->len + map_index - (int)(localization->milage_mm / localization->unit_mm));
	while (idx < 0) {
		idx += localization->len;
	}

	return localization->probability[idx % localization->len];
}

static void _set_existance(localization_obj_t *localization, int map_index, float prob)
{
	int idx = (localization->len + map_index - (int)(localization->milage_mm / localization->unit_mm));
	while (idx < 0) {
		idx += localization->len;
	}

	localization->probability[idx % localization->len] = prob;

	return;
}

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t localization_estimate(mp_obj_t self_in, mp_obj_t delta_mm, mp_obj_t feature)
{
	localization_obj_t *o = (localization_obj_t *)self_in;

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

	aiMini4wdSetLedIndicator((uint16_t)max_idx, 1);

	o->latest_pos = max_idx;
	return MP_OBJ_NEW_SMALL_INT(max_idx);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(localization_estimate_obj, localization_estimate);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t localization_get_probability(mp_obj_t self_in, mp_obj_t index)
{
	localization_obj_t *o = (localization_obj_t *)self_in;

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
STATIC MP_DEFINE_CONST_FUN_OBJ_2(localization_get_probability_obj, localization_get_probability);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t localization_set_weight(mp_obj_t self_in, mp_obj_t weight)
{
	localization_obj_t *o = (localization_obj_t *)self_in;

	o->feature_weight = mp_obj_get_float(weight);

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(localization_set_weight_obj, localization_set_weight);



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC const mp_rom_map_elem_t machine_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_push_map_item),	MP_ROM_PTR(&localization_push_map_item_obj) },
	{ MP_ROM_QSTR(MP_QSTR_len),				MP_ROM_PTR(&localization_len_obj) },
	{ MP_ROM_QSTR(MP_QSTR_prepare),			MP_ROM_PTR(&localization_prepare_obj) },
	{ MP_ROM_QSTR(MP_QSTR_estimate),		MP_ROM_PTR(&localization_estimate_obj) },
	{ MP_ROM_QSTR(MP_QSTR_set_weight),		MP_ROM_PTR(&localization_set_weight_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_probability),	MP_ROM_PTR(&localization_get_probability_obj) },
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void localization_state_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
	localization_obj_t *o = (localization_obj_t *)self_in;

	aiMini4wdDebugPrintf("Localization:\n");
	aiMini4wdDebugPrintf("  Max map size = %d\n", o->max_len);
	aiMini4wdDebugPrintf("  Actual map size = %d\n", o->len);
	aiMini4wdDebugPrintf("  Sampling Unit = %d[mm]\n", o->unit_mm);
	aiMini4wdDebugPrintf("  Feature weight = %f\n", o->feature_weight);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC MP_DEFINE_CONST_DICT(machine_locals_dict, machine_locals_dict_table);

const mp_obj_type_t localization_type = {
    { &mp_type_type },
    .name = MP_QSTR_Localization,
    .print = localization_state_print,
    .make_new = localization_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_locals_dict,
};
