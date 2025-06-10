/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitPbHub
*/
#include <gtest/gtest.h>
#include <Wire.h>
#include <M5Unified.h>
#include <M5UnitUnified.hpp>
#include <googletest/test_template.hpp>
#include <unit/unit_PbHub.hpp>
#include <random>
#include <vector>

using namespace m5::unit::googletest;
using namespace m5::unit;
using namespace m5::unit::pbhub;

const ::testing::Environment* global_fixture = ::testing::AddGlobalTestEnvironment(new GlobalFixture<400000U>());

class TestPbHub : public ComponentTestBase<UnitPbHub, bool> {
protected:
    virtual UnitPbHub* get_instance() override
    {
        auto ptr = new m5::unit::UnitPbHub();
        return ptr;
    }
    virtual bool is_using_hal() const override
    {
        return GetParam();
    };
};

// INSTANTIATE_TEST_SUITE_P(ParamValues, TestPbHub, ::testing::Values(false, true));
// INSTANTIATE_TEST_SUITE_P(ParamValues, TestPbHub,::testing::Values(true));
INSTANTIATE_TEST_SUITE_P(ParamValues, TestPbHub, ::testing::Values(false));

namespace {
auto rng = std::default_random_engine{};
}

TEST_P(TestPbHub, Digital)
{
    SCOPED_TRACE(ustr);

    for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
        auto s = m5::utility::formatString("CH:%u", ch);
        SCOPED_TRACE(s);

        EXPECT_TRUE(unit->writeDigital0(ch, true));
        EXPECT_TRUE(unit->writeDigital0(ch, false));
        EXPECT_TRUE(unit->writeDigital1(ch, true));
        EXPECT_TRUE(unit->writeDigital1(ch, false));
    }

    for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
        auto s = m5::utility::formatString("CH:%u", ch);
        SCOPED_TRACE(s);

        bool b0{}, b1{};
        EXPECT_TRUE(unit->readDigital0(b0, ch));
        EXPECT_TRUE(unit->readDigital1(b1, ch));
    }
}

TEST_P(TestPbHub, Analog)
{
    SCOPED_TRACE(ustr);

    auto ver = unit->firmwareVersion();
    bool can = ver == 0x00;  // PbHub
    M5_LOGI("%02X writedAnalog %s", unit->firmwareVersion(), can ? "supported" : "NOT supported");

    for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
        auto s = m5::utility::formatString("CH:%u", ch);
        SCOPED_TRACE(s);

        uint16_t v{};
        EXPECT_TRUE(unit->readAnalog0(v, ch));
    }

    if (can) {
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            uint8_t v = rng() & 0xFF;
            EXPECT_TRUE(unit->writeAnalog0(ch, v));
            EXPECT_TRUE(unit->writeAnalog1(ch, v));
        }
    } else
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            uint8_t v = rng() & 0xFF;
            EXPECT_FALSE(unit->writeAnalog0(ch, v));
            EXPECT_FALSE(unit->writeAnalog1(ch, v));
        }
}

TEST_P(TestPbHub, PWM)
{
    SCOPED_TRACE(ustr);

    auto ver = unit->firmwareVersion();
    bool can = ver != 0xFF && ver;  // PbHuub  v1.1
    M5_LOGI("%02X PWM %s", unit->firmwareVersion(), can ? "supported" : "NOT supported");

    if (can) {
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            uint8_t out = rng() & 0xFF;
            uint8_t v{};
            EXPECT_TRUE(unit->writePWM0(ch, out));
            EXPECT_TRUE(unit->writePWM1(ch, out));

            EXPECT_TRUE(unit->readPWM0(v, ch));
            EXPECT_TRUE(unit->readPWM1(v, ch));
        }
    } else {
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            uint8_t out = rng() & 0xFF;
            uint8_t v{};
            EXPECT_FALSE(unit->writePWM0(ch, out));
            EXPECT_FALSE(unit->writePWM1(ch, out));

            EXPECT_FALSE(unit->readPWM0(v, ch));
            EXPECT_FALSE(unit->readPWM1(v, ch));
        }
    }
}

