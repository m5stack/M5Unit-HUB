/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_PCA9548AP.hpp
  @brief PCA9548AP Unit for M5UnitUnified
 */
#ifndef M5_UNIT_PAHUB_UNIT_PCA9548AP_HPP
#define M5_UNIT_PAHUB_UNIT_PCA9548AP_HPP

#include <M5UnitComponent.hpp>
#include <array>

namespace m5 {
namespace unit {

/*!
  @class m5::unit::UnitPCA9548AP
  @brief PCA9548AP unit
 */
class UnitPCA9548AP : public Component {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitPCA9548AP, 0x70);

public:
    constexpr static uint8_t MAX_CHANNEL = 6;

    explicit UnitPCA9548AP(const uint8_t addr = DEFAULT_ADDRESS);
    virtual ~UnitPCA9548AP() = default;

    /*!
      @brief Get current channel
      @return Channel no(0...)
    */
    uint8_t currentChannel() const
    {
        return _current;
    }

    /*!
      @brief Read channel status bits
      @param[out] bits Status bits
      @return True if successful
    */
    bool readChannel(uint8_t& bits);

protected:
    virtual m5::hal::error::error_t select_channel(const uint8_t ch) override;
    virtual std::shared_ptr<Adapter> ensure_adapter(const uint8_t ch) override;

protected:
    uint8_t _current{0xFF};  // current channel 0 ~ MAX_CHANNEL
};

}  // namespace unit
}  // namespace m5
#endif
