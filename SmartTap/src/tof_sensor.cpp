#include "tof_sensor.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor_data.h"

static constexpr uint16_t TOF_OUT_OF_RANGE_MM = 8190;

static Adafruit_VL53L0X tof;
static bool tofReady = false;

static void tofTask(void *) {
  for (;;) {
    if (!tofReady) {
      tofReady = tof.begin();
      if (tofReady) {
        tof.startRangeContinuous();
      }
      sensorDataSetTof(tofReady, 0, false);
      if (!tofReady) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        continue;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(20));

    if (!tof.isRangeComplete()) {
      continue;
    }

    uint16_t mm = tof.readRangeResult();
    sensorDataSetTof(true, mm, mm < TOF_OUT_OF_RANGE_MM);
  }
}

void tofSensorInit(uint8_t sdaPin, uint8_t sclPin) {
  Wire.begin(sdaPin, sclPin);
  xTaskCreate(tofTask, "tof", 4096, nullptr, 2, nullptr);
}
