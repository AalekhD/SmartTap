#pragma once

#include <stdint.h>

// Set to 0 to disable the automatic relay toggle at compile time.
// Can also be overridden from platformio.ini build_flags with -DRELAY_TOGGLE_ENABLED=0.
#ifndef RELAY_TOGGLE_ENABLED
#define RELAY_TOGGLE_ENABLED 1
#endif

void relayControlInit(uint8_t relayPin);
