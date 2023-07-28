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
    CONTROL_PING,
    CONTROL_CHARACTERISTICS_MAX, // !!SHOULD ALWAYS BE LAST!!

};
typedef enum CONTROL_CHARACTERISTIC control_characteristic_t;

/**
 * @brief Retrieves the bluetooth ping flag which is updated at a certain interval.
 *
 * @param[in] flag Pointer to ping flag data. 1 if recieved, 0 if not recieved.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int bluetooth_get_ping(uint8_t *flag);

/**
 * @brief Scans for BLE devices that are advertising.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int bluetooth_start_scan(void);

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
int bluetooth_write_data(data_characteristic_t characteristic, char *data, uint8_t length);

/**
 * @brief Initialize the BLE stack.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int bluetooth_init(void);

/**
 * @brief Reinitialize the BLE stack.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int bluetooth_reinit(void);

#endif