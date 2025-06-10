/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_PbHub.hpp
  @brief PbHub Unit for M5UnitUnified
 */
#ifndef M5_UNIT_HUB_UNIT_PBHUB_HPP
#define M5_UNIT_HUB_UNIT_PBHUB_HPP

#include <M5UnitComponent.hpp>
#include <array>

class TwoWire;

namespace m5 {
namespace unit {

/*!
  @namespace pbhub
  @brief namespace for PbHub
*/
namespace pbhub {

/*!
  @enum LEDMode
  @brief LED contol type
 */
enum class LEDMode : uint8_t {
    WS28xx,  //!< WS28xx, SK6812 (as default)
    SK6822,  //!< SK6822, APA106
    Unknown = 0xFF,
};

}  // namespace pbhub

/*!
  @class m5::unit::UnitPbHub
  @brief PbHub unit
  @note Automatic identification of PbHub and PbHub v1.1
  @warning Not all Units with a black interface (PortB) support expansion through PbHUB.
  @warning PbHUB can only be applied to basic single-bus communication, through the I2C protocol
  @warning to achieve basic digital read and write, analog Read and write.
  @warning But for units such as Weight (built-in HX711) that not only need to read analog,
  @warning but also depend on the timing of the Unit, PbHUB cannot be expanded.
 */
class UnitPbHub : public Component {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitPbHub, 0x61);

public:
    constexpr static uint8_t MAX_CHANNEL{6};
    constexpr static uint8_t MAX_LED_COUNT{74};

    explicit UnitPbHub(const uint8_t addr = DEFAULT_ADDRESS);
    virtual ~UnitPbHub() = default;

    virtual bool begin() override;

    /*!
      @brief Get the firmware version
      @retval == 0 No firmware version (means PbHub)
      @retval == 0xFF Not determined because begin() has not been called.
      @retval !=0 && !=0xFF Firmware version (means PbHub v1.1)
     */
    inline uint8_t firmwareVersion() const
    {
        return _ver;
    }

    ///@name Digital R/W
    ///@{
    /*!
      @brief Write digital 0 to a specific channel
      @param ch Channel
      @param high HIGH if true, LOW if false
      @return True if successful
     */
    inline bool writeDigital0(const uint8_t ch, const bool high)
    {
        return write_digital(ch, 0, high);
    }
    /*!
      @brief Write digital 1 to a specific channel
      @param ch Channel
      @param high HIGH if true, LOW if false
      @return True if successful
     */
    inline bool writeDigital1(const uint8_t ch, const bool high)
    {
        return write_digital(ch, 1, high);
    }
    /*!
      @brief Read digital 0 from a specific channel
      @param[out] high HIGH if true, LOW if false
      @param ch Channel
      @return True if successful
     */
    inline bool readDigital0(bool& high, const uint8_t ch)
    {
        return read_digital(ch, 0, high);
    }
    /*!
      @brief Read digital 0 from a specific channel
      @param[out] high HIGH if true, LOW if false
      @param ch Channel
      @return True if successful
     */
    inline bool readDigital1(bool& high, const uint8_t ch)
    {
        return read_digital(ch, 1, high);
    }
    ///@}

    ///@warning Function in PbHub
    ///@name Analog write
    ///@{
    /*!
      @brief Write analog 0 to a specific channel
      @param ch Channel
      @param val Value
      @return True if successful
     */
    inline bool writeAnalog0(const uint8_t ch, const uint8_t val)
    {
        return write_analog(ch, 0, val);
    }
    /*!
      @brief Write analog 1 to a specific channel
      @param ch Channel
      @param val Value
      @return True if successful
     */
    inline bool writeAnalog1(const uint8_t ch, const uint8_t val)
    {
        return write_analog(ch, 0, val);
    }
    ///@}

    ///@name Analog read
    ///@{
    /*!
      @brief Read analog 0 from a specific channel
      @param[out] val 0-1023(PbHub) 0-4095(PbHub v1.1)
      @param ch Channel
      @return True if successful
     */
    bool readAnalog0(uint16_t& val, const uint8_t ch);
    ///@}

