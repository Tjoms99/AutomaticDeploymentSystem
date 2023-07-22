#ifndef __BATTERY_H__
#define __BATTERY_H__
#include <stdint.h>

/**
 * @brief Sets the time interval for how often the battery percentage should be written to the BLE Battery characteristic.
 *
 * @param[in] time_ms Publishing interval.
 */
void battery_set_publish_interval(uint32_t time_ms);

/**
 * @brief Set battery charging to fast charge (100mA).
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int battery_set_fast_charge(void);

/**
 * @brief Set battery charging to slow charge (50mA).
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int battery_set_slow_charge(void);

/**
 * @brief Start battery charging.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int battery_charge_start(void);

/**
 * @brief Stop battery charging.
 *
 * @retval 0 if successful. Negative errno number on error.
 *
 * @note: want to stop charging to save power during runtime (Disables LED).
 */
int battery_charge_stop(void);

/**
 * @brief Calculates the battery voltage using the ADC.
 *
 * @param[in] battery_volt Pointer where battery voltage is stored.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int battery_get_voltage(float *battery_volt);

/**
 * @brief Calculates the battery percentage using the battery voltage.
 *
 * @param[in] battery_percentage  Pointer where battery percentage is stored.
 *
 * @param[in] battery_volt Voltage used to calculate the percentage of how much energy is left in a 3.7V LiPo battery.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int battery_get_percentage(int *battery_percentage, float battery_volt);

/**
 * @brief Initialize the battery charging circuit.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int battery_init(void);

#endif