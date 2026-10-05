/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Example using M5UnitUnified for UnitPbHub

  Uses the PbHub's own API only, so no other unit library is needed.
  Each channel has two pins, IO0 and IO1:
  - IO0 of every channel is read as analog input and plotted to serial
  - IO1 of every channel is a digital output, toggled by BtnA

  Connect sensors (e.g. Unit Angle) to read IO0, and LEDs etc. to see IO1.
  For using child units through the PbHub, see the ViaPbHub example of each unit library.
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <M5UnitUnifiedHUB.h>
#include <wiring/m5_unit_unified_wiring.hpp>  // wiring::addI2C / failStop

namespace {
auto& lcd = M5.Display;
m5::unit::UnitUnified Units;
m5::unit::UnitPbHub hub;

constexpr uint32_t READ_INTERVAL_MS{100};
bool output_high{};

}  // namespace

void setup()
{
    M5.begin();
    M5.setTouchButtonHeightByRatio(100);
    // The screen shall be in landscape mode
    if (lcd.height() > lcd.width()) {
        lcd.setRotation(1);
    }

    if (!m5::unit::wiring::addI2C(Units, hub) || !Units.begin()) {
        M5_LOGE("Failed to begin");
        M5_LOGW("%s", Units.debugInfo().c_str());
        m5::unit::wiring::failStop();
    }
    M5.Log.printf("M5UnitUnified initialized\n");
    M5.Log.printf("%s\n", Units.debugInfo().c_str());

    // 0: PbHub, 1 or later: PbHub v1.1
    M5.Log.printf("Firmware version:%u\n", hub.firmwareVersion());

    for (uint8_t ch = 0; ch < m5::unit::UnitPbHub::MAX_CHANNEL; ++ch) {
        hub.writeDigital1(ch, output_high);
    }
    lcd.fillScreen(TFT_DARKGREEN);
}

void loop()
{
    M5.update();
    Units.update();

    if (M5.BtnA.wasClicked()) {
        output_high = !output_high;
        for (uint8_t ch = 0; ch < m5::unit::UnitPbHub::MAX_CHANNEL; ++ch) {
            if (!hub.writeDigital1(ch, output_high)) {
                M5_LOGE("Failed to write digital ch:%u", ch);
            }
        }
        M5.Log.printf("IO1:%s\n", output_high ? "HIGH" : "LOW");
        lcd.fillScreen(output_high ? TFT_ORANGE : TFT_DARKGREEN);
    }

    static auto prev_ms = m5::utility::millis();
    const auto now_ms   = m5::utility::millis();
    if (now_ms - prev_ms >= READ_INTERVAL_MS) {
        prev_ms = now_ms;
        for (uint8_t ch = 0; ch < m5::unit::UnitPbHub::MAX_CHANNEL; ++ch) {
            uint16_t val{};
            if (hub.readAnalog0(val, ch)) {
                M5.Log.printf(">CH%u:%u\n", ch, val);
            }
        }
    }
}

#if !defined(ARDUINO)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

#if CONFIG_FREERTOS_UNICORE
// Single-core SoCs: run loop() back-to-back, but every ~2 s yield a 5 ms slice so the IDLE task
// runs and feeds the task watchdog (default 5 s).
static inline void feedIdleTaskPeriodically(void)
{
    constexpr uint32_t FEED_INTERVAL_MS   = 2000;
    constexpr TickType_t FEED_SLEEP_TICKS = pdMS_TO_TICKS(5);
    static uint32_t s_next_feed_ms        = 0;
    const uint32_t now_ms                 = static_cast<uint32_t>(esp_timer_get_time() / 1000);
    if (now_ms >= s_next_feed_ms) {
        s_next_feed_ms = now_ms + FEED_INTERVAL_MS;
        vTaskDelay(FEED_SLEEP_TICKS);
    }
}
#endif

extern "C" void app_main(void)
{
    setup();
    for (;;) {
#if CONFIG_FREERTOS_UNICORE
        feedIdleTaskPeriodically();
#endif
        loop();
    }
}
#endif
