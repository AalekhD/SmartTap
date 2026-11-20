#include <Arduino.h>
#include "debug_console.h"
#include "flowmeter.h"
#include "relay_control.h"
#include "sensor_data.h"
#include "telemetry_uploader.h"
#include "tof_sensor.h"

constexpr uint8_t FLOW_PIN = 27;
constexpr uint8_t RELAY_PIN = 26;
constexpr uint8_t I2C_SDA_PIN = 21;
constexpr uint8_t I2C_SCL_PIN = 22;

void setup() {
  Serial.begin(115200);

  sensorDataInit();
  flowmeterInit(FLOW_PIN);
  tofSensorInit(I2C_SDA_PIN, I2C_SCL_PIN);
  relayControlInit(RELAY_PIN);
  debugConsoleInit();
  telemetryUploaderInit();
}

void loop() {
  delay(1000);
}
