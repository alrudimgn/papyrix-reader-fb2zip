#include "PowerDebug.h"

#include <Arduino.h>
#include <InputManager.h>
#include <SDCardManager.h>

#include "PapyrixSettings.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

extern InputManager inputManager;

namespace papyrix::powerdebug {

namespace {

constexpr char LOG_PATH[] = "/papyrix_power_debug.log";
constexpr size_t MAX_LOG_SIZE = 128 * 1024;
constexpr uint32_t RTC_MAGIC = 0x50574442;  // "PWDB"

struct RtcPowerEvent {
  uint32_t magic = 0;
  uint32_t seq = 0;
  uint32_t uptimeMs = 0;
  int resetReason = 0;
  int wakeupCause = 0;
  int powerPinLevel = -1;
  char event[48] = {};
};

RTC_DATA_ATTR RtcPowerEvent rtcPowerEvent;
RTC_DATA_ATTR uint32_t rtcPowerSeq = 0;

const char* resetReasonName(const esp_reset_reason_t reason) {
  switch (reason) {
    case ESP_RST_UNKNOWN:
      return "unknown";
    case ESP_RST_POWERON:
      return "poweron";
    case ESP_RST_EXT:
      return "ext";
    case ESP_RST_SW:
      return "software";
    case ESP_RST_PANIC:
      return "panic";
    case ESP_RST_INT_WDT:
      return "int_wdt";
    case ESP_RST_TASK_WDT:
      return "task_wdt";
    case ESP_RST_WDT:
      return "wdt";
    case ESP_RST_DEEPSLEEP:
      return "deepsleep";
    case ESP_RST_BROWNOUT:
      return "brownout";
    case ESP_RST_SDIO:
      return "sdio";
    case ESP_RST_USB:
      return "usb";
    case ESP_RST_JTAG:
      return "jtag";
    case ESP_RST_EFUSE:
      return "efuse";
    case ESP_RST_PWR_GLITCH:
      return "power_glitch";
    case ESP_RST_CPU_LOCKUP:
      return "cpu_lockup";
    default:
      return "other";
  }
}

const char* wakeupCauseName(const esp_sleep_wakeup_cause_t cause) {
  switch (cause) {
    case ESP_SLEEP_WAKEUP_UNDEFINED:
      return "undefined";
    case ESP_SLEEP_WAKEUP_EXT0:
      return "ext0";
    case ESP_SLEEP_WAKEUP_EXT1:
      return "ext1";
    case ESP_SLEEP_WAKEUP_TIMER:
      return "timer";
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      return "touchpad";
    case ESP_SLEEP_WAKEUP_ULP:
      return "ulp";
    case ESP_SLEEP_WAKEUP_GPIO:
      return "gpio";
    case ESP_SLEEP_WAKEUP_UART:
      return "uart";
    case ESP_SLEEP_WAKEUP_WIFI:
      return "wifi";
    case ESP_SLEEP_WAKEUP_COCPU:
      return "cocpu";
    case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG:
      return "cocpu_trap";
    case ESP_SLEEP_WAKEUP_BT:
      return "bt";
    default:
      return "other";
  }
}

int readPowerPinLevel() { return digitalRead(InputManager::POWER_BUTTON_PIN); }

uint16_t readAdcStable(const uint8_t pin) {
  (void)analogRead(pin);
  return static_cast<uint16_t>(analogRead(pin));
}

bool appendLine(const char* line) {
  if (!SdMan.ready()) return false;

  FsFile file = SdMan.open(LOG_PATH, O_RDWR | O_CREAT | O_AT_END);
  if (!file) return false;

  if (file.size() > MAX_LOG_SIZE) {
    file.close();
    SdMan.remove(LOG_PATH);
    file = SdMan.open(LOG_PATH, O_RDWR | O_CREAT | O_TRUNC);
    if (!file) return false;
    file.println("# Papyrix power debug log rotated");
  }

  const bool ok = file.println(line) > 0;
  file.sync();
  file.close();
  return ok;
}

void formatAndAppend(const char* event, const char* details) {
  char line[1024];
  const int powerPin = readPowerPinLevel();
  std::snprintf(line, sizeof(line),
                "[%lu ms] %s reset=%s(%d) wake=%s(%d) pwr_pin=%d pwr_pressed=%u heap=%lu min_heap=%lu max_alloc=%lu %s",
                millis(), event, resetReasonName(esp_reset_reason()), static_cast<int>(esp_reset_reason()),
                wakeupCauseName(esp_sleep_get_wakeup_cause()), static_cast<int>(esp_sleep_get_wakeup_cause()), powerPin,
                powerPin == LOW ? 1U : 0U, static_cast<unsigned long>(ESP.getFreeHeap()),
                static_cast<unsigned long>(ESP.getMinFreeHeap()), static_cast<unsigned long>(ESP.getMaxAllocHeap()),
                details ? details : "");
  appendLine(line);
}

}  // namespace

void markRtcEvent(const char* event) {
  rtcPowerEvent.magic = RTC_MAGIC;
  rtcPowerEvent.seq = ++rtcPowerSeq;
  rtcPowerEvent.uptimeMs = millis();
  rtcPowerEvent.resetReason = static_cast<int>(esp_reset_reason());
  rtcPowerEvent.wakeupCause = static_cast<int>(esp_sleep_get_wakeup_cause());
  rtcPowerEvent.powerPinLevel = readPowerPinLevel();
  std::strncpy(rtcPowerEvent.event, event ? event : "unknown", sizeof(rtcPowerEvent.event) - 1);
  rtcPowerEvent.event[sizeof(rtcPowerEvent.event) - 1] = '\0';
}

void flushRtcEvent(const char* stage) {
  if (rtcPowerEvent.magic != RTC_MAGIC) return;

  char details[256];
  std::snprintf(details, sizeof(details),
                "stage=%s rtc_seq=%lu rtc_event=%s rtc_uptime=%lu rtc_reset=%d rtc_wake=%d rtc_pwr_pin=%d",
                stage ? stage : "unknown", static_cast<unsigned long>(rtcPowerEvent.seq), rtcPowerEvent.event,
                static_cast<unsigned long>(rtcPowerEvent.uptimeMs), rtcPowerEvent.resetReason,
                rtcPowerEvent.wakeupCause, rtcPowerEvent.powerPinLevel);
  formatAndAppend("rtc.last_power_event", details);
  rtcPowerEvent = {};
}

void logEvent(const char* event) { formatAndAppend(event ? event : "unknown", ""); }

void logf(const char* event, const char* format, ...) {
  char details[256];
  details[0] = '\0';

  if (format) {
    va_list args;
    va_start(args, format);
    std::vsnprintf(details, sizeof(details), format, args);
    va_end(args);
  }

  formatAndAppend(event ? event : "unknown", details);
}

void logInputSnapshot(const char* stage) {
  const uint16_t adcBtn1 = readAdcStable(InputManager::BUTTON_ADC_PIN_1);
  const uint16_t adcBtn2 = readAdcStable(InputManager::BUTTON_ADC_PIN_2);
  const int powerLevel = readPowerPinLevel();
  const uint8_t rawState = inputManager.getState();
  char details[256];
  std::snprintf(details, sizeof(details),
                "stage=%s adc_btn1=%u adc_btn2=%u raw_state=0x%02x pwr_pin=%d pwr_pressed=%u gpio0=%d gpio7=%d "
                "gpio8=%d gpio10=%d gpio12=%d gpio20=%d gpio21=%d",
                stage ? stage : "unknown", adcBtn1, adcBtn2, rawState, powerLevel, powerLevel == LOW ? 1U : 0U,
                digitalRead(0), digitalRead(7), digitalRead(8), digitalRead(10), digitalRead(12), digitalRead(20),
                digitalRead(21));
  formatAndAppend("input.snapshot", details);
}

void logSettingsSnapshot(const char* stage, const papyrix::Settings& settings) {
  char details[384];
  std::snprintf(details, sizeof(details),
                "stage=%s sleep_screen=%u status_bar=%u text_layout=%u short_power=%u orientation=%u font_size=%u "
                "ppr=%u side_layout=%u auto_sleep=%u paragraph=%u hyphenation=%u aa=%u show_images=%u startup=%u "
                "line_spacing=%u front_layout=%u full_book=%u ui_lang=%u pending=%u return_to=%u boot_guard=%u "
                "theme=%s",
                stage ? stage : "unknown", static_cast<unsigned>(settings.sleepScreen),
                static_cast<unsigned>(settings.statusBar), static_cast<unsigned>(settings.textLayout),
                static_cast<unsigned>(settings.shortPwrBtn), static_cast<unsigned>(settings.orientation),
                static_cast<unsigned>(settings.fontSize), static_cast<unsigned>(settings.pagesPerRefresh),
                static_cast<unsigned>(settings.sideButtonLayout), static_cast<unsigned>(settings.autoSleepMinutes),
                static_cast<unsigned>(settings.paragraphAlignment), static_cast<unsigned>(settings.hyphenation),
                static_cast<unsigned>(settings.textAntiAliasing), static_cast<unsigned>(settings.showImages),
                static_cast<unsigned>(settings.startupBehavior), static_cast<unsigned>(settings.lineSpacing),
                static_cast<unsigned>(settings.frontButtonLayout), static_cast<unsigned>(settings.fullBookProcess),
                static_cast<unsigned>(settings.uiLanguage), static_cast<unsigned>(settings.pendingTransition),
                static_cast<unsigned>(settings.transitionReturnTo), static_cast<unsigned>(settings.lastBookBootGuard),
                settings.themeName);
  formatAndAppend("settings.values", details);

  char pathDetails[768];
  std::snprintf(pathDetails, sizeof(pathDetails), "stage=%s last_book=%s", stage ? stage : "unknown",
                settings.lastBookPath);
  formatAndAppend("settings.last_book", pathDetails);

  std::snprintf(pathDetails, sizeof(pathDetails), "stage=%s file_dir=%s file_selected=%s", stage ? stage : "unknown",
                settings.fileListDir, settings.fileListSelectedName);
  formatAndAppend("settings.file_list", pathDetails);
}

void logBootSnapshot(const char* stage, const esp_reset_reason_t resetReason,
                     const esp_sleep_wakeup_cause_t wakeupCause, const bool usbConnected, const bool powerButtonBoot,
                     const bool usbColdBoot, const bool isX3, const uint8_t shortPowerAction,
                     const uint16_t powerButtonDurationMs) {
  char details[256];
  const int powerPin = readPowerPinLevel();
  std::snprintf(details, sizeof(details),
                "stage=%s reset_snapshot=%s(%d) wake_snapshot=%s(%d) usb=%u power_boot=%u usb_cold=%u device=%s "
                "short_power=%u power_duration=%u pwr_pin_snapshot=%d pwr_pressed_snapshot=%u",
                stage ? stage : "unknown", resetReasonName(resetReason), static_cast<int>(resetReason),
                wakeupCauseName(wakeupCause), static_cast<int>(wakeupCause), usbConnected ? 1U : 0U,
                powerButtonBoot ? 1U : 0U, usbColdBoot ? 1U : 0U, isX3 ? "X3" : "X4",
                static_cast<unsigned>(shortPowerAction), static_cast<unsigned>(powerButtonDurationMs), powerPin,
                powerPin == LOW ? 1U : 0U);
  formatAndAppend("boot.snapshot", details);
}

}  // namespace papyrix::powerdebug
