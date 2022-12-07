/*
 * i2c.h
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#ifndef I2C_H_
#define I2C_H_

    volatile uint64_t data_in;
    volatile uint32_t data_out;

    void init_i2c();
    void i2c_write(uint8_t cmd);
    void i2c_read(uint8_t bytes_to_read);

#endif /* I2C_H_ */
