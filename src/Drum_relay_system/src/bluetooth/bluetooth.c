/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

#define BT_LE_CONN_PARAM_LONG_TIMEOUT BT_LE_CONN_PARAM(BT_GAP_INIT_CONN_INT_MIN, \
                                                       BT_GAP_INIT_CONN_INT_MAX, \
                                                       0, 1000) // N*10ms

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

typedef uint8_t (*notify_function_callback_t)(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *data, uint16_t length);

#define NOTIFY_CHARACTERISTICS_MAX 5
static struct bt_uuid *control_characteristics_uuid[NOTIFY_CHARACTERISTICS_MAX];
static notify_function_callback_t notify_callbacks[NOTIFY_CHARACTERISTICS_MAX];
static struct bt_gatt_subscribe_params subscribe_control_characteristics[NOTIFY_CHARACTERISTICS_MAX];

enum DISCOVER_PHASE
{
    DISC_PHASE_SERVICE, /* Looking for Service UUIDs */
    DISC_PHASE_CHRC,    /* Looking for Characteristic UUIDs */
    DISC_PHASE_CCCD,    /* Looking for CCCDs */
};
typedef enum DISCOVER_PHASE discover_phase_t;
/* Phase of attribute discovery process */
static discover_phase_t discover_phase;
static uint16_t cccd_handle; /* Handle to Notify CCCD for Report */
static uint16_t ctrl_handle; /* Handle for Control Characteristic */
static uint16_t rpt_handle;  /* Handle for Report Characteristic */

static struct bt_conn *default_conn;
static struct bt_uuid_128 uuid128 = BT_UUID_INIT_128(0);
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params[NOTIFY_CHARACTERISTICS_MAX];
static volatile uint8_t service = 0;
static volatile uint8_t characteristic = 0;

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
    char *data_s = data;

    // Set end of string value to ignore the rest of the pointer value.
    data_s[length] = '\0';

    printk("[NOTIFICATION] handle %x\n", params->value_handle);
    printk("Data %s\n", data_s);
    printk("Length %u\n", length);

    // Consume data
    memset(data, '\0', sizeof(data));

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

void discover_service(uint8_t target_service)
{
    if (target_service == 0)
    {
        memcpy(&uuid128, BT_UUID_DATA, sizeof(uuid128));
    }
    else
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

void discover_control_characteristic(uint8_t target_characteristic,
                                     struct bt_conn *conn,
                                     const struct bt_gatt_attr *attr)
{
    switch (target_characteristic)
    {
    case 0:
        memcpy(&uuid128, BT_UUID_SYSTEM_ON, sizeof(uuid128));
        break;
    case 1:
        memcpy(&uuid128, BT_UUID_SAMPLING_ON, sizeof(uuid128));
        break;
    case 2:
        memcpy(&uuid128, BT_UUID_RS232_ON, sizeof(uuid128));
        break;
    case 3:
        memcpy(&uuid128, BT_UUID_12V_ON, sizeof(uuid128));
        break;
    case 4:
        memcpy(&uuid128, BT_UUID_SAMPLING_TIME, sizeof(uuid128));
        break;
    }

    discover_params.uuid = &uuid128.uuid;
    discover_params.start_handle = attr->handle + 1;
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

    int err = bt_gatt_discover(conn, &discover_params);
    err ? printk("Discover failed (err %d)\n", err) : printk("Discover Chacateristic...\n");
}

void discover_description(struct bt_conn *conn,
                          const struct bt_gatt_attr *attr)
{

    memcpy(&uuid128, BT_UUID_GATT_CCC, sizeof(uuid128));
    discover_params.uuid = &uuid128.uuid;
    discover_params.start_handle = attr->handle + 2;
    discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    subscribe_params[characteristic].value_handle = bt_gatt_attr_value_handle(attr);

    int err = bt_gatt_discover(conn, &discover_params);
    err ? printk("Discover failed (err %d)\n", err) : printk("Discover Chacateristic...\n");
}

static uint8_t discover_func(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             struct bt_gatt_discover_params *params)
{

    struct bt_gatt_service_val *gatt_service;
    struct bt_gatt_chrc *gatt_chrc;
    struct bt_gatt_include *gatt_include;
    char str[BT_UUID_STR_LEN];

    int err;

    if (!attr)
    {
        printk("Discover complete\n");
        (void)memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }

    switch (params->type)
    {
    case BT_GATT_DISCOVER_SECONDARY:
    case BT_GATT_DISCOVER_PRIMARY:
        if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_CONTROL))
        {
            discover_control_characteristic(characteristic, conn, attr);
        }
        break;
    case BT_GATT_DISCOVER_CHARACTERISTIC:
        if (service == 1)
        {
            discover_description(conn, attr);
        }
        break;
    case BT_GATT_DISCOVER_DESCRIPTOR:
        subscribe_params[characteristic].notify = notify_func;
        subscribe_params[characteristic].value = BT_GATT_CCC_NOTIFY;
        subscribe_params[characteristic].ccc_handle = attr->handle;

        err = bt_gatt_subscribe(conn, &subscribe_params[characteristic]);
        (err && err != -EALREADY) ? printk("Subscribe failed (err %d)\n", err) : printk("[SUBSCRIBED]\n");

        characteristic++;

        if (characteristic == 5)
        {
            return BT_GATT_ITER_STOP;
        }
        discover_service(1);

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

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
                         struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    int err;

    if (default_conn)
    {
        return;
    }

    /* We're only interested in connectable events */
    if (type != BT_GAP_ADV_TYPE_ADV_IND &&
        type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND)
    {
        return;
    }

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    printk("Device found: %s (RSSI %d)\n", addr_str, rssi);

    /* connect only to devices in close proximity */
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

    /* This demo doesn't require active scan */
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
    service = 1;
    characteristic = 0;

    discover_service(service);

    // bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
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

    start_scan();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

void bluetooth_init(void)
{
    int err;

    control_characteristics_uuid[0] = BT_UUID_SYSTEM_ON;
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