#include "NimBLEDevice.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

NimBLEServer *pServer;
//-------------------------------------------------------------------------------------------------
// DATA SERVICE & CHARACTERISTICS
#define SERVICE_UUID_DATA "561721cd-dbb7-462d-a651-8f542da25b61"
#define CHARACTERISTIC_UUID_DEPTH "0655982c-02f8-49bb-961d-4cb9984399b1"
#define CHARACTERISTIC_UUID_PRESSURE "6e38e4a1-d667-420d-8774-87a8c319a54c"
#define CHARACTERISTIC_UUID_TEMPERATURE "61393466-b9b0-447f-8272-1d51c378ae40"

NimBLEService *pService_data;
NimBLECharacteristic *pCharacteristic_depth;
NimBLECharacteristic *pCharacteristic_pressure;
NimBLECharacteristic *pCharacteristic_temperature;
//-------------------------------------------------------------------------------------------------
// CONTROL SERVICE & CHARACTERISTICS
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

bool ble_finished = false;
bool deviceConnected = false;
bool oldDeviceConnected = false;
std::string value = "0";

/** Handler class for characteristic actions */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
  void onRead(NimBLECharacteristic *pCharacteristic){

      
      Serial.print(pCharacteristic->getUUID().toString().c_str());
      Serial.print(": onRead(), value: ");
      Serial.println(pCharacteristic->getValue().c_str());
      
  };

  void onWrite(NimBLECharacteristic *pCharacteristic)
  {
     Serial.print(pCharacteristic->getUUID().toString().c_str());
      Serial.print(": onWrite(), value: ");
      Serial.println(pCharacteristic->getValue().c_str());

    const char *uuid = pCharacteristic->getUUID().toString().c_str();
    if (strcmp(CHARACTERISTIC_UUID_DEPTH, uuid) == 0)
    {
      publish_depth(pCharacteristic->getValue().c_str());
    }
    else if (strcmp(CHARACTERISTIC_UUID_PRESSURE, uuid) == 0)
    {
      publish_pressure(pCharacteristic->getValue().c_str());
    }
    else if (strcmp(CHARACTERISTIC_UUID_TEMPERATURE, uuid) == 0)
    {
      publish_temperature(pCharacteristic->getValue().c_str());
    }
    /*
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onWrite(), value: ");
    Serial.println(pCharacteristic->getValue().c_str()); */
  };

  /** Called before notification or indication is sent,
   *  the value can be changed here before sending if desired.
   */
  void onNotify(NimBLECharacteristic *pCharacteristic)
  {
    Serial.println("Sending notification to clients");
  };

  /** The status returned in status is defined in NimBLECharacteristic.h.
   *  The value returned in code is the NimBLE host return code.
   */
  void onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code)
  {
    String str = ("Notification/Indication status code: ");
    str += status;
    str += ", return code: ";
    str += code;
    str += ", ";
    str += NimBLEUtils::returnCodeToString(code);
    Serial.println(str);
  };

  void onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue)
  {
    String str = "Client ID: ";
    str += desc->conn_handle;
    str += " Address: ";
    str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
    if (subValue == 0)
    {
      str += " Unsubscribed to ";
    }
    else if (subValue == 1)
    {
      str += " Subscribed to notfications for ";
    }
    else if (subValue == 2)
    {
      str += " Subscribed to indications for ";
    }
    else if (subValue == 3)
    {
      str += " Subscribed to notifications and indications for ";
    }
    str += std::string(pCharacteristic->getUUID()).c_str();
    ble_finished = true;
    Serial.println(str);
  };
};

static CharacteristicCallbacks chrCallbacks;

