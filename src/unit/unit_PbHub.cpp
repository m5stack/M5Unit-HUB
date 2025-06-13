/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_PbHub.cpp
  @brief PbHub Unit for M5UnitUnified
 */
#include "unit_PbHub.hpp"
#include <m5_unit_component/adapter.hpp>
#include <M5Utility.hpp>

using namespace m5::utility::mmh3;
using namespace m5::unit;
using namespace m5::unit::types;
using namespace m5::unit::pbhub;
using namespace m5::unit::pbhub::command;

namespace {

constexpr uint8_t ch_table[UnitPbHub::MAX_CHANNEL] = {0, 1, 2, 3, 4, 6};

uint8_t make_reg(const uint8_t base, const uint8_t ch, const uint8_t offset = 0)
{
    //    M5_LIB_LOGD(">>>> Reg:%02X <= base:%02x ch:%u index:%u",
    //                ((ch < UnitPbHub::MAX_CHANNEL) ? (base + offset) | (0x40 + 0x10 * ch_table[ch]) : 0x00), base, ch,
    //                offset);
    return (ch < UnitPbHub::MAX_CHANNEL) ? (base + offset) | (0x40 + 0x10 * ch_table[ch]) : 0x00;
}

constexpr uint8_t MIN_ANGLE{0};
constexpr uint8_t MAX_ANGLE{180};

constexpr uint16_t MIN_PULSE{500};
constexpr uint16_t MAX_PULSE{2500};

inline bool valid_angle(const uint8_t a)
{
    //    return a >= MIN_ANGLE && a <= MAX_ANGLE;
    return a <= MAX_ANGLE;
}

inline bool valid_pulse(const uint16_t p)
{
    return p >= MIN_PULSE && p <= MAX_PULSE;
}

}  // namespace

namespace m5 {
namespace unit {

// Adapter For children
class AdapterPbHub : public AdapterI2C {
public:
    class PbHubWireImpl : public AdapterI2C::WireImpl {
    public:
        PbHubWireImpl(TwoWire& wire, const uint8_t addr, const uint32_t clock, const uint8_t ch)
            : AdapterI2C::WireImpl(wire, addr, clock), _channel{ch}
        {
        }

        // For write LED color
        // Pass to PbHub API
        virtual m5::hal::error::error_t writeWithTransaction(const uint8_t* rgb888, const size_t len,
                                                             const uint32_t stop) override
        {
            const uint8_t reg = LED_COLOR_SINGLE_REG + 0x40 + (0x10 * (_channel == 5 ? 6 : _channel));
            const uint8_t* p  = rgb888;
            for (uint_fast16_t i = 0; i < len / 3; ++i) {
                std::array<uint8_t, 5> buf{};
                buf[0]   = i & 0xFF;
                buf[1]   = i >> 8;
                buf[2]   = *p++;
                buf[3]   = *p++;
                buf[4]   = *p++;
                auto ret = AdapterI2C::WireImpl::writeWithTransaction(reg, buf.data(), buf.size(), stop);
                if (ret != m5::hal::error::error_t::OK) {
                    return ret;
                }
            }
            return m5::hal::error::error_t::OK;
        }

        static constexpr uint8_t IO_RX{0};
        static constexpr uint8_t IO_TX{1};

        // RX
        inline virtual m5::hal::error::error_t pinModeRX(const gpio::Mode) override
        {
            return m5::hal::error::error_t::OK;  // Ignore
        }
        inline virtual m5::hal::error::error_t writeDigitalRX(const bool high) override
        {
            const uint8_t reg = make_reg(WRITE_DIGITAL_0_REG, _channel, IO_RX);
            uint8_t v[1]      = {high};
            return AdapterI2C::WireImpl::writeWithTransaction(reg, v, 1, true);
        }
        inline virtual m5::hal::error::error_t readDigitalRX(bool& high) override
        {
            return read_digital(high, IO_RX);
        }
        inline virtual m5::hal::error::error_t writeAnalogRX(const uint16_t v) override
        {
            return m5::hal::error::error_t::UNKNOWN_ERROR;
        }
        inline virtual m5::hal::error::error_t readAnalogRX(uint16_t& v)
        {
            const uint8_t reg = make_reg(READ_ANALOG_0_REG, _channel);
            return reg ? read_register16LE(reg, v) : m5::hal::error::error_t::INVALID_ARGUMENT;
        }
        // TX
        inline virtual m5::hal::error::error_t pinModeTX(const gpio::Mode) override
        {
            return m5::hal::error::error_t::OK;  // Ignore
        }
        inline virtual m5::hal::error::error_t writeDigitalTX(const bool high) override
        {
            const uint8_t reg = make_reg(WRITE_DIGITAL_0_REG, _channel, IO_TX);
            uint8_t v[1]      = {high};
            return AdapterI2C::WireImpl::writeWithTransaction(reg, v, 1, true);
        }
        inline virtual m5::hal::error::error_t readDigitalTX(bool& high) override
        {
            return read_digital(high, IO_TX);
        }
        inline virtual m5::hal::error::error_t writeAnalogTX(const uint16_t v) override
        {
            return m5::hal::error::error_t::UNKNOWN_ERROR;
        }
        inline virtual m5::hal::error::error_t readAnalogTX(uint16_t& v)
        {
            M5_LIB_LOGE("Cannot read analog1");
            return m5::hal::error::error_t::UNKNOWN_ERROR;
        }

