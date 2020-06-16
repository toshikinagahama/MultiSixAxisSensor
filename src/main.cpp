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
  timer_stop_timeout = millis();
  uint64_t elaspedTime = timer_stop_timeout - timer_start_timeout;

  if (M5.BtnA.pressedFor(1000))
  {
    event = EVT_BTN_A_LONGPRESS;
  }
  else if (M5.BtnB.pressedFor(1000))
  {
    event = EVT_BTN_B_LONGPRESS;
  }
  else if (elaspedTime >= 60 * 1000)
  {
    switch (state)
    {
    case STATE_ADVERTISE:
      /* code */
      event = EVT_TIMEOUT;
      break;

    default:
      event = EVT_NOP;
      break;
    }
  }
  else
  {
    event = EVT_NOP;
  }
  // Serial.printf("%d\n", event);
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
          // Serial.printf(val);
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
      delay(1000);
    }
  }
}

void setup()
{
  M5.begin();
  Serial.begin(9600);
  M5.Axp.ScreenBreath(8);
  show_battery_info();

  ble->initialize();
  sixSensor->initialize();

  sampling_period_us = round(1000000 * (1.0 / 32)); //サンプリング時間の設定
  xTaskCreatePinnedToCore(sampling, "sampling", 4096, NULL, 1, NULL, 1);
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
    setCpuFrequencyMhz(20);
    state = STATE_WAIT;
    break;
  case STATE_WAIT:
    if (event == EVT_BTN_A_LONGPRESS)
      state = STATE_ADVERTISE_INIT;
    delay(1);
    break;
  case STATE_ADVERTISE_INIT:
    timer_start_timeout = millis();
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