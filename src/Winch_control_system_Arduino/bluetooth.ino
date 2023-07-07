#include "NimBLEDevice.h"

// DATA SERVICE
#define SERVICE_UUID_DATA "561721cd-dbb7-462d-a651-8f542da25b61"
#define CHARACTERISTIC_UUID_DEPTH "0655982c-02f8-49bb-961d-4cb9984399b1"
#define CHARACTERISTIC_UUID_PRESSURE "6e38e4a1-d667-420d-8774-87a8c319a54c"
#define CHARACTERISTIC_UUID_TEMPERATURE "61393466-b9b0-447f-8272-1d51c378ae40"

NimBLEService *pService_data;
NimBLECharacteristic *pCharacteristic_depth;
NimBLECharacteristic *pCharacteristic_pressure;
NimBLECharacteristic *pCharacteristic_temperature;

// CONTROL SERVICE
#define SERVICE_UUID_CONTROL "97ef73d0-8c29-424f-829a-1a2c78ae506a"
#define CHARACTERISTIC_UUID_SYSTEM_ON "b1cdad8d-31b9-4f80-b0d6-294b1519b524"
#define CHARACTERISTIC_UUID_SAMPLING_ON "fdd5a88d-df71-4b57-93c6-01ed143c3c3d"
#define CHARACTERISTIC_UUID_RS232_ON "8d1a814d-2889-4ce2-a228-ec44a9053bb8"
#define CHARACTERISTIC_UUID_12V_ON "2932a5b9-faee-4d50-b6da-b82b5b9739ad"
#define CHARACTERISTIC_UUID_SAMPLING_TIME "c45c93cb-aec4-4590-8b65-9ee695aa8e40"
NimBLEService *pService_control;
NimBLECharacteristic *pCharacteristic_system_on;
NimBLECharacteristic *pCharacteristic_sampling_on;
NimBLECharacteristic *pCharacteristic_rs232_on;
NimBLECharacteristic *pCharacteristic_12v_on;
NimBLECharacteristic *pCharacteristic_sampling_time;

void ble_begin(void)
{
    NimBLEDevice::init("Winch Control System");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Default +3db, now +9db

    NimBLEServer *pServer = NimBLEDevice::createServer();
    //-------------------------------------------------------------------------------------------------
    // DATA
    pService_data = pServer->createService(SERVICE_UUID_DATA);
    pCharacteristic_depth = pService_data->createCharacteristic(CHARACTERISTIC_UUID_DEPTH);
    pCharacteristic_pressure = pService_data->createCharacteristic(CHARACTERISTIC_UUID_PRESSURE);
    pCharacteristic_temperature = pService_data->createCharacteristic(CHARACTERISTIC_UUID_TEMPERATURE);

    pService_data->start();
    pCharacteristic_depth->setValue("-0.12");
    pCharacteristic_pressure->setValue("100310");
    pCharacteristic_temperature->setValue("24.31");

    //-------------------------------------------------------------------------------------------------
    // CONTROL
    pService_control = pServer->createService(SERVICE_UUID_CONTROL);
    pCharacteristic_system_on = pService_control->createCharacteristic(CHARACTERISTIC_UUID_SYSTEM_ON);
    pCharacteristic_sampling_on = pService_control->createCharacteristic(CHARACTERISTIC_UUID_SAMPLING_ON);
    pCharacteristic_rs232_on = pService_control->createCharacteristic(CHARACTERISTIC_UUID_RS232_ON);
    pCharacteristic_12v_on = pService_control->createCharacteristic(CHARACTERISTIC_UUID_12V_ON);
    pCharacteristic_sampling_time = pService_control->createCharacteristic(CHARACTERISTIC_UUID_SAMPLING_TIME);

    pService_control->start();
    pCharacteristic_system_on->setValue("1");
    pCharacteristic_sampling_on->setValue("1");
    pCharacteristic_rs232_on->setValue("0");
    pCharacteristic_12v_on->setValue("0");
    pCharacteristic_sampling_time->setValue("001");

    //-------------------------------------------------------------------------------------------------
    // ADVERTISING
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID_DATA);
    pAdvertising->addServiceUUID(SERVICE_UUID_CONTROL);

    pAdvertising->start();
}
