/*
 * mini4wd.h
 *
 * Created: 2019/03/10
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */ 
#ifndef MINI_4WD_H_
#define MINI_4WD_H_

#include <ai_mini4wd_sensor.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void mini4wd_initialize(void);

int aiMini4wdNextSensorData(AiMini4wdSensorData *storage);
void aiMini4wdWaitInternalTrigger(void);
void aiMini4wdWaitExternalTrigger(void);
int aiMini4wdCheckExternalTrigger(void);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
extern const mp_obj_module_t mini4wd_module;
extern const mp_obj_type_t machine_type;
extern const mp_obj_type_t spe_type;
extern const mp_obj_type_t machine_learning_type;


#endif