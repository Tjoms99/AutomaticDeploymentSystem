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

void ms5847_30ba_start_pressure_convertion(void);

void ms5847_30ba_start_temperature_convertion(void);

void ms5847_30ba_get_convertion(uint32_t *adc);

void ms5847_30ba_calculate(float *pressure, float *temperature, uint32_t adc_value_pressure, uint32_t adc_value_temperature);

void ms5847_30ba_measure(float *pressure, float *temperature);

void ms5847_30ba_get_depth(float *depth, float pressure, float pressure_reference);

#endif /* MS5837_30BA_H_ */
