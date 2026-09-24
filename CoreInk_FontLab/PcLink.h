#pragma once

#include <Arduino.h>
#include <M5GFX.h>
#include <cstdint>

// Timeout waiting for the PC-side script to ACK a transferred BMP page.
extern const uint32_t kAckTimeoutMs;

bool checkScriptReady();
bool waitForAck(uint32_t timeoutMs);
void haltWithError(const String &msg);
void sendPageAndWaitAck(const String &name);
