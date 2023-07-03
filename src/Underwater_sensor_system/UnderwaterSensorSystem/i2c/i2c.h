/*
 * i2c.h
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#ifndef I2C_H_
#define I2C_H_

volatile uint64_t i2c_data_in;
volatile uint32_t i2c_data_out;

void i2c_init();
void i2c_write(uint8_t cmd, uint8_t address);
void i2c_read(uint8_t bytes_to_read, uint8_t address);

#endif /* I2C_H_ */
