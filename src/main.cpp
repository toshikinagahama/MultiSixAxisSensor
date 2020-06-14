#include <M5StickC.h>
#include "global.h"
#include "SixAxisSensor.h"
#include "MyBLE.h"
#include "func.h"

MyBLE *ble = new MyBLE();
SixAxisSensor *sixSensor = new SixAxisSensor();

//タイマー割り込み用

hw_timer_t *timer = NULL;

int sampling_period_us;

/**
 * 
 * イベント検知
 */
void DETECT_EVENT()
{
  timer_stop = millis();
  uint64_t elaspedTime = timer_stop - timer_start;

  if (M5.BtnA.pressedFor(1000))
  {
    event = EVT_BTN_A_LONGPRESS;
  }
  else if (M5.BtnB.pressedFor(1000))
  {
    event = EVT_BTN_B_LONGPRESS;
  }
  else if (elaspedTime < 60 * 1000)
  {
    event = EVT_TIMEOUT;
  }
  else
  {
    event = EVT_NOP;
  }
}

void sampling(void *arg)
{
  while (1)
  {
    if (state == STATE_MEAS)
    {
      if (IsConnected)
      {
        if (!IsMeasStop)
        {
          unsigned long time_start = micros();
          sixSensor->getValues();
          char val[128];
          sprintf(val, "%f,%f,%f,%f,%f,%f\r\n", sixSensor->acc[0], sixSensor->acc[1], sixSensor->acc[2], sixSensor->gyro[0], sixSensor->gyro[1], sixSensor->gyro[2]);
          ble->notify(val);
          //サンプリング分待つ
          unsigned long time_end = micros();
          while (time_end - time_start < sampling_period_us)
          {
            time_end = micros();
          }
        }
      }
    }
    else
    {
      delay(500);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  M5.begin();
  show_battery_info();

  ble->initialize();
  sixSensor->initialize();

  sampling_period_us = round(1000000 * (1.0 / 32)); //サンプリング時間の設定
  xTaskCreatePinnedToCore(sampling, "sampling", 4096, NULL, 1, NULL, 0);
}

void loop()
{
  M5.update();
  if (M5.BtnA.wasPressed())
  {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    show_battery_info();
  }

  DETECT_EVENT();

  switch (state)
  {
  case STATE_WAIT_INIT:
    M5.Lcd.printf("WAIT\n");
    ble->advertiseStop();
    setCpuFrequencyMhz(80);
    state = STATE_WAIT;
    break;
  case STATE_WAIT:
    if (event == EVT_BTN_A_LONGPRESS)
      state = STATE_ADVERTISE_INIT;
    delay(500);
    break;
  case STATE_ADVERTISE_INIT:
    timer_start = millis();
    setCpuFrequencyMhz(240);
    M5.Lcd.printf("Advertise Start\n");
    ble->advertiseStart();
    state = STATE_ADVERTISE;
    break;
  case STATE_ADVERTISE:
    if (event == EVT_BTN_B_LONGPRESS)
      state = STATE_WAIT_INIT;
    if (IsConnected)
      state = STATE_WAIT_MEAS;
    if (event == EVT_TIMEOUT)
      state = STATE_WAIT_INIT;
    break;
  case STATE_WAIT_MEAS:
    if (!IsMeasStop)
      state = STATE_MEAS;
    if (!IsConnected)
      state = STATE_WAIT_INIT;
    break;
  case STATE_MEAS:
    if (IsMeasStop)
      state = STATE_WAIT_MEAS;
    break;
  default:
    break;
  }
}