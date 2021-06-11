/*
 * mini4wd_type.h
 *
 * Created: 2019/03/10
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#include <stdio.h>
#include <stdlib.h>

#include "py/runtime.h"
#include "py/mphal.h"

#include "mini4wd.h"

#include <ai_mini4wd.h>
#include <ai_mini4wd_hid.h>
#include <ai_mini4wd_motor_driver.h>
#include <ai_mini4wd_sensor.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static int sSensorCount;
static AiMini4wdSensorData sSensorData;
static float sBatteryVoltage = 0;
static float sMotorCurrent = 0;
static float sTireSize = 31.0f;
static float sKp = 0.3f;
static float sKi = 0.01f;
static float sKd = 0.01f;
static int sCurrentDuty = 0;

static float sKpArr[26] = {0.3f};
static float sKiArr[26] = {0.01f};
static float sKdArr[26] = {0.01f};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef struct _machine_obj_t {
    mp_obj_base_t base;
} machine_obj_t;

STATIC const machine_obj_t machine_obj = {{&machine_type}};


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t machine_make_new(const mp_obj_type_t *type_in, size_t n_args, 
	size_t n_kw, const mp_obj_t *args)
{
	aiMini4wdMotorDriverSetPidGain(sKp, sKi, sKd);

	return (mp_obj_t)&machine_obj;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_grab_sensor(mp_obj_t self_in)
{
	sSensorCount = aiMini4wdNextSensorData(&sSensorData);
	(void)aiMini4wdGetBatteryVoltage(&sBatteryVoltage);
	(void)aiMini4wdMotorDriverGetDriveCurrent(&sMotorCurrent);
	sCurrentDuty = aiMini4wdMotorDriverGetDuty();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_grab_sensor_obj, mini4wd_grab_sensor);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getCount(mp_obj_t self_in)
{
    return mp_obj_new_int(sSensorCount);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getCount_obj, mini4wd_getCount);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getAx(mp_obj_t self_in)
{
    return mp_obj_new_float(sSensorData.imu.accel_f[1]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getAx_obj, mini4wd_getAx);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getAy(mp_obj_t self_in)
{
    return mp_obj_new_float(sSensorData.imu.accel_f[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getAy_obj, mini4wd_getAy);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getAz(mp_obj_t self_in)
{
    return mp_obj_new_float(sSensorData.imu.accel_f[2]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getAz_obj, mini4wd_getAz);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getRoll(mp_obj_t self_in)
{
    return mp_obj_new_float(sSensorData.imu.gyro_f[1]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getRoll_obj, mini4wd_getRoll);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getPitch(mp_obj_t self_in)
{
    return mp_obj_new_float(sSensorData.imu.gyro_f[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getPitch_obj, mini4wd_getPitch);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getYaw(mp_obj_t self_in)
{
    return mp_obj_new_float(sSensorData.imu.gyro_f[2]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getYaw_obj, mini4wd_getYaw);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getRpm(mp_obj_t self_in)
{
    return mp_obj_new_float(aiMini4wdSensorGetCurrentRpm());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getRpm_obj, mini4wd_getRpm);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getSpeed(mp_obj_t self_in)
{
    return mp_obj_new_float(aiMini4wdSensorGetSpeed());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getSpeed_obj, mini4wd_getSpeed);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getVbat(mp_obj_t self_in)
{
    return mp_obj_new_float(sBatteryVoltage);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getVbat_obj, mini4wd_getVbat);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_getMotorCurrent(mp_obj_t self_in)
{
    return mp_obj_new_float(sMotorCurrent);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_getMotorCurrent_obj, mini4wd_getMotorCurrent);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_waitIntTrig(mp_obj_t self_in)
{
	aiMini4wdWaitInternalTrigger();

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_waitIntTrig_obj, mini4wd_waitIntTrig);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_waitExtTrig(mp_obj_t self_in)
{
	aiMini4wdWaitExternalTrigger();
	
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_waitExtTrig_obj, mini4wd_waitExtTrig);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_checkExtTrig(mp_obj_t self_in)
{
	int ret = aiMini4wdCheckExternalTrigger();
	
    return mp_obj_new_bool(ret != 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_checkExtTrig_obj, mini4wd_checkExtTrig);



/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_led(mp_obj_t self_in, mp_obj_t led_pattern)
{
	int led_pattern_ = mp_obj_get_int(led_pattern);
	aiMini4wdPrintLedPattern(led_pattern_);

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_led_obj, mini4wd_led);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_switch(mp_obj_t self_in, mp_obj_t index)
{
	int index_ = mp_obj_get_int(index);

    int ret = 0;
    if (index_ == 0) {
		ret = aiMini4wdGetSwitchStatus(cAiMini4wdSwitch0);
	}
	else if (index_ == 1){
		ret = aiMini4wdGetSwitchStatus(cAiMini4wdSwitch1);
	}

    return mp_obj_new_bool(ret == 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_switch_obj, mini4wd_switch);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_setDuty(mp_obj_t self_in, mp_obj_t duty)
{
	int duty_i = mp_obj_get_int(duty);
	if (duty_i > 255) {
		duty_i =  255;
	}
	else if (duty_i < -255) {
		duty_i = -255;
	}

	aiMini4wdMotorDriverDrive(duty_i);

    return mp_const_none;;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_setDuty_obj, mini4wd_setDuty);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_setRpm(mp_obj_t self_in, mp_obj_t rpm)
{
	int rpm_i = mp_obj_get_int(rpm);
	if (rpm_i < 0) {
		rpm_i = 0;
	}

	aiMini4wdMotorDriverSetRpm(rpm_i);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_setRpm_obj, mini4wd_setRpm);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_setSpeed(mp_obj_t self_in, mp_obj_t speed_kmph)
{
	float speed_kmph_f = mp_obj_get_float(speed_kmph);

	float rpm = (speed_kmph_f * 1000000.0) / (sTireSize * 3.14159f * 60);
	int rpm_i = (int)rpm;

	int idx = abs((int)(speed_kmph_f+0.5f));
	idx = idx < (sizeof(sKpArr) / sizeof(sKpArr[0])) ? idx : (sizeof(sKpArr) / sizeof(sKpArr[0])) - 1;

	aiMini4wdMotorDriverSetPidGain(sKpArr[idx], sKiArr[idx], sKdArr[idx]);
	aiMini4wdMotorDriverSetRpm(rpm_i);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_setSpeed_obj, mini4wd_setSpeed);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_tireSize(mp_obj_t self_in, mp_obj_t size_mm)
{
	float tire_mm_ = mp_obj_get_float(size_mm);

	sTireSize = tire_mm_;

	aiMini4wdSensorSetTireSize(tire_mm_);

   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_tireSize_obj, mini4wd_tireSize);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_setGainKp(mp_obj_t self_in, mp_obj_t Kp)
{
	sKp = mp_obj_get_float(Kp);

	aiMini4wdMotorDriverSetPidGain(sKp, sKi, sKd);

	for (int i=0 ; i < (sizeof(sKpArr) / sizeof(sKpArr[0])) ; ++i) {
		sKpArr[i] = sKp;
	}

   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_setGainKp_obj, mini4wd_setGainKp);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_setGainKi(mp_obj_t self_in, mp_obj_t Ki)
{
	sKi = mp_obj_get_float(Ki);

	aiMini4wdMotorDriverSetPidGain(sKp, sKi, sKd);

	for (int i=0 ; i < (sizeof(sKiArr) / sizeof(sKiArr[0])) ; ++i) {
		sKiArr[i] = sKi;
	}

   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_setGainKi_obj, mini4wd_setGainKi);

/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_setGainKd(mp_obj_t self_in, mp_obj_t Kd)
{
	sKd = mp_obj_get_float(Kd);

	aiMini4wdMotorDriverSetPidGain(sKp, sKi, sKd);

	for (int i=0 ; i < (sizeof(sKdArr) / sizeof(sKdArr[0])) ; ++i) {
		sKdArr[i] = sKd;
	}

   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_setGainKd_obj, mini4wd_setGainKd);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_setGain(size_t n, const mp_obj_t *args)
{
//	mp_arg_check_num(n_args, n_kw, 3, 3, false);

	float Kp = mp_obj_get_float(args[1]);
	float Ki = mp_obj_get_float(args[2]);
	float Kd = mp_obj_get_float(args[3]);
	int idx = mp_obj_get_int(args[4]);

	if (0 <= idx && idx < (sizeof(sKdArr) / sizeof(sKdArr[0]))) {
		sKpArr[idx] = Kp;
		sKiArr[idx] = Ki;
		sKiArr[idx] = Kd;
	}

	aiMini4wdMotorDriverSetPidGain(Kp, Ki, Kd);

   return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mini4wd_setGain_obj, 5, mini4wd_setGain);



/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_calibrateTachometer(mp_obj_t self_in)
{
	uint16_t threshold_mv = 0;
	uint16_t *work_buf = m_new(uint16_t,  1024);

	int ret = aiMini4wdSensorCalibrateTachoMeter(&threshold_mv, work_buf, 1024);
	if (ret != 0) {
	   return mp_const_none;
	}

	return mp_obj_new_int((int)threshold_mv);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mini4wd_calibrateTachometer_obj, mini4wd_calibrateTachometer);


/*---------------------------------------------------------------------------*/
STATIC mp_obj_t mini4wd_setTachometerThreshold(mp_obj_t self_in, mp_obj_t threshold)
{
	uint16_t threshold_mv = mp_obj_get_int(threshold);
	aiMini4wdSensorSetTachometerThreshold(threshold_mv, 1); // Flashに毎回保存する

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mini4wd_setTachometerThreshold_obj, mini4wd_setTachometerThreshold);


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC const mp_rom_map_elem_t machine_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_led),				MP_ROM_PTR(&mini4wd_led_obj) },
	{ MP_ROM_QSTR(MP_QSTR_sw),				MP_ROM_PTR(&mini4wd_switch_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setDuty),			MP_ROM_PTR(&mini4wd_setDuty_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setRpm),			MP_ROM_PTR(&mini4wd_setRpm_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setSpeed),		MP_ROM_PTR(&mini4wd_setSpeed_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setTireSize),		MP_ROM_PTR(&mini4wd_tireSize_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setGainKp),		MP_ROM_PTR(&mini4wd_setGainKp_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setGainKi),		MP_ROM_PTR(&mini4wd_setGainKi_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setGainKd),		MP_ROM_PTR(&mini4wd_setGainKd_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setGain),			MP_ROM_PTR(&mini4wd_setGain_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setTachometerThreshold),		MP_ROM_PTR(&mini4wd_setTachometerThreshold_obj) },

	{ MP_ROM_QSTR(MP_QSTR_grab),			MP_ROM_PTR(&mini4wd_grab_sensor_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getCount),		MP_ROM_PTR(&mini4wd_getCount_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getAx),			MP_ROM_PTR(&mini4wd_getAx_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getAy),			MP_ROM_PTR(&mini4wd_getAy_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getAz),			MP_ROM_PTR(&mini4wd_getAz_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getRoll),			MP_ROM_PTR(&mini4wd_getRoll_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getPitch),		MP_ROM_PTR(&mini4wd_getPitch_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getYaw),			MP_ROM_PTR(&mini4wd_getYaw_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getSpeed),		MP_ROM_PTR(&mini4wd_getSpeed_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getRpm),			MP_ROM_PTR(&mini4wd_getRpm_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getVbat),			MP_ROM_PTR(&mini4wd_getVbat_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getMotorCurrent),	MP_ROM_PTR(&mini4wd_getMotorCurrent_obj) },

	{ MP_ROM_QSTR(MP_QSTR_waitIntTrig),		MP_ROM_PTR(&mini4wd_waitIntTrig_obj) },
	{ MP_ROM_QSTR(MP_QSTR_waitExtTrig),		MP_ROM_PTR(&mini4wd_waitExtTrig_obj) },
	{ MP_ROM_QSTR(MP_QSTR_checkExtTrig),	MP_ROM_PTR(&mini4wd_checkExtTrig_obj) },
	{ MP_ROM_QSTR(MP_QSTR_calibrateTachometer),	MP_ROM_PTR(&mini4wd_calibrateTachometer_obj) },
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void machine_state_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
	if (aiMini4wdOdometerEnabled()) {
		mp_printf(print, "%d\t%.3f\t%.3f\t%.3f\t%.0f\t%.0f\t%.0f\t%.3f\t%.3f\t%.3f\t%d\t%.3f\t%.3f",
			sSensorCount,
			sSensorData.imu.accel_f[0],
			sSensorData.imu.accel_f[1],
			sSensorData.imu.accel_f[2],
			sSensorData.imu.gyro_f[0],
			sSensorData.imu.gyro_f[1],
			sSensorData.imu.gyro_f[2],
			sSensorData.rpm,
			
			sBatteryVoltage,
			sMotorCurrent,
			sCurrentDuty,
			
			sSensorData.odometry.delta_x_mm,
			sSensorData.odometry.delta_y_mm
			);
	}
	else {
		mp_printf(print, "%d\t%.3f\t%.3f\t%.3f\t%.0f\t%.0f\t%.0f\t%.3f\t%.3f\t%.3f\t%d",
			sSensorCount,
			sSensorData.imu.accel_f[0],
			sSensorData.imu.accel_f[1],
			sSensorData.imu.accel_f[2],
			sSensorData.imu.gyro_f[0],
			sSensorData.imu.gyro_f[1],
			sSensorData.imu.gyro_f[2],
			sSensorData.rpm,
			
			sBatteryVoltage,
			sMotorCurrent,
			sCurrentDuty);
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
STATIC MP_DEFINE_CONST_DICT(machine_locals_dict, machine_locals_dict_table);

const mp_obj_type_t machine_type = {
    { &mp_type_type },
    .name = MP_QSTR_Machine,
    .print = machine_state_print,
    .make_new = machine_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_locals_dict,
};
