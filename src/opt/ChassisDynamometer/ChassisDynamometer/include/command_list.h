/*
 * command_list.h
 *
 * Created: 2021/06/30 10:17:41
 *  Author: kiyot
 */ 


#ifndef COMMAND_LIST_H_
#define COMMAND_LIST_H_

#include <console.h>

extern ConsoleCommand g_ctrl_cmd;
extern ConsoleCommand g_state_cmd;
extern ConsoleCommand g_led_cmd;
extern ConsoleCommand g_adcw_cmd;
extern ConsoleCommand g_adcr_cmd;
extern ConsoleCommand g_adccap_cmd;
extern ConsoleCommand g_monitor_cmd;
extern ConsoleCommand g_sensor_cmd;
extern ConsoleCommand g_usb_ctrl_cmd;

#endif /* COMMAND_LIST_H_ */