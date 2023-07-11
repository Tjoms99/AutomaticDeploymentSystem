/*
 * sensors.h
 *
 *  Created on: 1 Jul 2023
 *      Author: marcu
 */

#ifndef SENSORS_SENSORS_H_
#define SENSORS_SENSORS_H_

void sensors_sample(void);

void sensors_sample_and_print(void);

void sensors_set_depth_zero(void);

void sensors_init(void);

#endif /* SENSORS_SENSORS_H_ */
