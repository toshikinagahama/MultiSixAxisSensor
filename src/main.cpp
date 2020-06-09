#include <M5StickC.h>
#include "BluetoothSerial.h"
#include <utility/MahonyAHRS.h>

//姿勢関連
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

float pitch = 0.0F;
float roll = 0.0F;
float yaw = 0.0F;

float temp = 0.0F;

//[追加]GyroZのデータを蓄積するための変数
float stockedGyroZs[10];
int stockCnt = 0;
float adjustGyroZ = 0;
int stockedGyroZLength = 0;

//Bluetooth関連
byte counter;

BluetoothSerial bts;

void setup()
{

  stockedGyroZLength = sizeof(stockedGyroZs) / sizeof(int);

  M5.begin();
  M5.Lcd.println("Bluetooth Now");
  M5.IMU.Init();

  Serial.begin(9600);
  bts.begin("M5StickC"); //PC側で確認するときの名前

  counter = 0;
}

void loop()
{
  if (M5.BtnB.wasPressed())
  {
    esp_restart();
  }
  // put your main code here, to run repeatedly:
  M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  //[変更]これは使わない
  M5.IMU.getAhrsData(&pitch, &roll, &yaw);

  //[追加]起動時にstockedGyroZLengthの数だけデータを貯める
  // if(stockCnt<stockedGyroZLength){
  //   stockedGyroZs[stockCnt]=gyroZ;
  //   stockCnt++;
  // }else{
  //   if(adjustGyroZ==0){
  //     for(int i=0;i<stockedGyroZLength;i++){
  //       adjustGyroZ+=stockedGyroZs[i]/stockedGyroZLength;
  //     }
  //   }
  //   //貯めたデータの平均値を使ってgyroZを補正する
  //   gyroZ-=adjustGyroZ;
  //   //ここでaccelデータと補正したgyroデータを使ってpitch・roll・yawを計算する
  //   MahonyAHRSupdateIMU(gyroX * DEG_TO_RAD, gyroY * DEG_TO_RAD, gyroZ * DEG_TO_RAD, accX, accY, accZ, &pitch, &roll, &yaw);
  // }

  bts.printf("%f,%f,%f,%f,%f,%f,%f,%f,%f\r\n", accX, accY, accZ, gyroX, gyroY, gyroZ, roll, pitch, yaw);
  counter++;
  M5.update();
  delay(10);
}