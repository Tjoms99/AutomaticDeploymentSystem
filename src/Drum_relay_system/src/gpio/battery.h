#ifndef __BATTERY_H__
#define __BATTERY_H__

void battery_fast_charge(void);

void battery_slow_charge(void);

void battery_charge_start(void);

void battery_charge_stop(void);

float battery_get_voltage(void);

float battery_get_percentage(void);

void battery_init(void);

#endif