#include "relay_control.h"

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor_data.h"

static constexpr uint32_t TOGGLE_MS = 5000;
static constexpr uint16_t TOF_ON_THRESHOLD_MM = 50;
static constexpr uint32_t TOF_POLL_MS = 100;

static uint8_t relayPin;

static void setRelay(bool on, uint32_t now) {
  digitalWrite(relayPin, on ? HIGH : LOW);
  Serial.printf("Relay %s at %lu ms\n", on ? "ON" : "OFF", (unsigned long)now);
}

#if RELAY_TOGGLE_ENABLED
static void relayTask(void *) {
  bool on = false;
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(TOGGLE_MS));
    on = !on;
    setRelay(on, millis());
  }
}
#else
// ON when a valid ToF reading is below the threshold; OFF otherwise (including no reading).
static void relayTask(void *) {
  bool on = false;
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(TOF_POLL_MS));
    SensorData snap = sensorDataSnapshot();
    bool wantOn = snap.tofValid && snap.distanceMm < TOF_ON_THRESHOLD_MM;
    if (wantOn != on) {
      on = wantOn;
      setRelay(on, millis());
    }
  }
}
#endif

void relayControlInit(uint8_t pin) {
  relayPin = pin;
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  xTaskCreate(relayTask, "relay", 4096, nullptr, 2, nullptr);
}
