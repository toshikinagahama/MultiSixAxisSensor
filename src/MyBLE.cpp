#include <M5StickC.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "MyBLE.h"
#include "global.h"

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    M5.Lcd.println("Connect");
    IsConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    M5.Lcd.println("Disconnect");
    IsConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  //Read時
  void onRead(BLECharacteristic *pCharacteristic)
  {
    M5.Lcd.println("read");
    pCharacteristic->setValue("Hello World!");
  }

  //Write時
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    // M5.Lcd.println("write");
    std::string value = pCharacteristic->getValue();
    if (value == "1111")
    {
      IsMeasStop = false;
    }
    else if (value == "2222")
    {
      IsMeasStop = true;
    }
    Serial.println(value.c_str());
    M5.Lcd.println(value.c_str());
  }
};

/**
 * 
 * 初期化
 * 
 */
void MyBLE::initialize()
{

  BLEDevice::init("M5StickC");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pServer->setCallbacks(new MyServerCallbacks());
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());
  pAdvertising = pServer->getAdvertising();
}

/**
 * 
 * アドバタイズスタート
 */
void MyBLE::advertiseStart()
{
  pService->start();
  pAdvertising->start();
}

/**
 * 
 * アドバタイズストップ
 */
void MyBLE::advertiseStop()
{
  pService->stop();
  pAdvertising->stop();
}

/**
 * 
 * notify
 */
void MyBLE::notify(char *val)
{
  pCharacteristic->setValue(val);
  pCharacteristic->notify();
}