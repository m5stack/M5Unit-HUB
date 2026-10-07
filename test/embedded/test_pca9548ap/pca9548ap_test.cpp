/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitPaHub
*/
#include <gtest/gtest.h>
#include <M5Unified.h>
#include <M5UnitUnified.hpp>
#include <googletest/test_template.hpp>
#include <unit/unit_PCA9548AP.hpp>

using namespace m5::unit::googletest;
using namespace m5::unit;

class TestPCA9548AP : public I2CComponentTestBase<UnitPCA9548AP> {
protected:
    virtual UnitPCA9548AP* get_instance() override
    {
        auto ptr = new m5::unit::UnitPCA9548AP();
        return ptr;
    }
};

TEST_F(TestPCA9548AP, Basic)
{
    SCOPED_TRACE(ustr);

    // No channel is selected right after begin()
    EXPECT_EQ(unit->currentChannel(), 0xFF);

    for (uint8_t ch = 0; ch < UnitPCA9548AP::MAX_CHANNEL; ++ch) {
        EXPECT_TRUE(unit->selectChannel(ch));
        EXPECT_EQ(unit->currentChannel(), ch);

        uint8_t bits{};
        EXPECT_TRUE(unit->readChannel(bits));
        EXPECT_EQ(1U << ch, bits);
    }

    // Out of range does not change the selection
    const auto cur = unit->currentChannel();
    EXPECT_FALSE(unit->selectChannel(UnitPCA9548AP::MAX_CHANNEL));
    EXPECT_FALSE(unit->selectChannel(255));
    EXPECT_EQ(unit->currentChannel(), cur);
    uint8_t bits{};
    EXPECT_TRUE(unit->readChannel(bits));
    EXPECT_EQ(bits, 1U << cur);
}
