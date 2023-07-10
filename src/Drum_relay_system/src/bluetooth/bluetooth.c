/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bluetooth.h"
#include "../rs485/rs485.h"
#include "../rs485/uart.h"

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

static void start_scan(void);
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
#define BT_UUID_DEPTH BT_UUID_DECLARE_128(uuid_depth_characteristic_val)
#define BT_UUID_PRESSURE BT_UUID_DECLARE_128(uuid_pressure_characteristic_val)
#define BT_UUID_TEMPERATURE BT_UUID_DECLARE_128(uuid_temperature_characteristic_val)

static data_characteristic_t data_characteristic;
static uint16_t characteristic_value_handlers[DATA_CHARACTERISTICS_MAX];

//-------------------------------------------------------------------------------------------------
// CONTROL SERVICE & CHARACTERISTICS
#define uuid_control_service_val BT_UUID_128_ENCODE(0x97ef73d0, 0x8c29, 0x424f, 0x829a, 0x1a2c78ae506a)
#define uuid_system_on_characteristic_val BT_UUID_128_ENCODE(0xb1cdad8d, 0x31b9, 0x4f80, 0xb0d6, 0x294b1519b524)
#define uuid_sampling_on_characteristic_val BT_UUID_128_ENCODE(0xfdd5a88d, 0xdf71, 0x4b57, 0x93c6, 0x01ed143c3c3d)
#define uuid_rs232_on_characteristic_val BT_UUID_128_ENCODE(0x8d1a814d, 0x2889, 0x4ce2, 0xa228, 0xec44a9053bb8)
#define uuid_12v_on_characteristic_val BT_UUID_128_ENCODE(0x2932a5b9, 0xfaee, 0x4d50, 0Xb6da, 0xb82b5b9739ad)
#define uuid_sampling_time_characteristic_val BT_UUID_128_ENCODE(0xc45c93cb, 0xaec4, 0x4590, 0x8b65, 0x9ee695aa8e40)

#define BT_UUID_CONTROL BT_UUID_DECLARE_128(uuid_control_service_val)
#define BT_UUID_SYSTEM_ON BT_UUID_DECLARE_128(uuid_system_on_characteristic_val)
#define BT_UUID_SAMPLING_ON BT_UUID_DECLARE_128(uuid_sampling_on_characteristic_val)
#define BT_UUID_RS232_ON BT_UUID_DECLARE_128(uuid_rs232_on_characteristic_val)
#define BT_UUID_12V_ON BT_UUID_DECLARE_128(uuid_12v_on_characteristic_val)
#define BT_UUID_SAMPLING_TIME BT_UUID_DECLARE_128(uuid_sampling_time_characteristic_val)

static control_characteristic_t control_characteristic;
static struct bt_gatt_subscribe_params subscribe_params[CONTROL_CHARACTERISTICS_MAX];

bool is_initialized = false;
//-------------------------------------------------------------------------------------------------
typedef uint8_t (*notify_function_callback_t)(struct bt_conn *conn,
                                              struct bt_gatt_subscribe_params *params,
                                              const void *data,
                                              uint16_t length);
static notify_function_callback_t notify_callbacks[CONTROL_CHARACTERISTICS_MAX];

//-------------------------------------------------------------------------------------------------
// FUNCTIONS
void bluetooth_write_data(data_characteristic_t charecteristic, uint8_t *data, uint8_t length)
{
    if (!is_initialized)
    {
        return;
    }
    bt_gatt_write_without_response(default_conn, characteristic_value_handlers[charecteristic], data, length, false);
}

static uint8_t notify_func(struct bt_conn *conn,
                           struct bt_gatt_subscribe_params *params,
                           const void *data, uint16_t length)
{
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
    static char *data_s;
    data_s = data;

    // Set end of string value to ignore the rest of the pointer value.
    // data_s[length] = '\0';

    if (subscribe_params[CONTROL_SAMPLING_ON].value_handle == params->value_handle)
    {
        data_s = "4\0";
        printk("FOUND SAMPLING\n");
        rs485_write("4");
    }

    // printk("[NOTIFICATION] handle %x\n", params->value_handle);
    // printk("Data %s\n", data_s);
    // printk("Length %u\n", length);

    return BT_GATT_ITER_CONTINUE;
}

