#pragma once

#include <esp_sleep.h>
#include <esp_system.h>

#include <cstdint>

namespace papyrix {

struct Settings;

namespace powerdebug {

void markRtcEvent(const char* event);
void flushRtcEvent(const char* stage);
void logEvent(const char* event);
void logf(const char* event, const char* format, ...);
void logInputSnapshot(const char* stage);
void logSettingsSnapshot(const char* stage, const papyrix::Settings& settings);

void logBootSnapshot(const char* stage, esp_reset_reason_t resetReason, esp_sleep_wakeup_cause_t wakeupCause,
                     bool usbConnected, bool powerButtonBoot, bool usbColdBoot, bool isX3, uint8_t shortPowerAction,
                     uint16_t powerButtonDurationMs);

}  // namespace powerdebug

}  // namespace papyrix
