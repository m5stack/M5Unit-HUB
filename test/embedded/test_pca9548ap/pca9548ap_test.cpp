/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitPaHub
*/
#include <gtest/gtest.h>
#include <Wire.h>
#include <M5Unified.h>
#include <M5UnitUnified.hpp>
#include <googletest/test_template.hpp>
#include <unit/unit_PCA9548AP.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

using namespace m5::unit::googletest;
using namespace m5::unit;

const ::testing::Environment* global_fixture = ::testing::AddGlobalTestEnvironment(new GlobalFixture<400000U>());

class TestPCA9548AP : public ComponentTestBase<UnitPCA9548AP, bool> {
   protected:
    virtual UnitPCA9548AP* get_instance() override {
        auto ptr = new m5::unit::UnitPCA9548AP();
        return ptr;
    }
    virtual bool is_using_hal() const override {
        return GetParam();
    };
};

// INSTANTIATE_TEST_SUITE_P(ParamValues, TestPCA9548AP, ::testing::Values(false, true));
// INSTANTIATE_TEST_SUITE_P(ParamValues, TestPCA9548AP,::testing::Values(true));
INSTANTIATE_TEST_SUITE_P(ParamValues, TestPCA9548AP, ::testing::Values(false));

TEST_P(TestPCA9548AP, Basic) {
    SCOPED_TRACE(ustr);

    for (uint8_t ch = 0; ch < 6U; ++ch) {
        EXPECT_TRUE(unit->selectChannel(ch));
        EXPECT_EQ(unit->currentChannel(), ch);

        uint8_t bits{};
        EXPECT_TRUE(unit->readChannel(bits));
        // M5_LOGW("ch:%u bits:%0x", ch, bits);
        EXPECT_EQ(1U << ch, bits);
    }
}
