#include "relay_control.h"

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static constexpr uint32_t TOGGLE_MS = 5000;

static uint8_t relayPin;

#if RELAY_TOGGLE_ENABLED
static void setRelay(bool on, uint32_t now) {
  digitalWrite(relayPin, on ? HIGH : LOW);
  Serial.printf("Relay %s at %lu ms\n", on ? "ON" : "OFF", (unsigned long)now);
}

static void relayTask(void *) {
  bool on = false;
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(TOGGLE_MS));
    on = !on;
    setRelay(on, millis());
  }
}
#endif

void relayControlInit(uint8_t pin) {
  relayPin = pin;
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
#if RELAY_TOGGLE_ENABLED
  xTaskCreate(relayTask, "relay", 4096, nullptr, 2, nullptr);
#endif
}
