#ifndef SIXAXISSENSOR_H
#define SIXAXISSENSOR_H
#include <M5StickC.h>

//姿勢クラス
class SixAxisSensor
{

public:
  float acc[3] = {0.0F};
  float accX = 0.0F;
  float accY = 0.0F;
  float accZ = 0.0F;

  float gyro[3] = {0.0F};
  float gyroX = 0.0F;
  float gyroY = 0.0F;
  float gyroZ = 0.0F;

  float pos[3] = {0.0F};
  float pitch = 0.0F;
  float roll = 0.0F;
  float yaw = 0.0F;

  uint64_t cnt = 0.0;

  /**
   * 
   * 初期化
   * 
   */
  void initialize();
  void getValues();
};

#endif // SIXAXISSENSOR_H