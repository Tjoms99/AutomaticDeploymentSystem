/*
 * sensors.h
 *
 *  Created on: 1 Jul 2023
 *      Author: marcu
 */

#ifndef SENSORS_SENSORS_H_
#define SENSORS_SENSORS_H_

void sensors_init();
void sensors_get_values(float *temperature, float *pressure);
void sensors_get_depth(float *depth);

#endif /* SENSORS_SENSORS_H_ */
