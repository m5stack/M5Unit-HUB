/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for the child adapter of UnitPbHub (AdapterPbHub)
*/
#include <gtest/gtest.h>
#include <M5Unified.h>
#include <M5UnitUnified.hpp>
#include <googletest/test_template.hpp>
#include <unit/unit_PbHub.hpp>
#include <array>

using namespace m5::unit::googletest;
using namespace m5::unit;
using namespace m5::utility::mmh3;

namespace {

// Child unit with GPIO access, connected to a PbHub channel
class UnitPbHubChild : public Component {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitPbHubChild, 0x00);

public:
    UnitPbHubChild() : Component(0x00)
    {
    }
    virtual bool begin() override
    {
        return true;
    }
    virtual void update(const bool) override
    {
    }
};

const char UnitPbHubChild::name[] = "UnitPbHubChild";
const types::uid_t UnitPbHubChild::uid{"UnitPbHubChild"_mmh3};
const types::attr_t UnitPbHubChild::attr{types::attribute::AccessGPIO};

}  // namespace

class TestPbHubAdapter : public I2CComponentTestBase<UnitPbHub> {
protected:
    virtual UnitPbHub* get_instance() override
    {
        auto ptr = new m5::unit::UnitPbHub();
        if (ptr) {
            for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
                if (!ptr->add(children[ch], ch)) {
                    delete ptr;
                    return nullptr;
                }
            }
        }
        return ptr;
    }

    // Each child adapter is made by UnitPbHub::ensure_adapter() from the parent's impl type
    void check_child_adapters(const AdapterI2C::ImplType expected)
    {
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            auto& child = children[ch];
            EXPECT_NE(child.adapter(), nullptr);
            if (!child.adapter()) {
                continue;
            }
            EXPECT_EQ(child.adapter()->type(), Adapter::Type::I2C);
            if (child.adapter()->type() != Adapter::Type::I2C) {
                continue;
            }
            auto ad = static_cast<AdapterI2C*>(child.adapter());
            EXPECT_EQ(ad->implType(), expected);
            EXPECT_EQ(ad->address(), unit->address());
        }
    }

    // GPIO / LED requests of the children are converted to the PbHub commands
    void check_child_access()
    {
        for (uint8_t ch = 0; ch < UnitPbHub::MAX_CHANNEL; ++ch) {
            auto s = m5::utility::formatString("CH:%u", ch);
            SCOPED_TRACE(s);

            auto& child = children[ch];

            // RX (IO0)
            EXPECT_TRUE(child.pinModeRX(gpio::Mode::Output));
            EXPECT_TRUE(child.writeDigitalRX(true));
            EXPECT_TRUE(child.writeDigitalRX(false));
            bool high{};
            EXPECT_TRUE(child.readDigitalRX(high));
            uint16_t v{};
            EXPECT_TRUE(child.readAnalogRX(v));
            EXPECT_LE(v, 4095U);
            EXPECT_FALSE(child.writeAnalogRX(0));  // Not supported

            // TX (IO1)
            EXPECT_TRUE(child.pinModeTX(gpio::Mode::Output));
            EXPECT_TRUE(child.writeDigitalTX(true));
            EXPECT_TRUE(child.writeDigitalTX(false));
            EXPECT_TRUE(child.readDigitalTX(high));
            EXPECT_FALSE(child.readAnalogTX(v));   // Not supported
            EXPECT_FALSE(child.writeAnalogTX(0));  // Not supported

            // LED (RGB888 x 2)
            if (unit->firmwareVersion() >= 1) {
                const std::array<uint8_t, 6> rgb{0x10, 0x00, 0x00, 0x00, 0x10, 0x00};
                EXPECT_EQ(child.writeWithTransaction(rgb.data(), rgb.size()), m5::hal::error::error_t::OK);
                const std::array<uint8_t, 6> off{};
                EXPECT_EQ(child.writeWithTransaction(off.data(), off.size()), m5::hal::error::error_t::OK);
            }
        }
    }

    std::array<UnitPbHubChild, UnitPbHub::MAX_CHANNEL> children{};
};

TEST_F(TestPbHubAdapter, Child)
{
    SCOPED_TRACE(ustr);

    auto parent = static_cast<AdapterI2C*>(unit->adapter());
    EXPECT_NE(parent, nullptr);
    if (!parent) {
        return;
    }
    M5_LOGI("Parent impl type:%u", static_cast<uint8_t>(parent->implType()));

    check_child_adapters(parent->implType());
    check_child_access();
}

#if defined(ARDUINO) && defined(ESP_PLATFORM) && __has_include(<driver/i2c_master.h>) && \
    (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0))
// Arduino-ESP32 3.3 or later drives Wire by the ESP-IDF I2C master driver, and exposes its bus handle.
// Connecting the PbHub with the handle uses the delegating child adapter (ImplType::ESPIDFMasterBus)
class TestPbHubAdapterMasterBus : public TestPbHubAdapter {
protected:
    virtual void SetUp() override
    {
        // GROVE is not driven by Wire on these boards (NessoN1: SoftwareI2C, NanoC6/NanoH2: Ex_I2C)
        const auto board = M5.getBoard();
        if (board == m5::board_t::board_ArduinoNessoN1 || board == m5::board_t::board_M5NanoC6 ||
            board == m5::board_t::board_M5NanoH2) {
            GTEST_SKIP() << "GROVE is not driven by Wire on this board";
        }
        TestPbHubAdapter::SetUp();
    }

    virtual bool begin() override
    {
        auto pin_num_sda = M5.getPin(m5::pin_name_t::port_a_sda);
        auto pin_num_scl = M5.getPin(m5::pin_name_t::port_a_scl);
        if (i2cIsInit(0)) {
            Wire.end();
        }
        if (!Wire.begin(pin_num_sda, pin_num_scl, unit->component_config().clock)) {
            M5_LOGE("Failed to begin Wire");
            return false;
        }
        auto bus = static_cast<i2c_master_bus_handle_t>(i2cBusHandle(0));
        if (!bus) {
            M5_LOGE("Failed to get the bus handle");
            return false;
        }
        return Units.add(*unit, bus) && Units.begin();
    }
};

TEST_F(TestPbHubAdapterMasterBus, Child)
{
    SCOPED_TRACE(ustr);

    auto parent = static_cast<AdapterI2C*>(unit->adapter());
    EXPECT_NE(parent, nullptr);
    if (!parent) {
        return;
    }
    EXPECT_EQ(parent->implType(), AdapterI2C::ImplType::ESPIDFMasterBus);

    check_child_adapters(AdapterI2C::ImplType::ESPIDFMasterBus);
    check_child_access();
}
#endif