static void print_chrc_props(uint8_t properties)
{
    printk("Properties: ");

    if (properties & BT_GATT_CHRC_BROADCAST)
    {
        printk("[bcast]");
    }

    if (properties & BT_GATT_CHRC_READ)
    {
        printk("[read]");
    }

    if (properties & BT_GATT_CHRC_WRITE)
    {
        printk("[write]");
    }

    if (properties & BT_GATT_CHRC_WRITE_WITHOUT_RESP)
    {
        printk("[write w/w rsp]");
    }

    if (properties & BT_GATT_CHRC_NOTIFY)
    {
        printk("[notify]");
    }

    if (properties & BT_GATT_CHRC_INDICATE)
    {
        printk("[indicate]");
    }

    if (properties & BT_GATT_CHRC_AUTH)
    {
        printk("[auth]");
    }

    if (properties & BT_GATT_CHRC_EXT_PROP)
    {
        printk("[ext prop]");
    }

    printk("\n");
}

//-------------------------------------------------------------------------------------------------
// DISCOVER FUNCTIONS
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

    int err = bt_gatt_discover(default_conn, &discover_params);
    err ? printk("Discover failed (err %d)\n", err) : printk("Discover Service...\n");
}

void discover_data_characteristic(data_characteristic_t target_characteristic,
                                  struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr)
{
    switch (target_characteristic)
    {
    case DATA_DEPTH:
        memcpy(&uuid128, BT_UUID_DEPTH, sizeof(uuid128));
        break;
    case DATA_PRESSURE:
        memcpy(&uuid128, BT_UUID_PRESSURE, sizeof(uuid128));
        break;
    case DATA_TEMPERATURE:
        memcpy(&uuid128, BT_UUID_TEMPERATURE, sizeof(uuid128));
        break;
    }

    discover_params.uuid = &uuid128.uuid;
    discover_params.start_handle = attr->handle + 1;
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

    int err = bt_gatt_discover(conn, &discover_params);
    err ? printk("Discover failed (err %d)\n", err) : printk("Discover Chacateristic...\n");
}

void discover_control_characteristic(control_characteristic_t target_characteristic,
                                     struct bt_conn *conn,
                                     const struct bt_gatt_attr *attr)
{
    switch (target_characteristic)
    {
    case CONTROL_SYSTEM_ON:
        memcpy(&uuid128, BT_UUID_SYSTEM_ON, sizeof(uuid128));
        break;
    case CONTROL_SAMPLING_ON:
        memcpy(&uuid128, BT_UUID_SAMPLING_ON, sizeof(uuid128));
        break;
    case CONTROL_RS232_ON:
        memcpy(&uuid128, BT_UUID_RS232_ON, sizeof(uuid128));
        break;
    case CONTROL_12V_ON:
        memcpy(&uuid128, BT_UUID_12V_ON, sizeof(uuid128));
        break;
    case CONTROL_SAMPLING_TIME:
        memcpy(&uuid128, BT_UUID_SAMPLING_TIME, sizeof(uuid128));
        break;
    }

    discover_params.uuid = &uuid128.uuid;
    discover_params.start_handle = attr->handle + 1;
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

    int err = bt_gatt_discover(conn, &discover_params);
    err ? printk("Discover failed (err %d)\n", err) : printk("Discover Chacateristic...\n");
}

void subscribe_to_control_characteristic(struct bt_conn *conn,
                                         const struct bt_gatt_attr *attr)
{
    subscribe_params[control_characteristic].notify = notify_func;
    subscribe_params[control_characteristic].value = BT_GATT_CCC_NOTIFY;
    subscribe_params[control_characteristic].ccc_handle = attr->handle;

    int err = bt_gatt_subscribe(conn, &subscribe_params[control_characteristic]);
    (err && err != -EALREADY) ? printk("Subscribe failed (err %d)\n", err) : printk("[SUBSCRIBED] to handle %x \n", attr->handle - 1);
}

void discover_description(struct bt_conn *conn,
                          const struct bt_gatt_attr *attr)
{

    memcpy(&uuid128, BT_UUID_GATT_CCC, sizeof(uuid128));
    discover_params.uuid = &uuid128.uuid;
    discover_params.start_handle = attr->handle + 2;
    discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    subscribe_params[control_characteristic].value_handle = bt_gatt_attr_value_handle(attr);

    int err = bt_gatt_discover(conn, &discover_params);
    err ? printk("Discover failed (err %d)\n", err) : printk("Discover Description...\n");
}

