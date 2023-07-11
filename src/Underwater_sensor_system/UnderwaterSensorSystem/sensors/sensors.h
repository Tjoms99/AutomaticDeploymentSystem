/*
 * sensors.h
 *
 *  Created on: 1 Jul 2023
 *      Author: marcu
 */

#ifndef SENSORS_SENSORS_H_
#define SENSORS_SENSORS_H_

void sensors_pressure_at_zero_depth(float pressure);

void sensors_sample();

void sensors_sample_print();

void sensors_init();

#endif /* SENSORS_SENSORS_H_ */
