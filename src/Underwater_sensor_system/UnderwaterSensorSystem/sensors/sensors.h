/*
 * sensors.h
 *
 *  Created on: 1 Jul 2023
 *      Author: marcu
 */

#ifndef SENSORS_SENSORS_H_
#define SENSORS_SENSORS_H_

void sensors_init();
void sensors_get_values(float *temperature, float *pressure, float *depth);
void sensors_get_depth(float *depth);
void sensors_pressure_at_zero_depth(float pressure);

#endif /* SENSORS_SENSORS_H_ */
