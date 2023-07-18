/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bluetooth.h"
#include "../rs485/rs485.h"

#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/byteorder.h>

static int start_scan(void);
static uint8_t discover_func(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             struct bt_gatt_discover_params *params);

//-------------------------------------------------------------------------------------------------
// BLUETOOTH GENERAL
#define BT_LE_CONN_PARAM_LONG_TIMEOUT BT_LE_CONN_PARAM(BT_GAP_INIT_CONN_INT_MIN, \
                                                       BT_GAP_INIT_CONN_INT_MAX, \
                                                       0, 1000) // N*10ms

static struct bt_conn *default_conn;
static struct bt_uuid_128 uuid128 = BT_UUID_INIT_128(0);
static struct bt_gatt_discover_params discover_params;

enum SERVICES
{
    SERVICE_DATA,
    SERVICE_CONTROL,
    SERVICES_MAX, // !!SHOULD ALWAYS BE LAST!!

};
typedef enum SERVICES services_t;
static services_t service;

//-------------------------------------------------------------------------------------------------
// DATA SERVICE & CHARACTERISTICS
#define uuid_data_service_val BT_UUID_128_ENCODE(0x561721cd, 0xdbb7, 0x462d, 0xa651, 0x8f542da25b61)
#define uuid_depth_characteristic_val BT_UUID_128_ENCODE(0x0655982c, 0x02f8, 0x49bb, 0x961d, 0x4cb9984399b1)
#define uuid_pressure_characteristic_val BT_UUID_128_ENCODE(0x6e38e4a1, 0xd667, 0x420d, 0x8774, 0x87a8c319a54c)
#define uuid_temperature_characteristic_val BT_UUID_128_ENCODE(0x61393466, 0xb9b0, 0x447f, 0x8272, 0x1d51c378ae40)

#define BT_UUID_DATA BT_UUID_DECLARE_128(uuid_data_service_val)
#define BT_UUID_DATA_DEPTH BT_UUID_DECLARE_128(uuid_depth_characteristic_val)
#define BT_UUID_DATA_PRESSURE BT_UUID_DECLARE_128(uuid_pressure_characteristic_val)
#define BT_UUID_DATA_TEMPERATURE BT_UUID_DECLARE_128(uuid_temperature_characteristic_val)

static data_characteristic_t data_characteristic;
static uint16_t characteristic_value_handlers[DATA_CHARACTERISTICS_MAX];

//-------------------------------------------------------------------------------------------------
// CONTROL SERVICE & CHARACTERISTICS
#define uuid_control_service_val BT_UUID_128_ENCODE(0x97ef73d0, 0x8c29, 0x424f, 0x829a, 0x1a2c78ae506a)
#define uuid_system_on_characteristic_val BT_UUID_128_ENCODE(0xb1cdad8d, 0x31b9, 0x4f80, 0xb0d6, 0x294b1519b524)
#define uuid_sampling_on_characteristic_val BT_UUID_128_ENCODE(0xfdd5a88d, 0xdf71, 0x4b57, 0x93c6, 0x01ed143c3c3d)
#define uuid_depth_init_characteristic_val BT_UUID_128_ENCODE(0x972b390a, 0x7f72, 0x4620, 0xa866, 0xe99ed2b49e65)
#define uuid_rs232_on_characteristic_val BT_UUID_128_ENCODE(0x8d1a814d, 0x2889, 0x4ce2, 0xa228, 0xec44a9053bb8)
#define uuid_12v_on_characteristic_val BT_UUID_128_ENCODE(0x2932a5b9, 0xfaee, 0x4d50, 0Xb6da, 0xb82b5b9739ad)
#define uuid_sampling_time_characteristic_val BT_UUID_128_ENCODE(0xc45c93cb, 0xaec4, 0x4590, 0x8b65, 0x9ee695aa8e40)

#define BT_UUID_CONTROL BT_UUID_DECLARE_128(uuid_control_service_val)
#define BT_UUID_CONTROL_SYSTEM_ON BT_UUID_DECLARE_128(uuid_system_on_characteristic_val)
#define BT_UUID_CONTROL_SAMPLING_ON BT_UUID_DECLARE_128(uuid_sampling_on_characteristic_val)
#define BT_UUID_CONTROL_RS232_ON BT_UUID_DECLARE_128(uuid_rs232_on_characteristic_val)
#define BT_UUID_CONTROL_12V_ON BT_UUID_DECLARE_128(uuid_12v_on_characteristic_val)
#define BT_UUID_CONTROL_SAMPLING_TIME BT_UUID_DECLARE_128(uuid_sampling_time_characteristic_val)

