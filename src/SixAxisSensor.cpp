#include "SixAxisSensor.h"
#include <M5StickC.h>

/**
 * 
 * 初期化
 * 
 */
void SixAxisSensor::initialize()
{
  M5.IMU.Init();

  for (uint8_t i = 0; i < 3; i++)
  {
    acc[i] = 0.0F;
    gyro[i] = 0.0F;
    pos[i] = 0.0F;
  }

  cnt = 0.0;
}

/**
 * 
 * 測定
 */

void SixAxisSensor::getValues()
{
  M5.IMU.getGyroData(&gyro[0], &gyro[1], &gyro[2]);
  M5.IMU.getAccelData(&acc[0], &acc[1], &acc[2]);
  M5.MPU6886.getAhrsData(&pos[0], &pos[1], &pos[2]);
}
