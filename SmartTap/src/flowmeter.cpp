#include "flowmeter.h"

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor_data.h"

static constexpr float FLOW_PULSES_PER_LITRE = 450.0f;

static portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t pulseCount = 0;

static void IRAM_ATTR onFlowPulse() {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulseCount++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}

static void flowTask(void *) {
  const TickType_t period = pdMS_TO_TICKS(1000);
  TickType_t lastWake = xTaskGetTickCount();

  for (;;) {
    vTaskDelayUntil(&lastWake, period);

    portENTER_CRITICAL(&pulseMux);
    uint32_t pulses = pulseCount;
    pulseCount = 0;
    portEXIT_CRITICAL(&pulseMux);

    float lpm = pulses * 60.0f / FLOW_PULSES_PER_LITRE;
    float litres = pulses / FLOW_PULSES_PER_LITRE;
    sensorDataSetFlow(lpm, litres);
  }
}

void flowmeterInit(uint8_t pin) {
  pinMode(pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin), onFlowPulse, RISING);
  xTaskCreate(flowTask, "flow", 4096, nullptr, 2, nullptr);
}