bool ble_is_finished()
{
  return ble_finished;
}
void ble_begin(void)
{

  NimBLEDevice::init("Winch Control System");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Default +3db, now +9db

  pServer = NimBLEDevice::createServer();

  //-------------------------------------------------------------------------------------------------
  // DATA
  pService_data = pServer->createService(SERVICE_UUID_DATA);
  pCharacteristic_depth = pService_data->createCharacteristic(CHARACTERISTIC_UUID_DEPTH, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ);
  pCharacteristic_pressure = pService_data->createCharacteristic(CHARACTERISTIC_UUID_PRESSURE, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ);
  pCharacteristic_temperature = pService_data->createCharacteristic(CHARACTERISTIC_UUID_TEMPERATURE, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ);

  pCharacteristic_depth->setCallbacks(&chrCallbacks);
  pCharacteristic_pressure->setCallbacks(&chrCallbacks);
  pCharacteristic_temperature->setCallbacks(&chrCallbacks);

  pService_data->start();
  pCharacteristic_depth->setValue("-0.12");
  pCharacteristic_pressure->setValue("100310");
  pCharacteristic_temperature->setValue("24.31");

  //-------------------------------------------------------------------------------------------------
  // CONTROL
  pService_control = pServer->createService(SERVICE_UUID_CONTROL);
  pCharacteristic_system_on = pService_control->createCharacteristic(CHARACTERISTIC_UUID_SYSTEM_ON, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristic_sampling_on = pService_control->createCharacteristic(CHARACTERISTIC_UUID_SAMPLING_ON, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristic_rs232_on = pService_control->createCharacteristic(CHARACTERISTIC_UUID_RS232_ON, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristic_12v_on = pService_control->createCharacteristic(CHARACTERISTIC_UUID_12V_ON, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristic_sampling_time = pService_control->createCharacteristic(CHARACTERISTIC_UUID_SAMPLING_TIME, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

  pCharacteristic_system_on->setCallbacks(&chrCallbacks);
  pCharacteristic_sampling_on->setCallbacks(&chrCallbacks);
  pCharacteristic_rs232_on->setCallbacks(&chrCallbacks);
  pCharacteristic_12v_on->setCallbacks(&chrCallbacks);
  pCharacteristic_sampling_time->setCallbacks(&chrCallbacks);

  pService_control->start();
  pCharacteristic_system_on->setValue("1");
  pCharacteristic_sampling_on->setValue("1");
  pCharacteristic_rs232_on->setValue("0");
  pCharacteristic_12v_on->setValue("0");
  pCharacteristic_sampling_time->setValue(value);

  //-------------------------------------------------------------------------------------------------
  // ADVERTISING
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

  pAdvertising->addServiceUUID(SERVICE_UUID_DATA);
  pAdvertising->addServiceUUID(SERVICE_UUID_CONTROL);

  pAdvertising->start();

  pCharacteristic_system_on->notify();
  pCharacteristic_sampling_on->notify();
  pCharacteristic_rs232_on->notify();
  pCharacteristic_12v_on->notify();
  pCharacteristic_sampling_time->notify();
}

void ble_notify_system_on(byte *state, uint8_t length)
{
  pCharacteristic_system_on->setValue(state, length);
  pCharacteristic_system_on->notify();
}

void ble_notify_sampling_on(byte *state, uint8_t length)
{
  pCharacteristic_sampling_on->setValue(state, length);
  pCharacteristic_sampling_on->notify();
}

void ble_notify_rs232_on(byte *state, uint8_t length)
{
  pCharacteristic_rs232_on->setValue(state, length);
  pCharacteristic_rs232_on->notify();
}

void ble_notify_12v_on(byte *state, uint8_t length)
{
  pCharacteristic_12v_on->setValue(state, length);
  pCharacteristic_12v_on->notify();
}

void ble_notify_sampling_time(byte *state, uint8_t length)
{
  pCharacteristic_sampling_time->setValue(state, length);
  pCharacteristic_sampling_time->notify();
}

void ble_loop()
{

  /*
    pCharacteristic_system_on->notify();
  pCharacteristic_sampling_on->notify();
  pCharacteristic_rs232_on->notify();
  pCharacteristic_12v_on->notify();
  pCharacteristic_sampling_time->notify();*/
  // notify changed value
  // if (deviceConnected)
  // {

  //  value++;
  //  }
  //  disconnecting
  /*  if (!deviceConnected && oldDeviceConnected)
      {
          delay(500);                  // give the bluetooth stack the chance to get things ready
          pServer->startAdvertising(); // restart advertising
          Serial.println("start advertising");
          oldDeviceConnected = deviceConnected;
      }
      // connecting
      if (deviceConnected && !oldDeviceConnected)
      {
          // do stuff here on connecting
          oldDeviceConnected = deviceConnected;
      } */
}