    ///@warning Function in v1.1 or later
    ///@name PWM
    ///@{
    /*!
      @brief Write PWN 0 to a specific channel
      @param ch Channel
      @param pwm PWM value
      @return True if successful
     */
    inline bool writePWM0(const uint8_t ch, const uint8_t pwm)
    {
        return write_pwm(ch, 0, pwm);
    }
    /*!
      @brief Write PWN 1 to a specific channel
      @param ch Channel
      @param pwm PWM value
      @return True if successful
     */
    inline bool writePWM1(const uint8_t ch, const uint8_t pwm)
    {
        return write_pwm(ch, 1, pwm);
    }
    /*!
      @brief Read PWN 0 from a specific channel
      @param[out] pwm PWM value
      @param ch Channel
      @return True if successful
     */
    inline bool readPWM0(uint8_t& pwm, const uint8_t ch)
    {
        return read_pwm(ch, 0, pwm);
    }
    /*!
      @brief Read PWN 0 from a specific channel
      @param[out] pwm PWM value
      @param ch Channel
      @return True if successful
     */
    inline bool readPWM1(uint8_t& pwm, const uint8_t ch)
    {
        return read_pwm(ch, 1, pwm);
    }
    ///@}

    ///@warning Function in v1.1 or later
    ///@name LED
    ///@{
    /*!
      @brief Write the number of the LED to a specific channel
      @param ch Channel
      @param num Number of th LED
      @return True if successful
      @warning Maximum LED is 74 for each channel
     */
    bool writeLEDCount(const uint8_t ch, const uint16_t num);
    /*!
      @brief Write the LED color to a specific channel
      @param ch Channel
      @param index LED index
      @param rgb888  00000000RRRRRRRRGGGGGGGGBBBBBBBB 24bits color
      @return True if successful
     */
    bool writeLEDColor(const uint8_t ch, const uint16_t index, const uint32_t rgb888);
    /*!
      @brief Fill the LED color to a specific channel
      @param ch Channel
      @param rgb888  00000000RRRRRRRRGGGGGGGGBBBBBBBB 24bits color
      @param first First position of the LEDs
      @param count Number of pixels to fill (To the end if zero)
      @return True if successful
     */
    bool fillLEDColor(const uint8_t ch, const uint32_t rgb888, const uint16_t first = 0, const uint16_t count = 0);
    /*!
      @brief Write the LED brightness to a specific channel
      @param ch Channel
      @param value 0-255
      @return True if successful
     */
    bool writeLEDBrightness(const uint8_t ch, const uint8_t value);
    /*!
      @brief Write the LED control mode
      @param m LEDMode
      @return True if successful
      @warning Settings common to all channels
      @warning Function in PbHub v1.1 firmware version 2 or later
     */
    bool writeLEDMode(const pbhub::LEDMode m);
    /*!
      @brief Read the LED control mode
      @param[out] m LEDMode
      @return True if successful
      @warning Function in PbHub v1.1 firmware version 2 or later
    */
    bool readLEDMode(pbhub::LEDMode& m);
    ///@}

    ///@warning Function in v1.1 or later
    ///@note angle valid range between 0 and 180
    ///@note pulse valid range between 500 and 2500
    ///@name Servo
    ///@{
    /*!
      @brief Write servo 0 angle to a specific channel
      @param ch Channel
      @param angle Angle value
      @return True if successful
     */
    inline bool writeServo0Angle(const uint8_t ch, const uint8_t angle)
    {
        return write_servo_angle(ch, 0, angle);
    }
    /*!
      @brief Write servo 1 angle to a specific channel
      @param ch Channel
      @param angle Angle value
      @return True if successful
     */
    inline bool writeServo1Angle(const uint8_t ch, const uint8_t angle)
    {
        return write_servo_angle(ch, 1, angle);
    }
    /*!
      @brief Write servo 0 pulse to a specific channel
      @param ch Channel
      @param pulse Pulse value
      @return True if successful
     */
    inline bool writeServo0Pulse(const uint8_t ch, const uint16_t pulse)
    {
        return write_servo_pulse(ch, 0, pulse);
    }
    /*!
      @brief Write servo 1 pulse to a specific channel
      @param ch Channel
      @param pulse Pulse value
      @return True if successful
     */
    inline bool writeServo1Pulse(const uint8_t ch, const uint16_t pulse)
    {
        return write_servo_pulse(ch, 1, pulse);
    }
    /*!
      @brief Read servo 0 angle from a specific channel
      @param[out] angle Angle value
      @param ch Channel
      @return True if successful
     */
    inline bool readServo0Angle(uint8_t& angle, const uint8_t ch)
    {
        return read_servo_angle(ch, 0, angle);
    }
    /*!
      @brief Read servo 1 angle from a specific channel
      @param[out] angle Angle value
      @param ch Channel
      @return True if successful
     */
    inline bool readServo1Angle(uint8_t& angle, const uint8_t ch)
    {
        return read_servo_angle(ch, 1, angle);
    }
    /*!
      @brief Read servo 0 pulse from a specific channel
      @param[out] pulse Pulse value
      @param ch Channel
      @return True if successful
     */
    inline bool readServo0Pulse(uint16_t& pulse, const uint8_t ch)
    {
        return read_servo_pulse(ch, 0, pulse);
    }
    /*!
      @brief Read servo 1 pulse from a specific channel
      @param[out] pulse Pulse value
      @param ch Channel
      @return True if successful
     */
    inline bool readServo1Pulse(uint16_t& pulse, const uint8_t ch)
    {
        return read_servo_pulse(ch, 1, pulse);
    }
    ///@}