static control_characteristic_t control_characteristic;
static struct bt_gatt_subscribe_params subscribe_params[CONTROL_CHARACTERISTICS_MAX];

bool is_initialized = false;

/**
 * @brief Notify callback function for subscribed characteristics.
 *
 * @param[in] conn Current BLE connection.
 *
 * @param[in] params Current subscribe parameters.
 *
 * @param[in] data Pointer to data in characteristic.
 *
 * @param[in] length Lenght of data pointer in bytes.
 *
 * @retval BT_GATT_ITER_CONTINUE to keep subscribing, BT_GATT_ITER_STOP to stop subscribing.
 */
static uint8_t notify_func(struct bt_conn *conn,
                           struct bt_gatt_subscribe_params *params,
                           const void *data, uint16_t length)
{
    static char *data_s;

    if (!data)
    {
        printk("Unsubscribed\n");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }

    if (!is_initialized)
    {
        return BT_GATT_ITER_CONTINUE;
    }

    data_s = (char *)data;

    // Set end of string value to ignore the rest of the pointer value.
    data_s[length] = '\0';

    if (subscribe_params[CONTROL_SAMPLING_ON].value_handle == params->value_handle)
    {
        printk("FOUND SAMPLING\n");
        rs485_write("a");
    }

    else if (subscribe_params[CONTROL_SAMPLING_TIME].value_handle == params->value_handle)
    {
        printk("FOUND SAMPLING TIME %s\n", data_s);
        rs485_write("b");
        rs485_write(data_s);
        rs485_write("b");
    }

    else if (subscribe_params[CONTROL_DEPTH_INIT].value_handle == params->value_handle)
    {
        printk("FOUND DEPTH INIT\n");
        rs485_write("c");
    }

    else if (subscribe_params[CONTROL_RS232_ON].value_handle == params->value_handle)
    {
        printk("FOUND RS232\n");
        rs485_write("d");
    }

    else if (subscribe_params[CONTROL_12V_ON].value_handle == params->value_handle)
    {
        printk("FOUND 12V\n");
        rs485_write("e");
    }

    else if (subscribe_params[CONTROL_SYSTEM_ON].value_handle == params->value_handle)
    {
        printk("FOUND SYSTEM\n");
        rs485_write("f");
    }

    return BT_GATT_ITER_CONTINUE;
}

/**
 * @brief Tries to discover a services.
 *
 * @param[in] target_service Type of service to look for (mapped in function).
 */
void discover_service(services_t target_service)
{
    if (target_service == SERVICE_DATA)
    {
        memcpy(&uuid128, BT_UUID_DATA, sizeof(uuid128));
    }
    else if (target_service == SERVICE_CONTROL)
    {
        memcpy(&uuid128, BT_UUID_CONTROL, sizeof(uuid128));
    }

    discover_params.uuid = &uuid128.uuid;
    discover_params.func = discover_func;
    discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
    discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
    discover_params.type = BT_GATT_DISCOVER_PRIMARY;

    int ret = bt_gatt_discover(default_conn, &discover_params);
    ret ? printk("Discover failed (error %d)\n", ret) : printk("Discover Service...\n");
}

/**
 * @brief Tries to discover a data characteristic.
 *
 * @param[in] target_characteristic Type of characteristic to look for (mapped in function).
 *
 * @param[in] attr GATT attribute, used to get the current handle (basically an address to the characteristic).
 */
void discover_data_characteristic(data_characteristic_t target_characteristic,
                                  const struct bt_gatt_attr *attr)
{
    switch (target_characteristic)
    {
    case DATA_DEPTH:
        memcpy(&uuid128, BT_UUID_DATA_DEPTH, sizeof(uuid128));
        break;
    case DATA_PRESSURE:
        memcpy(&uuid128, BT_UUID_DATA_PRESSURE, sizeof(uuid128));
        break;
    case DATA_TEMPERATURE:
        memcpy(&uuid128, BT_UUID_DATA_TEMPERATURE, sizeof(uuid128));
        break;
    default:
        // Do nothing
        break;
    }

    discover_params.uuid = &uuid128.uuid;
    discover_params.start_handle = attr->handle + 1;
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

    int ret = bt_gatt_discover(default_conn, &discover_params);
    ret ? printk("Discover failed (error %d)\n", ret) : printk("Discover Chacateristic...\n");
}

