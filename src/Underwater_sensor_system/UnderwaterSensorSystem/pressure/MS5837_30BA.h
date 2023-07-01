/*
 * pressure.h
 *
 *  Created on: 7 Dec 2022
 *      Author: marcu
 */

#ifndef MS5837_30BA_H_
#define MS5837_30BA_H_
#include <stdint.h>

void ms5847_30ba_init(void);
void ms5847_30ba_measure(float *pressure, float *temperature);
void ms5847_30ba_get_depth(float *depth, float pressure, float pressure_reference);

#endif /* MS5837_30BA_H_ */
