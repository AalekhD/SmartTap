#pragma once

#include <stdint.h>

struct SensorData {
  float flowLpm = 0.0f;
  float totalLitres = 0.0f;
  uint16_t distanceMm = 0;
  bool tofOk = false;
  bool tofValid = false;
};

void sensorDataInit();
void sensorDataSetFlow(float lpm, float litresAdded);
void sensorDataSetTof(bool ok, uint16_t distanceMm, bool valid);
SensorData sensorDataSnapshot();