/**
 * @brief Tries to discover a control characteristic.
 *
 * @param[in] target_characteristic Type of characteristic to look for (mapped in function).
 *
 * @param[in] attr GATT attribute, used to get the current handle (basically an address to the characteristic).
 */
void discover_control_characteristic(control_characteristic_t target_characteristic,
                                     const struct bt_gatt_attr *attr)
{
    switch (target_characteristic)
    {
    case CONTROL_SYSTEM_ON:
        memcpy(&uuid128, BT_UUID_CONTROL_SYSTEM_ON, sizeof(uuid128));
        break;
    case CONTROL_SAMPLING_ON:
        memcpy(&uuid128, BT_UUID_CONTROL_SAMPLING_ON, sizeof(uuid128));
        break;
    case CONTROL_DEPTH_INIT:
        memcpy(&uuid128, BT_UUID_CONTROL_SAMPLING_ON, sizeof(uuid128));
        break;
    case CONTROL_RS232_ON:
        memcpy(&uuid128, BT_UUID_CONTROL_RS232_ON, sizeof(uuid128));
        break;
    case CONTROL_12V_ON:
        memcpy(&uuid128, BT_UUID_CONTROL_12V_ON, sizeof(uuid128));
        break;
    case CONTROL_SAMPLING_TIME:
        memcpy(&uuid128, BT_UUID_CONTROL_SAMPLING_TIME, sizeof(uuid128));
        break;
    default:
        // Do nothing
        break;
    }

    discover_params.uuid = &uuid128.uuid;
    discover_params.start_handle = attr->handle + 1;
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

    int ret = bt_gatt_discover(default_conn, &discover_params);
    ret ? printk("Discover failed (error %d)\n", ret) : printk("Discover Chacateristic...\n");
}

/**
 * @brief Tries to subscribe to a characteristic.
 * Sets the flag to get automatically notified when a value changes in the characteristic.
 *
 * @param[in] attr GATT attribute, used to get the current handle (basically an address to the characteristic).
 */
void subscribe_to_control_characteristic(const struct bt_gatt_attr *attr)
{
    subscribe_params[control_characteristic].notify = notify_func;
    subscribe_params[control_characteristic].value = BT_GATT_CCC_NOTIFY;
    subscribe_params[control_characteristic].ccc_handle = attr->handle;

    int ret = bt_gatt_subscribe(default_conn, &subscribe_params[control_characteristic]);
    (ret && ret != -EALREADY) ? printk("Subscribe failed (error %d)\n", ret) : printk("[SUBSCRIBED] to handle %x \n", attr->handle - 1);
}

/**
 * @brief Tries to discover a discription.
 *
 * @param[in] attr GATT attribute, used to get the current handle (basically an address to the discription).
 */
void discover_description(const struct bt_gatt_attr *attr)
{

    memcpy(&uuid128, BT_UUID_GATT_CCC, sizeof(uuid128));
    discover_params.uuid = &uuid128.uuid;
    discover_params.start_handle = attr->handle + 2;
    discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    subscribe_params[control_characteristic].value_handle = bt_gatt_attr_value_handle(attr);

    int ret = bt_gatt_discover(default_conn, &discover_params);
    ret ? printk("Discover failed (error %d)\n", ret) : printk("Discover Description...\n");
}

/**
 * @brief Discover function used by bt_gatt_discover.
 *
 * @param[in] conn Current BLE connection.
 *
 * @param[in] attr GATT attribute that contains information about what is being discovered.
 *
 * @param[in] params Current GATT attribute parameters.
 *
 * @note Recursive function that continously looks for next GATT attribute.
 *  Needs to be modified if more services are added.
 */
