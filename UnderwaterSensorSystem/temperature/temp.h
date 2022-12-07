/*
 * temp.h
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#ifndef TEMP_H_
#define TEMP_H_

volatile static uint32_t k4 = 28446;
volatile static uint32_t k3 = 24926;
volatile static uint32_t k2 = 36016;
volatile static uint32_t k1 = 32791;
volatile static uint32_t k0 = 40781;

void init_temp();
double get_temperature();




#endif /* TEMP_H_ */