    /*!
      @brief Read the firmware version
      @param[out] ver
      @return True if successful
      @warning Function in v1.1 or later
     */
    bool readFirmwareVersion(uint8_t& ver);

    /*!
      @brief Change device I2C address
      @param addr I2C address
      @return True if successful
      @warning Handling warning
      @warning Function in v1.1 or later
    */
    bool changeI2CAddress(const uint8_t addr);

protected:
    virtual std::shared_ptr<Adapter> ensure_adapter(const uint8_t ch) override;

    bool write_digital(const uint8_t ch, const uint8_t index, const bool high);
    bool read_digital(const uint8_t ch, const uint8_t index, bool& high);
    bool write_analog(const uint8_t ch, const uint8_t index, const uint8_t val);
    bool write_pwm(const uint8_t ch, const uint8_t index, const uint8_t val);
    bool read_pwm(const uint8_t ch, const uint8_t index, uint8_t& val);
    bool write_servo_angle(const uint8_t ch, const uint8_t index, const uint8_t angle);
    bool read_servo_angle(const uint8_t ch, const uint8_t index, uint8_t& angle);
    bool write_servo_pulse(const uint8_t ch, const uint8_t index, const uint16_t angle);
    bool read_servo_pulse(const uint8_t ch, const uint8_t index, uint16_t& angle);

    inline bool is_firmware_2_or_later() const
    {
        return (_ver != 0xFF) && (_ver >= 2);
    }
    inline bool is_pbhub() const
    {
        return (_ver != 0xFF) && (_ver == 0);
    }
    inline bool is_pbhub_v11() const
    {
        return (_ver != 0xFF) && _ver;
    }

private:
    std::array<uint16_t, +MAX_CHANNEL> _numLED{74, 74, 74, 74, 74, 74};
    uint8_t _ver{0xFF};
};

namespace pbhub {
namespace command {
///@cond
constexpr uint8_t WRITE_DIGITAL_0_REG{0x00};
constexpr uint8_t WRITE_DIGITAL_1_REG{0x01};

constexpr uint8_t PWM_0_REG{0x02};  // v1.1
constexpr uint8_t PWM_1_REG{0x03};  // v1.1
constexpr uint8_t WRITE_ANALOG_0_REG{0x02};
constexpr uint8_t WRITE_ANALOG_1_REG{0x03};

constexpr uint8_t READ_DIGITAL_0_REG{0x04};
constexpr uint8_t READ_DIGITAL_1_REG{0x05};
constexpr uint8_t READ_ANALOG_0_REG{0x06};
// reserved
constexpr uint8_t LED_NUM_REG{0x08};
constexpr uint8_t LED_COLOR_SINGLE_REG{0x09};
constexpr uint8_t LED_COLOR_MORE_REG{0x0A};
constexpr uint8_t LED_BRIGHTNESS_REG{0x0B};

constexpr uint8_t SERVO_ANGLE_0_REG{0x0C};  // v1.1
constexpr uint8_t SERVO_ANGLE_1_REG{0x0D};  // v1.1
constexpr uint8_t SERVO_PULSE_0_REG{0x0E};  // v1.1
constexpr uint8_t SERVO_PULSE_1_REG{0x0F};  // v1.1

constexpr uint8_t LED_MODE_REG{0x0FA};         // v1.1 FW V2
constexpr uint8_t FIRMWARE_VERSION_REG{0xFE};  // v1.1
constexpr uint8_t I2C_ADDRESS_REG{0xFF};       // v1.1
///@endcond
}  // namespace command
}  // namespace pbhub

}  // namespace unit
}  // namespace m5
#endif
