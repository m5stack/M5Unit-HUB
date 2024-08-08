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
#include <unit/unit_PaHub.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

using namespace m5::unit::googletest;
using namespace m5::unit;

const ::testing::Environment* global_fixture = ::testing::AddGlobalTestEnvironment(new GlobalFixture<400000U>());

class TestPaHub : public ComponentTestBase<UnitPaHub, bool> {
   protected:
    virtual UnitPaHub* get_instance() override {
        auto ptr = new m5::unit::UnitPaHub();
        return ptr;
    }
    virtual bool is_using_hal() const override {
        return GetParam();
    };
};

// INSTANTIATE_TEST_SUITE_P(ParamValues, TestPaHub,
//                         ::testing::Values(false, true));
// INSTANTIATE_TEST_SUITE_P(ParamValues, TestPaHub,
// ::testing::Values(true));
INSTANTIATE_TEST_SUITE_P(ParamValues, TestPaHub, ::testing::Values(false));

TEST_P(TestPaHub, Dummy) {
    SCOPED_TRACE(ustr);
}