    protected:
        m5::hal::error::error_t write_digital(const uint8_t io, const uint8_t val)
        {
            const uint8_t reg = make_reg(WRITE_DIGITAL_0_REG, _channel, io);
            return AdapterI2C::WireImpl::writeWithTransaction(reg, &val, 1, true);
        }

        m5::hal::error::error_t read_digital(bool& high, const uint8_t io)
        {
            const uint8_t reg = make_reg(READ_DIGITAL_0_REG, _channel, io);
            high              = true;
            uint8_t v{};
            auto err = read_register8(reg, v);

            //            M5_LIB_LOGE(">>>> R:%02X res:%u ch:%u io:%u", reg, err, _channel, io);

            if (err == m5::hal::error::error_t::OK) {
                high = v;
            }
            return err;
        }

        m5::hal::error::error_t read_register8(const uint8_t reg, uint8_t& v)
        {
            v        = 0;
            auto err = AdapterI2C::WireImpl::writeWithTransaction(reg, nullptr, 0U, true);
            if (err == m5::hal::error::error_t::OK) {
                uint8_t rbuf[1]{};
                err = readWithTransaction(rbuf, 1);
                if (err == m5::hal::error::error_t::OK) {
                    v = rbuf[0];
                }
            }
            return err;
        }
        m5::hal::error::error_t read_register16LE(const uint8_t reg, uint16_t& v)
        {
            v        = 0;
            auto err = AdapterI2C::WireImpl::writeWithTransaction(reg, nullptr, 0U, true);
            if (err == m5::hal::error::error_t::OK) {
                uint8_t rbuf[2]{};
                err = readWithTransaction(rbuf, 2);
                if (err == m5::hal::error::error_t::OK) {
                    v = (rbuf[1] << 8) | rbuf[0];
                }
            }
            return err;
        }

