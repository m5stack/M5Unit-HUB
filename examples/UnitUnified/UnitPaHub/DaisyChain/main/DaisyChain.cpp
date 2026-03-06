/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Example: Example of daisy chaining Unit-PaHub
  Device ---> PaHub2 ch:4 ---> PaHub2
                                |- ch:5 UnitVmeter
                                |- ch:4 UnitKmeterISO
                                `- ch:2 UnitAmeter
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <M5UnitUnifiedHUB.h>
#include <M5UnitUnifiedMETER.h>  // (*1) If other units are used, change accordingly
#include <M5HAL.hpp>

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

m5::unit::UnitVmeter vmeter{};
m5::unit::UnitKmeterISO kmeter_iso{};
m5::unit::UnitAmeter ameter{};

}  // namespace

void setup()
{
    M5.begin();
    M5.setTouchButtonHeightByRatio(100);
    // The screen shall be in landscape mode
    if (lcd.height() > lcd.width()) {
        lcd.setRotation(1);
    }

    if (!hub1.add(vmeter, 5) ||      // Connect UnitVmeter to hub1 channel 5
        !hub1.add(kmeter_iso, 4) ||  // Connect UnitKmeterISO to hub1 channel 4
        !hub1.add(ameter, 2) ||      // Connect UnitAmeter to hub1 channel 2
        !hub0.add(hub1, 4)) {        // Connect hub1 to hub0 channel 4
        M5_LOGE("Failed to add");
        M5_LOGW("%s", Units.debugInfo().c_str());

        lcd.fillScreen(TFT_RED);
        while (true) {
            m5::utility::delay(10000);
        }
    }

    auto board = M5.getBoard();

    bool unit_ready{};
    if (board == m5::board_t::board_ArduinoNessoN1) {
        // NessoN1: Wire/Wire1 are used internally, use M5HAL SoftwareI2C for GROVE
        auto pin_num_sda = M5.getPin(m5::pin_name_t::port_b_out);
        auto pin_num_scl = M5.getPin(m5::pin_name_t::port_b_in);
        M5_LOGI("getPin(M5HAL): SDA:%u SCL:%u", pin_num_sda, pin_num_scl);

        m5::hal::bus::I2CBusConfig i2c_cfg;
        i2c_cfg.pin_sda = m5::hal::gpio::getPin(pin_num_sda);
        i2c_cfg.pin_scl = m5::hal::gpio::getPin(pin_num_scl);
        auto i2c_bus    = m5::hal::bus::i2c::getBus(i2c_cfg);
        M5_LOGI("Bus:%d", i2c_bus.has_value());
        unit_ready = Units.add(hub0, i2c_bus ? i2c_bus.value() : nullptr) && Units.begin();
    } else if (board == m5::board_t::board_M5NanoC6) {
        // NanoC6: Wire.begin() conflicts with m5gfx::i2c, use M5.Ex_I2C
        M5_LOGI("Using M5.Ex_I2C");
        unit_ready = Units.add(hub0, M5.Ex_I2C) && Units.begin();
    } else {
        auto pin_num_sda = M5.getPin(m5::pin_name_t::port_a_sda);
        auto pin_num_scl = M5.getPin(m5::pin_name_t::port_a_scl);
        M5_LOGI("getPin: SDA:%u SCL:%u", pin_num_sda, pin_num_scl);

        Wire.end();
        Wire.begin(pin_num_sda, pin_num_scl, 400000U);
        unit_ready = Units.add(hub0, Wire) && Units.begin();
    }

    if (!unit_ready) {
        M5_LOGE("Failed to begin");
        M5_LOGW("%s", Units.debugInfo().c_str());

        lcd.fillScreen(TFT_RED);
        while (true) {
            m5::utility::delay(10000);
        }
    }
    M5_LOGI("M5UnitUnified has been begun");
    M5_LOGI("%s", Units.debugInfo().c_str());
    lcd.fillScreen(TFT_DARKGREEN);
}

void loop()
{
    M5.update();
    Units.update();
    if (vmeter.updated()) {
        M5.Log.printf(">Voltage:%f\n", vmeter.voltage());
    }
    if (kmeter_iso.updated()) {
        M5.Log.printf(">KmeterTemp:%f\n", kmeter_iso.temperature());
    }
    if (ameter.updated()) {
        M5.Log.printf(">Current:%f\n", ameter.current());
    }
}
