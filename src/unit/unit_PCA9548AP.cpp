/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_PCA9548AP.cpp
  @brief PCA9548AP Unit for M5UnitUnified
 */
#include "unit_PCA9548AP.hpp"
#include "m5_unit_component/adapter.hpp"
#include <M5Utility.hpp>

using namespace m5::utility::mmh3;
using namespace m5::unit::types;

namespace m5 {
namespace unit {

// class UnitPaHub
const char UnitPCA9548AP::name[] = "UnitPCA9548AP";
const types::uid_t UnitPCA9548AP::uid{"UnitPCA9548AP"_mmh3};
const types::attr_t UnitPCA9548AP::attr{attribute::AccessI2C};

UnitPCA9548AP::UnitPCA9548AP(const uint8_t addr) : Component(addr)
{
    auto ccfg         = component_config();
    ccfg.max_children = MAX_CHANNEL;
    ccfg.clock        = 400 * 1000U;
    component_config(ccfg);
}

bool UnitPCA9548AP::readChannel(uint8_t& bits)
{
    bits = 0;
    return readWithTransaction(&bits, 1) == m5::hal::error::error_t::OK;
}

std::shared_ptr<Adapter> UnitPCA9548AP::ensure_adapter(const uint8_t ch)
{
    if (ch >= MAX_CHANNEL) {
        M5_LIB_LOGE("Invalid channel %u", ch);
        return std::make_shared<Adapter>();  // Empty adapter
    }
    auto unit = child(ch);
    if (!unit) {
        M5_LIB_LOGE("Not exists unit %u", ch);
        return std::make_shared<Adapter>();  // Empty adapter
    }

    auto ad = asAdapter<AdapterI2C>(Adapter::Type::I2C);
    return ad ? std::shared_ptr<Adapter>(ad->duplicate(unit->address())) : std::make_shared<Adapter>();
}

m5::hal::error::error_t UnitPCA9548AP::select_channel(const uint8_t ch)
{
    // M5_LIB_LOGV("Try current to %u =>  %u", _current, ch);
    if (ch < MAX_CHANNEL) {
        m5::hal::error::error_t ret{m5::hal::error::error_t::OK};
        if (ch != _current) {
            uint8_t buf = (1U << ch);
            ret         = writeWithTransaction(&buf, 1);
            if (ret == m5::hal::error::error_t::OK) {
                _current = ch;
            }
        }
        return ret;
    }
    return m5::hal::error::error_t::INVALID_ARGUMENT;
}

}  // namespace unit
}  // namespace m5