TEST_P(TestPbHub, LED)
{
    SCOPED_TRACE(ustr);

    auto ver = unit->firmwareVersion();
    bool can = ver != 0xFF && ver >= 2;  // PbHuub  v1.1 FW 2 or later
    M5_LOGI("%02X LED mode %s", unit->firmwareVersion(), can ? "supported" : "NOT supported");

    for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
        auto s = m5::utility::formatString("CH:%u", ch);
        SCOPED_TRACE(s);

        EXPECT_TRUE(unit->writeLEDCount(ch, 0));
        EXPECT_FALSE(unit->writeLEDCount(ch, UnitPbHub::MAX_LED_COUNT + 1));
        EXPECT_FALSE(unit->writeLEDCount(ch, 255));
        EXPECT_TRUE(unit->writeLEDCount(ch, UnitPbHub::MAX_LED_COUNT));
    }

    const uint32_t color = rng();
    for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
        auto s = m5::utility::formatString("CH:%u", ch);
        SCOPED_TRACE(s);

        EXPECT_TRUE(unit->writeLEDColor(ch, 0, color));
        EXPECT_TRUE(unit->writeLEDColor(ch, UnitPbHub::MAX_LED_COUNT - 1, color));
        EXPECT_FALSE(unit->writeLEDColor(ch, UnitPbHub::MAX_LED_COUNT, color));
        EXPECT_FALSE(unit->writeLEDColor(ch, 255, color));
    }

    for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
        auto s = m5::utility::formatString("CH:%u", ch);
        SCOPED_TRACE(s);

        EXPECT_TRUE(unit->fillLEDColor(ch, color));
        EXPECT_TRUE(unit->fillLEDColor(ch, color, 0, 1));
        EXPECT_TRUE(unit->fillLEDColor(ch, color, 0, UnitPbHub::MAX_LED_COUNT));
        EXPECT_FALSE(unit->fillLEDColor(ch, color, 0, UnitPbHub::MAX_LED_COUNT + 1));
        EXPECT_FALSE(unit->fillLEDColor(ch, color, 0, 255));
        EXPECT_FALSE(unit->fillLEDColor(ch, color, 1, UnitPbHub::MAX_LED_COUNT));
        EXPECT_FALSE(unit->fillLEDColor(ch, color, UnitPbHub::MAX_LED_COUNT, 1));
        EXPECT_FALSE(unit->fillLEDColor(ch, color, 255, 0));
    }

    for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
        auto s = m5::utility::formatString("CH:%u", ch);
        SCOPED_TRACE(s);

        EXPECT_TRUE(unit->writeLEDBrightness(ch, 0));
        EXPECT_TRUE(unit->writeLEDBrightness(ch, 255));
        uint8_t br = rng();
        EXPECT_TRUE(unit->writeLEDBrightness(ch, br)) << br;
    }

    if (can) {
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            EXPECT_TRUE(unit->writeLEDMode(LEDMode::SK6822));
            LEDMode m{};
            EXPECT_TRUE(unit->readLEDMode(m));
            EXPECT_EQ(m, LEDMode::SK6822);

            EXPECT_TRUE(unit->writeLEDMode(LEDMode::WS28xx));
            EXPECT_TRUE(unit->readLEDMode(m));
            EXPECT_EQ(m, LEDMode::WS28xx);
        }

    } else {
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            EXPECT_FALSE(unit->writeLEDMode(LEDMode::SK6822));
            LEDMode m{};
            EXPECT_FALSE(unit->readLEDMode(m));
            EXPECT_EQ(m, LEDMode::Unknown);

            EXPECT_FALSE(unit->writeLEDMode(LEDMode::WS28xx));
            EXPECT_FALSE(unit->readLEDMode(m));
            EXPECT_EQ(m, LEDMode::Unknown);
        }
    }
}

