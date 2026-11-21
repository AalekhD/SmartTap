#pragma once

#include <stdint.h>

// 1 (default): relay toggles every 5 s (test mode).
// 0: relay follows ToF reading (ON when distance < 50 mm, OFF otherwise).
// Can be overridden from platformio.ini build_flags with -DRELAY_TOGGLE_ENABLED=0.
#ifndef RELAY_TOGGLE_ENABLED
#define RELAY_TOGGLE_ENABLED 1
#endif

void relayControlInit(uint8_t relayPin);
