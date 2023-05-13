/*
 * uart.h
 *
 *  Created on: 31 Jan 2023
 *      Author: marcus
 */

#ifndef __ICL3221_H_
#define __ICL3221_H_
#include <stdint.h>

    void icl3221_init(void);
    void icl3221_power(uint8_t on);
    void icl3221_set_mode(uint8_t mode);
    void icl3221_transmit(char data);
    void icl3221_recieve(char *data);



#endif /* __ICL3221_H_ */