TEST_P(TestPbHub, Servo)
{
    auto ver = unit->firmwareVersion();
    bool can = ver != 0xFF && ver;  // PbHuub  v1.1
    M5_LOGI("%02X Servo %s", unit->firmwareVersion(), can ? "supported" : "NOT supported");

    if (can) {
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            EXPECT_TRUE(unit->writeServo0Angle(ch, 0));
            EXPECT_TRUE(unit->writeServo0Angle(ch, 90));
            EXPECT_TRUE(unit->writeServo0Angle(ch, 180));
            EXPECT_FALSE(unit->writeServo0Angle(ch, 181));
            EXPECT_FALSE(unit->writeServo0Angle(ch, 255));

            EXPECT_TRUE(unit->writeServo1Angle(ch, 0));
            EXPECT_TRUE(unit->writeServo1Angle(ch, 90));
            EXPECT_TRUE(unit->writeServo1Angle(ch, 180));
            EXPECT_FALSE(unit->writeServo1Angle(ch, 181));
            EXPECT_FALSE(unit->writeServo1Angle(ch, 255));

            EXPECT_TRUE(unit->writeServo0Pulse(ch, 500));
            EXPECT_TRUE(unit->writeServo0Pulse(ch, 1500));
            EXPECT_TRUE(unit->writeServo0Pulse(ch, 2500));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 0));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 499));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 2501));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 65535));

            EXPECT_TRUE(unit->writeServo1Pulse(ch, 500));
            EXPECT_TRUE(unit->writeServo1Pulse(ch, 1500));
            EXPECT_TRUE(unit->writeServo1Pulse(ch, 2500));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 0));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 499));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 2501));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 65535));

            uint8_t a{};
            uint16_t p{};
            EXPECT_TRUE(unit->readServo0Angle(a, ch));
            EXPECT_TRUE(unit->readServo1Angle(a, ch));
            EXPECT_TRUE(unit->readServo0Pulse(p, ch));
            EXPECT_TRUE(unit->readServo1Pulse(p, ch));
        }
    } else {
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            EXPECT_FALSE(unit->writeServo0Angle(ch, 0));
            EXPECT_FALSE(unit->writeServo0Angle(ch, 90));
            EXPECT_FALSE(unit->writeServo0Angle(ch, 180));
            EXPECT_FALSE(unit->writeServo0Angle(ch, 181));
            EXPECT_FALSE(unit->writeServo0Angle(ch, 255));

            EXPECT_FALSE(unit->writeServo1Angle(ch, 0));
            EXPECT_FALSE(unit->writeServo1Angle(ch, 90));
            EXPECT_FALSE(unit->writeServo1Angle(ch, 180));
            EXPECT_FALSE(unit->writeServo1Angle(ch, 181));
            EXPECT_FALSE(unit->writeServo1Angle(ch, 255));

            EXPECT_FALSE(unit->writeServo0Pulse(ch, 500));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 1500));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 2500));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 0));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 499));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 2501));
            EXPECT_FALSE(unit->writeServo0Pulse(ch, 65535));

            EXPECT_FALSE(unit->writeServo1Pulse(ch, 500));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 1500));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 2500));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 0));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 499));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 2501));
            EXPECT_FALSE(unit->writeServo1Pulse(ch, 65535));

            uint8_t a{};
            uint16_t p{};

            EXPECT_FALSE(unit->readServo0Angle(a, ch));
            EXPECT_FALSE(unit->readServo1Angle(a, ch));
            EXPECT_FALSE(unit->readServo0Pulse(p, ch));
            EXPECT_FALSE(unit->readServo1Pulse(p, ch));
        }
    }
}

TEST_P(TestPbHub, ChageI2CAddress)
{
    SCOPED_TRACE(ustr);

    auto prev_ver = unit->firmwareVersion();
    uint8_t ver{};

    EXPECT_FALSE(unit->changeI2CAddress(0x07));  // Invalid
    EXPECT_FALSE(unit->changeI2CAddress(0x78));  // Invalid

    // Change to 0x10
    EXPECT_TRUE(unit->changeI2CAddress(0x10));
    EXPECT_TRUE(unit->readFirmwareVersion(ver));
    EXPECT_EQ(ver, prev_ver);
    EXPECT_EQ(unit->address(), 0x10);

    // Change to 0x77
    EXPECT_TRUE(unit->changeI2CAddress(0x77));
    EXPECT_TRUE(unit->readFirmwareVersion(ver));
    EXPECT_EQ(ver, prev_ver);
    EXPECT_EQ(unit->address(), 0x77);

    // Change to 0x52
    EXPECT_TRUE(unit->changeI2CAddress(0x52));
    EXPECT_TRUE(unit->readFirmwareVersion(ver));
    EXPECT_EQ(ver, prev_ver);
    EXPECT_EQ(unit->address(), 0x52);

    // Change to default
    EXPECT_TRUE(unit->changeI2CAddress(UnitPbHub::DEFAULT_ADDRESS));
    EXPECT_TRUE(unit->readFirmwareVersion(ver));
    EXPECT_EQ(ver, prev_ver);
    EXPECT_EQ(unit->address(), +UnitPbHub::DEFAULT_ADDRESS);
}
