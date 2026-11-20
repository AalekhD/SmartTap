#include "telemetry_uploader.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "network_config.h"
#include "sensor_data.h"

static constexpr uint32_t UPLOAD_PERIOD_MS = 1000;

static void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(500));
    Serial.print('.');
  }
  Serial.printf("\nWiFi connected, IP %s\n", WiFi.localIP().toString().c_str());
}

static void uploadTask(void *) {
  connectWifi();

  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(UPLOAD_PERIOD_MS));

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi lost, reconnecting");
      WiFi.reconnect();
      continue;
    }

    SensorData snap = sensorDataSnapshot();
    char body[256];
    snprintf(body, sizeof(body),
             "{\"device\":\"esp32dev\",\"uptime_ms\":%lu,\"flow_lpm\":%.2f,"
             "\"total_litres\":%.3f,\"distance_mm\":%u,\"tof_ok\":%s,\"tof_valid\":%s}",
             (unsigned long)millis(), snap.flowLpm, snap.totalLitres, snap.distanceMm,
             snap.tofOk ? "true" : "false", snap.tofValid ? "true" : "false");

    HTTPClient http;
    http.begin(TELEMETRY_URL);
    http.addHeader("Content-Type", "application/json");
    int code = http.POST(body);
    if (code != 200) {
      Serial.printf("Telemetry POST failed: %d\n", code);
    }
    http.end();
  }
}

void telemetryUploaderInit() {
  xTaskCreate(uploadTask, "telemetry", 8192, nullptr, 1, nullptr);
}
