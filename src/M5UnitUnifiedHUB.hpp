/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file M5UnitUnifiedHUB.hpp
  @brief Main header of M5UnitHUB

  @mainpage M5Unit-HUB
  Library for UnitHUB using M5UnitUnified.
*/
#ifndef M5_UNIT_UNIFIED_HUB_HPP
#define M5_UNIT_UNIFIED_HUB_HPP

#include "unit/unit_PCA9548AP.hpp"
#include "unit/unit_PbHub.hpp"

/*!
  @namespace m5
  @brief Top level namespace of M5Stack
 */
namespace m5 {

/*!
  @namespace unit
  @brief Unit-related namespace
 */
namespace unit {

using UnitPaHub  = m5::unit::UnitPCA9548AP;  //!< @brief Type alias for PaHub (TCA9548/PCA9548 compatible)
using UnitPaHub2 = m5::unit::UnitPCA9548AP;  //!< @brief Type alias for PaHub2 (v2.0 and v2.1)

}  // namespace unit
}  // namespace m5

#endif
