#include "NimBLEDevice.h"

// void setup() in Arduino
void ble_begin(void)  
{
    NimBLEDevice::init("NimBLE");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); //Default +3db, now +9db

    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEService *pService = pServer->createService("ABCD");
    NimBLECharacteristic *pCharacteristic = pService->createCharacteristic("1234");
    
    pService->start();
    pCharacteristic->setValue("Hello BLE");
    
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("ABCD"); 
    pAdvertising->start(); 
}
