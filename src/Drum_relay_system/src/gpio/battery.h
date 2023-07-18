#ifndef __BATTERY_H__
#define __BATTERY_H__

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
 * @retval Battery voltage.
 */
float battery_get_voltage(void);

/**
 * @brief Calculates the battery percentage using the battery voltage.
 *
 * @retval Battery percentage.
 */
float battery_get_percentage(void);

/**
 * @brief Initialize the battery charging circuit.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int battery_init(void);

#endif