static uint8_t discover_func(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             struct bt_gatt_discover_params *params)
{
    if (!attr)
    {
        printk("Discover complete\n");
        (void)memset(params, 0, sizeof(*params));
        is_initialized = true;
        return BT_GATT_ITER_STOP;
    }

    switch (params->type)
    {
    case BT_GATT_DISCOVER_SECONDARY:
    case BT_GATT_DISCOVER_PRIMARY:
        if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_DATA))
        {
            printk("[SERVICE] data\n");
            discover_data_characteristic(data_characteristic, attr);
        }
        else if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_CONTROL))
        {
            printk("[SERVICE] control\n");
            discover_control_characteristic(control_characteristic, attr);
        }
        break;
    case BT_GATT_DISCOVER_CHARACTERISTIC:
        printk("[CHARACTERISTIC] handle %x \n", attr->handle + 1);

        switch (service)
        {
        case SERVICE_DATA:
            characteristic_value_handlers[data_characteristic] = bt_gatt_attr_value_handle(attr);

            data_characteristic++;
            if (data_characteristic == DATA_CHARACTERISTICS_MAX)
            {
                service = SERVICE_CONTROL;
                discover_service(service);
                return BT_GATT_ITER_STOP;
            }

            discover_data_characteristic(data_characteristic, attr);
            break;

        case SERVICE_CONTROL:
            if (control_characteristic == CONTROL_CHARACTERISTICS_MAX)
            {
                // Remove discovery parameters (ENDS DISCOVERY)
                memset(&discover_params, 0, sizeof(discover_params));
                return BT_GATT_ITER_STOP;
            }
            discover_description(attr);

            break;
        default:
            // Do nothing
            break;
        }

        break;
    case BT_GATT_DISCOVER_DESCRIPTOR:
        subscribe_to_control_characteristic(attr);

        control_characteristic++;

        discover_control_characteristic(control_characteristic, attr);

        break;
    }

    return BT_GATT_ITER_STOP;
}

/**
 * @brief Callback function for bt_le_scan_start. Triggered when a device is found.
 *
 * @param[in] addr Bluetooth LE Device Address.
 *
 * @param[in] rssi Strength of advertiser signal.
 *
 * @param[in] type Advertising PDU (Protocol Data Unit) types.
 *
 * @param[in] ad Buffer containing advertiser data.
 */
static void device_found(const bt_addr_le_t *addr,
                         int8_t rssi,
                         uint8_t type,
                         struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    int ret;

    if (default_conn)
    {
        return;
    }

    // Only interested in connectable events
    if (type != BT_GAP_ADV_TYPE_ADV_IND &&
        type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND)
    {
        return;
    }

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    printk("Device found: %s (RSSI %d)\n", addr_str, rssi);

    // Connect to devices in close proximity
    if (rssi < -60)
    {
        return;
    }

    if (bt_le_scan_stop())
    {
        return;
    }

    ret = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
                            BT_LE_CONN_PARAM_LONG_TIMEOUT, &default_conn);
    if (ret)
    {
        printk("Create conn to %s failed (%d)\n", addr_str, ret);
        (void)start_scan();
    }
}

/**
 * @brief Scans for BLE devices that are advertising.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
static int start_scan(void)
{
    int ret = 0;

    ret = bt_le_scan_start(BT_LE_SCAN_PASSIVE, device_found);
    if (ret)

    {
        printk("Scanning failed to start (err %d)\n", ret);
        return ret;
    }

    printk("Scanning successfully started\n");
    return ret;
}

/**
 * @brief Callback function for when a device has connected.
 *  Starts to discover services.
 *
 * @param[in] conn Current BLE connection.
 *
 * @param[in] err Connection error
 */
static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err)
    {
        printk("Failed to connect to %s (%u)\n", addr, err);

        bt_conn_unref(default_conn);
        default_conn = NULL;

        (void)start_scan();
        return;
    }

    if (conn != default_conn)
    {
        return;
    }

    printk("Connected: %s\n", addr);

    // Prepare to discover service
    service = SERVICE_DATA;
    data_characteristic = DATA_DEPTH;
    control_characteristic = CONTROL_SYSTEM_ON;

    discover_service(service);
}

/**
 * @brief Callback function for when a device has disconnected.
 *  Starts to scan for BLE devices again.
 *
 * @param[in] conn Current BLE connection.
 *
 * @param[in] reason Disconnection reason.
 */
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (conn != default_conn)
    {
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printk("Disconnected: %s (reason 0x%02x)\n", addr, reason);

    bt_conn_unref(default_conn);
    default_conn = NULL;
    is_initialized = false;

    // Restart scanning.
    (void)start_scan();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

int bluetooth_write_data(data_characteristic_t charecteristic, uint8_t *data, uint8_t length)
{
    int ret = 0;
    if (!is_initialized)
    {
        printk("Bluetooth not initialized");
        return -ECANCELED;
    }

    ret |= bt_gatt_write_without_response(default_conn, characteristic_value_handlers[charecteristic], data, length, false);

    return ret;
}

int bluetooth_init(void)
{
    int ret = 0;

    ret |= bt_enable(NULL);
    if (ret)
    {
        printk("Bluetooth enable failed (err %d)\n", ret);
        return ret;
    }

    ret |= start_scan();

    return ret;
}