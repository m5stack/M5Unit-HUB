/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Example: Example of daisy chaining Unit-PaHub
  Device => PaHub2 ch:0 => PaHub2 ch:5 => UnitVmeter (*1)
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <M5UnitUnifiedHUB.h>
#include <M5UnitUnifiedMETER.h>  // (*1) If other units are used, change accordingly

namespace {
auto& lcd = M5.Display;
m5::unit::UnitUnified Units;
// Each hub must have a different address
// https://docs.m5stack.com/en/unit/pahub2
/*
  The Unit can modify the I2C address of the device by adapting the level combination of A0~A2 pins (v2.0)
  The Unit can modify the I2C address of the device by adapting the level combination of DIP switch (v2.1)
  (The default address is 0x70)
*/
m5::unit::UnitPaHub2 hub0{};      // 1st PaHub (0x70 as default)
m5::unit::UnitPaHub2 hub1{0x71};  // 2nd PaHub

// (*1) If other units are used, change accordingly
m5::unit::UnitVmeter vmeter{};

}  // namespace

void setup()
{
    M5.begin();

    auto pin_num_sda = M5.getPin(m5::pin_name_t::port_a_sda);
    auto pin_num_scl = M5.getPin(m5::pin_name_t::port_a_scl);
    M5_LOGI("getPin: SDA:%u SCL:%u", pin_num_sda, pin_num_scl);

    Wire.end();
    Wire.begin(pin_num_sda, pin_num_scl, 400000U);

    if (!hub1.add(vmeter, 5) ||    // Connect unit to hub1 channel 5
        !hub0.add(hub1, 0) ||      // Connect hub1 to hub0 channel 0
        !Units.add(hub0, Wire) ||  // Connect hub0 to core
        !Units.begin()) {
        M5_LOGE("Failed to begin");
        M5_LOGW("%s", Units.debugInfo().c_str());

        lcd.clear(TFT_RED);
        while (true) {
            m5::utility::delay(10000);
        }
    }

    M5_LOGI("M5UnitUnified has been begun");
    M5_LOGI("%s", Units.debugInfo().c_str());
    lcd.clear(TFT_DARKGREEN);
}

void loop()
{
    M5.update();
    Units.update();
    if (vmeter.updated()) {
        // (*1) If other units are used, change accordingly
        M5.Log.printf(">Voltage:%f\n", vmeter.voltage());
    }
}
