#ifndef __BLE_H__
#define __BLE_H__
#include <stdint.h>

enum DATA_CHARACTERISTIC
{
    DATA_DEPTH,
    DATA_PRESSURE,
    DATA_TEMPERATURE,
    DATA_CHARACTERISTICS_MAX, // !!SHOULD ALWAYS BE LAST!!

};
typedef enum DATA_CHARACTERISTIC data_characteristic_t;

enum CONTROL_CHARACTERISTIC
{
    CONTROL_SYSTEM_ON,
    CONTROL_SAMPLING_ON,
    CONTROL_DEPTH_INIT,
    CONTROL_RS232_ON,
    CONTROL_12V_ON,
    CONTROL_SAMPLING_TIME,
    CONTROL_CHARACTERISTICS_MAX, // !!SHOULD ALWAYS BE LAST!!

};
typedef enum CONTROL_CHARACTERISTIC control_characteristic_t;

void bluetooth_write_data(data_characteristic_t charecteristic, uint8_t *data, uint8_t length);
void bluetooth_init(void);

#endif