static uint8_t discover_func(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             struct bt_gatt_discover_params *params)
{

    struct bt_gatt_service_val *gatt_service;
    struct bt_gatt_chrc *gatt_chrc;
    struct bt_gatt_include *gatt_include;
    char str[BT_UUID_STR_LEN];
    struct bt_gatt_write_params *write_params;

    int err;

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
            discover_data_characteristic(data_characteristic, conn, attr);
        }
        else if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_CONTROL))
        {
            printk("[SERVICE] control\n");
            discover_control_characteristic(control_characteristic, conn, attr);
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

            discover_data_characteristic(data_characteristic, conn, attr);
            break;

        case SERVICE_CONTROL:

            if (control_characteristic == CONTROL_CHARACTERISTICS_MAX)
            {
                // Remove discovery parameters (ENDS DISCOVERY)
                memset(&discover_params, 0, sizeof(discover_params));
                return BT_GATT_ITER_STOP;
            }
            discover_description(conn, attr);

            break;
        }

        break;
    case BT_GATT_DISCOVER_DESCRIPTOR:
        subscribe_to_control_characteristic(conn, attr);

        control_characteristic++;

        discover_control_characteristic(control_characteristic, conn, attr);

        break;
    }

    return BT_GATT_ITER_STOP;

    /*
    switch (params->type)
    {
    case BT_GATT_DISCOVER_SECONDARY:
    case BT_GATT_DISCOVER_PRIMARY:
        gatt_service = attr->user_data;
        bt_uuid_to_str(gatt_service->uuid, str, sizeof(str));
        printk("[SERVICE] %s found: \nstart handle %x \nend_handle %x\n", str, attr->handle, gatt_service->end_handle);
        break;
    case BT_GATT_DISCOVER_CHARACTERISTIC:
        gatt_chrc = attr->user_data;
        bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));
        printk("[CHARACTERISTIC] %s found: \nhandle %x\n", str, attr->handle);
        print_chrc_props(gatt_chrc->properties);
        break;
    case BT_GATT_DISCOVER_INCLUDE:
        gatt_include = attr->user_data;
        bt_uuid_to_str(gatt_include->uuid, str, sizeof(str));
        printk("[INCLUDE] %s found: \nhandle %x \nstart %x \nend %x\n",
               str, attr->handle,
               gatt_include->start_handle,
               gatt_include->end_handle);
        break;
    default:
        bt_uuid_to_str(attr->uuid, str, sizeof(str));
        printk("[DESCRIPTOR] %s found: \nhandle %x\n", str, attr->handle);
        break;
    }


    return BT_GATT_ITER_CONTINUE; */
}

//-------------------------------------------------------------------------------------------------
// SCANNING FUNCTIONS
static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
                         struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    int err;

    if (default_conn)
    {
        return;
    }

    // We're only interested in connectable events
    if (type != BT_GAP_ADV_TYPE_ADV_IND &&
        type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND)
    {
        return;
    }

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    printk("Device found: %s (RSSI %d)\n", addr_str, rssi);

    // connect only to devices in close proximity
    if (rssi < -60)
    {
        return;
    }

    if (bt_le_scan_stop())
    {
        return;
    }

    err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
                            BT_LE_CONN_PARAM_LONG_TIMEOUT, &default_conn);
    if (err)
    {
        printk("Create conn to %s failed (%d)\n", addr_str, err);
        start_scan();
    }
}

static void start_scan(void)
{
    int err;

    err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, device_found);
    if (err)

    {
        printk("Scanning failed to start (err %d)\n", err);
        return;
    }

    printk("Scanning successfully started\n");
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err)
    {
        printk("Failed to connect to %s (%u)\n", addr, err);

        bt_conn_unref(default_conn);
        default_conn = NULL;

        start_scan();
        return;
    }

    if (conn != default_conn)
    {
        return;
    }

    printk("Connected: %s\n", addr);

    service = SERVICE_DATA;
    data_characteristic = DATA_DEPTH;
    control_characteristic = CONTROL_SYSTEM_ON;
    discover_service(service);
}

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

    start_scan();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

//-------------------------------------------------------------------------------------------------
// INIT
void bluetooth_init(void)
{
    int err;

    err = bt_enable(NULL);
    if (err)
    {
        printk("Bluetooth init failed (err %d)\n", err);
        return 0;
    }

    printk("Bluetooth initialized\n");

    start_scan();
    return 0;
}