    private:
        uint8_t _channel{};
    };

#if defined(ARDUINO)
    AdapterPbHub(TwoWire& wire, uint8_t addr, const uint32_t clock, const uint8_t ch) : AdapterI2C(wire, addr, clock)
    {
        _impl.reset(new PbHubWireImpl(wire, addr, clock, ch));
    }
#endif
    AdapterPbHub(m5::hal::bus::Bus* bus, const uint8_t addr, const uint32_t clock, const uint8_t ch)
        : AdapterI2C(bus, addr, clock)
    {
    }
    AdapterPbHub(m5::hal::bus::Bus& bus, const uint8_t addr, const uint32_t clock, const uint8_t ch)
        : AdapterPbHub(&bus, addr, clock, ch)
    {
    }
};

// class UnitPbHub
const char UnitPbHub::name[] = "UnitPbHub";
const types::uid_t UnitPbHub::uid{"UnitPbHub"_mmh3};
const types::attr_t UnitPbHub::attr{attribute::AccessI2C};

UnitPbHub::UnitPbHub(const uint8_t addr) : Component(addr)
{
    auto ccfg         = component_config();
    ccfg.max_children = MAX_CHANNEL;
    ccfg.clock        = 400 * 1000U;
    component_config(ccfg);
}

bool UnitPbHub::begin()
{
    // Detect
    bool tmp{};
    for (uint8_t ch = 0; ch < MAX_CHANNEL; ++ch) {
        if (!read_digital(ch, 0, tmp)) {
            M5_LIB_LOGE("Cannot detect PbHub");
            return false;
        }
    }

    if (readFirmwareVersion(_ver)) {
        M5_LIB_LOGI("PbHub v1.1 FW:%02X", _ver);
    } else {
        _ver = 0;
        M5_LIB_LOGI("PbHub");
    }

    return true;
}

bool UnitPbHub::readAnalog0(uint16_t& val, const uint8_t ch)
{
    const uint8_t reg = make_reg(READ_ANALOG_0_REG, ch);

    val = 0;
    return reg && readRegister16LE(reg, val, 0);
}

bool UnitPbHub::writeLEDCount(const uint8_t ch, const uint16_t num)
{
    if (num > MAX_LED_COUNT) {
        M5_LIB_LOGE("Too many LEDs %u/%u", num, MAX_LED_COUNT);
        return false;
    }

    const uint8_t reg = make_reg(LED_NUM_REG, ch);
    if (reg && writeRegister16LE(reg, num)) {
        _numLED[ch] = num;
        return true;
    }
    return false;
}

bool UnitPbHub::writeLEDColor(const uint8_t ch, const uint16_t index, const uint32_t rgb888)
{
    const uint8_t reg = make_reg(LED_COLOR_SINGLE_REG, ch);

    if (index >= MAX_LED_COUNT) {
        M5_LIB_LOGE("Too many LEDs %u/%u", index, MAX_LED_COUNT);
        return false;
    }

    std::array<uint8_t, 5> buf{};
    buf[0] = index & 0xFF;
    buf[1] = index >> 8;
    buf[2] = rgb888 >> 16;   // R
    buf[3] = rgb888 >> 8;    // G
    buf[4] = rgb888 & 0xff;  // B
    // m5::utility::log::dump(buf.data(), buf.size(), false);
    return reg && writeRegister(reg, buf.data(), buf.size());
}

bool UnitPbHub::fillLEDColor(const uint8_t ch, const uint32_t rgb888, const uint16_t first, const uint16_t count)
{
    const uint8_t reg  = make_reg(LED_COLOR_MORE_REG, ch);
    const uint16_t num = count ? count : (ch < MAX_CHANNEL) ? (_numLED[ch] - first) : 0;

    if (first + num > MAX_LED_COUNT) {
        M5_LIB_LOGE("Too many LEDs %u-%u/%u", first, count, MAX_LED_COUNT);
        return false;
    }

    std::array<uint8_t, 7> buf{};
    buf[0] = first & 0xFF;
    buf[1] = first >> 8;
    buf[2] = num & 0xFF;
    buf[3] = num >> 8;
    buf[4] = rgb888 >> 16;   // R
    buf[5] = rgb888 >> 8;    // G
    buf[6] = rgb888 & 0xff;  // B
    // m5::utility::log::dump(buf.data(), buf.size(), false);
    return reg && writeRegister(reg, buf.data(), buf.size());
}

bool UnitPbHub::writeLEDBrightness(const uint8_t ch, const uint8_t value)
{
    const uint8_t reg = make_reg(LED_BRIGHTNESS_REG, ch);
    return reg && writeRegister8(reg, value);
}

bool UnitPbHub::writeLEDMode(const pbhub::LEDMode m)
{
    if (!is_firmware_2_or_later()) {
        M5_LIB_LOGE("Not support this API. Need firmware version 2 or later (%u)", _ver);
        return false;
    }
    return (m != LEDMode::Unknown) && writeRegister8(LED_MODE_REG, m5::stl::to_underlying(m));
}

bool UnitPbHub::readLEDMode(pbhub::LEDMode& m)
{
    m = LEDMode::Unknown;

    if (!is_firmware_2_or_later()) {
        M5_LIB_LOGE("Not support this API. Need firmware version 2 or later (%u)", _ver);
        return false;
    }

    uint8_t v{0xFF};
    if (readRegister8(LED_MODE_REG, v, 0)) {
        m = static_cast<LEDMode>(v);
        return true;
    }
    return false;
}

bool UnitPbHub::readFirmwareVersion(uint8_t& ver)
{
    ver = 0x00;
    return readRegister8(FIRMWARE_VERSION_REG, ver, 0);
}

bool UnitPbHub::changeI2CAddress(const uint8_t addr)
{
    if (!m5::utility::isValidI2CAddress(addr)) {
        M5_LIB_LOGE("Invalid address : %02X", addr);
        return false;
    }
    if (writeRegister8(I2C_ADDRESS_REG, addr) && changeAddress(addr)) {
        // Wait wakeup
        auto timeout_at = m5::utility::millis() + 1000;
        do {
            m5::utility::delay(1);
            uint8_t v{};
            if (readRegister8(I2C_ADDRESS_REG, v, 0) && v == addr) {
                return true;
            }
        } while (m5::utility::millis() <= timeout_at);
    }
    return false;
}

//
std::shared_ptr<Adapter> UnitPbHub::ensure_adapter(const uint8_t ch)
{
    if (ch < MAX_CHANNEL) {
        // Return new child adapter
        auto ad   = asAdapter<AdapterI2C>(Adapter::Type::I2C);
        auto impl = ad->impl();
        if (impl->getWire()) {
            return std::make_shared<AdapterPbHub>(*impl->getWire(), ad->address(), ad->clock(), ch);
        } else {
            // TODO Not implement
            //    return std::make_shared<AdapterPbHub>(impl->getBus(), ad->address(), ad->clock(), ch);
        }
    } else {
        M5_LIB_LOGE("Invalid channel %u", ch);
    }
    return std::make_shared<Adapter>();  // Empty adapter
}

bool UnitPbHub::write_digital(const uint8_t ch, const uint8_t index, const bool high)
{
    const uint8_t reg = make_reg(WRITE_DIGITAL_0_REG, ch, index);
    return reg && writeRegister8(reg, high);
}

bool UnitPbHub::read_digital(const uint8_t ch, const uint8_t index, bool& high)
{
    const uint8_t reg = make_reg(READ_DIGITAL_0_REG, ch, index);

    uint8_t v{};
    high = false;
    if (reg && readRegister8(reg, v, 0)) {
        high = v;
        return true;
    }
    return false;
}

bool UnitPbHub::write_analog(const uint8_t ch, const uint8_t index, const uint8_t val)
{
    if (is_pbhub_v11()) {
        M5_LIB_LOGE("This API cannot support PbHubv1.1");
        return false;
    }
    const uint8_t reg = make_reg(WRITE_ANALOG_0_REG, ch, index);
    return reg && writeRegister8(reg, val);
}

bool UnitPbHub::write_pwm(const uint8_t ch, const uint8_t index, const uint8_t val)
{
    if (is_pbhub()) {
        M5_LIB_LOGE("This API cannot support PbHub");
        return false;
    }
    const uint8_t reg = make_reg(PWM_0_REG, ch, index);
    return reg && writeRegister8(reg, val);
}

bool UnitPbHub::read_pwm(const uint8_t ch, const uint8_t index, uint8_t& val)
{
    if (is_pbhub()) {
        M5_LIB_LOGE("This API cannot support PbHub");
        return false;
    }
    const uint8_t reg = make_reg(PWM_0_REG, ch, index);

    val = 0;
    return reg && readRegister8(reg, val, 0);
}

bool UnitPbHub::write_servo_angle(const uint8_t ch, const uint8_t index, const uint8_t angle)
{
    if (is_pbhub()) {
        M5_LIB_LOGE("This API cannot support PbHub");
        return false;
    }
    if (!valid_angle(angle)) {
        M5_LIB_LOGE("Invalid angle %u (%u - %u)", angle, MIN_ANGLE, MAX_ANGLE);
        return false;
    }

    const uint8_t reg = make_reg(SERVO_ANGLE_0_REG, ch, index);
    return reg && writeRegister8(reg, angle);
}

bool UnitPbHub::read_servo_angle(const uint8_t ch, const uint8_t index, uint8_t& angle)
{
    if (is_pbhub()) {
        M5_LIB_LOGE("This API cannot support PbHub");
        return false;
    }
    const uint8_t reg = make_reg(SERVO_ANGLE_0_REG, ch, index);

    angle = 0;
    return reg && readRegister8(reg, angle, 0);
}

bool UnitPbHub::write_servo_pulse(const uint8_t ch, const uint8_t index, const uint16_t pulse)
{
    if (is_pbhub()) {
        M5_LIB_LOGE("This API cannot support PbHub");
        return false;
    }
    if (!valid_pulse(pulse)) {
        M5_LIB_LOGE("Invalid pulse %u (%u - %u)", pulse, MIN_PULSE, MAX_PULSE);
        return false;
    }

    const uint8_t reg = make_reg(SERVO_PULSE_0_REG, ch, index);
    return reg && writeRegister16LE(reg, pulse);
}

bool UnitPbHub::read_servo_pulse(const uint8_t ch, const uint8_t index, uint16_t& pulse)
{
    if (is_pbhub()) {
        M5_LIB_LOGE("This API cannot support PbHub");
        return false;
    }
    const uint8_t reg = make_reg(SERVO_PULSE_0_REG, ch, index);

    pulse = 0;
    return reg && readRegister16LE(reg, pulse, 0);
}

}  // namespace unit
}  // namespace m5
