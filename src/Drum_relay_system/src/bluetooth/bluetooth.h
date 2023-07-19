#ifndef __BLE_H__
#define __BLE_H__
#include <stdint.h>

/**
 * @brief Characteristics defined in the Data service.
 *
 * @note Expected to write data to these characteristics.
 */
enum DATA_CHARACTERISTIC
{
    DATA_DEPTH,
    DATA_PRESSURE,
    DATA_TEMPERATURE,
    DATA_BATTERY,
    DATA_CHARACTERISTICS_MAX, // !!SHOULD ALWAYS BE LAST!!

};
typedef enum DATA_CHARACTERISTIC data_characteristic_t;

/**
 * @brief Characteristics defined in the Control service.
 *
 * @note Expected to get notified when data is being written to these characteristics by the server.
 * Should be handeled in the notify callback function.
 */
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

/**
 * @brief Writes to a bluetooth characteristic.
 *
 * @param[in] characteristic Type of data characteristic.
 *
 * @param[in] data Pointer to data/message to be transmitted.
 *
 * @param[in] length Lenght of data pointer in bytes.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int bluetooth_write_data(data_characteristic_t characteristic, uint8_t *data, uint8_t length);

/**
 * @brief Initialize RS485.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int bluetooth_init(void);

#endif