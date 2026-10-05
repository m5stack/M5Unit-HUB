/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for the child adapter of UnitPaHub (UnitPCA9548AP)
*/
#include <gtest/gtest.h>
#include <M5Unified.h>
#include <M5UnitUnified.hpp>
#include <googletest/test_template.hpp>
#include <unit/unit_PCA9548AP.hpp>
#include <array>

using namespace m5::unit::googletest;
using namespace m5::unit;
using namespace m5::utility::mmh3;

namespace {

// Address of the children. No device needs to answer it, the tests only see the channel selection
constexpr uint8_t CHILD_ADDRESS{0x3C};

// Child unit with I2C access, connected to a PaHub channel
class UnitPaHubChild : public Component {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitPaHubChild, 0x00);

public:
    UnitPaHubChild() : Component(CHILD_ADDRESS)
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

const char UnitPaHubChild::name[] = "UnitPaHubChild";
const types::uid_t UnitPaHubChild::uid{"UnitPaHubChild"_mmh3};
const types::attr_t UnitPaHubChild::attr{types::attribute::AccessI2C};

}  // namespace

class TestPCA9548APAdapter : public I2CComponentTestBase<UnitPCA9548AP> {
protected:
    virtual UnitPCA9548AP* get_instance() override
    {
        auto ptr = new m5::unit::UnitPCA9548AP();
        if (ptr) {
            for (uint8_t ch = 0; ch < UnitPCA9548AP::MAX_CHANNEL; ++ch) {
                if (!ptr->add(children[ch], ch)) {
                    delete ptr;
                    return nullptr;
                }
            }
        }
        return ptr;
    }

    std::array<UnitPaHubChild, UnitPCA9548AP::MAX_CHANNEL> children{};
};

// Each child adapter is a duplicate of the parent adapter with the address of the child
TEST_F(TestPCA9548APAdapter, ChildAdapter)
{
    SCOPED_TRACE(ustr);

    auto parent = static_cast<AdapterI2C*>(unit->adapter());
    EXPECT_NE(parent, nullptr);
    if (!parent) {
        return;
    }
    M5_LOGI("Parent impl type:%u", static_cast<uint8_t>(parent->implType()));

    for (uint8_t ch = 0; ch < UnitPCA9548AP::MAX_CHANNEL; ++ch) {
        auto s = m5::utility::formatString("CH:%u", ch);
        SCOPED_TRACE(s);

        auto& child = children[ch];
        EXPECT_EQ(child.channel(), ch);
        EXPECT_NE(child.adapter(), nullptr);
        if (!child.adapter()) {
            continue;
        }
        EXPECT_NE(child.adapter(), unit->adapter());
        EXPECT_EQ(child.adapter()->type(), Adapter::Type::I2C);
        if (child.adapter()->type() != Adapter::Type::I2C) {
            continue;
        }
        auto ad = static_cast<AdapterI2C*>(child.adapter());
        EXPECT_EQ(ad->implType(), parent->implType());
        EXPECT_EQ(ad->address(), CHILD_ADDRESS);
        EXPECT_EQ(ad->clock(), parent->clock());
    }
}

// An access of a child selects the channel of the child before the transfer
TEST_F(TestPCA9548APAdapter, ChannelSwitching)
{
    SCOPED_TRACE(ustr);

    // Descending order, so that every access changes the channel
    for (int_fast8_t ch = UnitPCA9548AP::MAX_CHANNEL - 1; ch >= 0; --ch) {
        auto s = m5::utility::formatString("CH:%d", ch);
        SCOPED_TRACE(s);

        // The result of the transfer does not matter (no device is at CHILD_ADDRESS)
        uint8_t buf[1]{};
        children[ch].readWithTransaction(buf, 1);
        EXPECT_EQ(unit->currentChannel(), ch);

        uint8_t bits{};
        EXPECT_TRUE(unit->readChannel(bits));
        EXPECT_EQ(bits, 1U << ch);
    }

    // Write access switches it too
    const uint8_t v{};
    children[2].writeWithTransaction(&v, 1);
    EXPECT_EQ(unit->currentChannel(), 2U);
    uint8_t bits{};
    EXPECT_TRUE(unit->readChannel(bits));
    EXPECT_EQ(bits, 1U << 2);
}

class TestPCA9548APInvalidChannel : public I2CComponentTestBase<UnitPCA9548AP> {
protected:
    virtual UnitPCA9548AP* get_instance() override
    {
        auto ptr = new m5::unit::UnitPCA9548AP();
        // Component::add() accepts it, but the PaHub has no such channel
        if (ptr && !ptr->add(child, UnitPCA9548AP::MAX_CHANNEL)) {
            delete ptr;
            return nullptr;
        }
        return ptr;
    }

    UnitPaHubChild child{};
};

// A child on a channel out of range gets an empty adapter
TEST_F(TestPCA9548APInvalidChannel, EmptyAdapter)
{
    SCOPED_TRACE(ustr);

    EXPECT_NE(child.adapter(), nullptr);
    if (!child.adapter()) {
        return;
    }
    EXPECT_EQ(child.adapter()->type(), Adapter::Type::Unknown);
}
