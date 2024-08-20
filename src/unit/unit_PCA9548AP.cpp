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

namespace m5 {
namespace unit {

// class UnitPaHub
const char UnitPCA9548AP::name[] = "UnitPCA9548AP";
const types::uid_t UnitPCA9548AP::uid{"UnitPCA9548AP"_mmh3};
const types::uid_t UnitPCA9548AP::attr{0};

UnitPCA9548AP::UnitPCA9548AP(const uint8_t addr) : Component(addr) {
    auto cfg         = component_config();
    cfg.max_children = MAX_CHANNEL;
    component_config(cfg);
}

bool UnitPCA9548AP::readChannel(uint8_t& bits) {
    bits = 0;
    return readWithTransaction(&bits, 1) == m5::hal::error::error_t::OK;
}

Adapter* UnitPCA9548AP::ensure_adapter(const uint8_t ch) {
    if (ch >= _adapters.size()) {
        M5_LIB_LOGE("Invalid channel %u", ch);
        return nullptr;
    }

    auto unit = child(ch);
    if (!unit) {
        M5_LIB_LOGE("Not exists unit %u", ch);
        return nullptr;
    }

    auto& ad = _adapters[ch];
    if (!ad) {
        ad.reset(_adapter->duplicate(unit->address()));
        // ad.reset(new PaHubAdapter(unit->address()));
    }
    return ad.get();
}

m5::hal::error::error_t UnitPCA9548AP::select_channel(const uint8_t ch) {
    // M5_LIB_LOGV("Try current to %u =>  %u", _current, ch);
    if (ch != _current && ch < MAX_CHANNEL) {
        _current       = 0;
        uint8_t buf[1] = {static_cast<uint8_t>((1U << ch) & 0xFF)};
        auto ret       = writeWithTransaction(buf, 1);
        if (ret == m5::hal::error::error_t::OK) {
            _current = ch;
        }
        return ret;
    }
    return m5::hal::error::error_t::UNKNOWN_ERROR;
}

}  // namespace unit
}  // namespace m5
