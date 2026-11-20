#include "sensor_data.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SensorData data;
static SemaphoreHandle_t dataMutex;

void sensorDataInit() {
  dataMutex = xSemaphoreCreateMutex();
}

void sensorDataSetFlow(float lpm, float litresAdded) {
  xSemaphoreTake(dataMutex, portMAX_DELAY);
  data.flowLpm = lpm;
  data.totalLitres += litresAdded;
  xSemaphoreGive(dataMutex);
}

void sensorDataSetTof(bool ok, uint16_t distanceMm, bool valid) {
  xSemaphoreTake(dataMutex, portMAX_DELAY);
  data.tofOk = ok;
  data.distanceMm = distanceMm;
  data.tofValid = valid;
  xSemaphoreGive(dataMutex);
}

SensorData sensorDataSnapshot() {
  xSemaphoreTake(dataMutex, portMAX_DELAY);
  SensorData copy = data;
  xSemaphoreGive(dataMutex);
  return copy;
}
