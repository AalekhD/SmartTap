#include "debug_console.h"

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor_data.h"

static void debugTask(void *) {
  const TickType_t period = pdMS_TO_TICKS(1000);
  TickType_t lastWake = xTaskGetTickCount();

  for (;;) {
    vTaskDelayUntil(&lastWake, period);

    SensorData snap = sensorDataSnapshot();
    Serial.printf("Flow: %.2f L/min | Total: %.3f L | ToF: ", snap.flowLpm, snap.totalLitres);
    if (!snap.tofOk) {
      Serial.println("not initialised");
    } else if (!snap.tofValid) {
      Serial.println("out of range");
    } else {
      Serial.printf("%u mm\n", snap.distanceMm);
    }
  }
}

void debugConsoleInit() {
  xTaskCreate(debugTask, "debug", 4096, nullptr, 1, nullptr);
}
