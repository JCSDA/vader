/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/EastwardWindAt10m.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char uwind_at_10m_A::Name[] = "uwind_at_10m_A";
const oops::Variables uwind_at_10m_A::Ingredients{std::vector<std::string>{"eastward_wind"}};

// Register the maker
static RecipeMaker<uwind_at_10m_A> makeruwind_at_10m_A_(uwind_at_10m_A::Name);

uwind_at_10m_A::uwind_at_10m_A(const Parameters_ & params,
                               const VaderConfigVars & configVariables) :
    configVariables_(configVariables)
{
    oops::Log::trace() << "uwind_at_10m_A::uwind_at_10m_A(params)" << std::endl;
}

std::string uwind_at_10m_A::name() const
{
    return uwind_at_10m_A::Name;
}

oops::Variable uwind_at_10m_A::product() const
{
    return oops::Variable{"eastward_wind_at_10m"};
}

oops::Variables uwind_at_10m_A::ingredients() const
{
    return uwind_at_10m_A::Ingredients;
}

oops::Variables uwind_at_10m_A::trajectoryVars() const
{
    return oops::Variables{};
}

size_t uwind_at_10m_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace uwind_at_10m_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["eastward_wind"].functionspace();
}

// -------------------------------------------------------------------------------------------------

void uwind_at_10m_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering uwind_at_10m_A::executeNL function"
      << std::endl;

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const int surfLevel = topDown ? configVariables_.getInt("nLevels") - 1 : 0;

    util::for_each_column(
        [surfLevel](const auto eastward_wind_col,
           auto uwind_at_10m_A_col) {
           uwind_at_10m_A_col(0) = eastward_wind_col(surfLevel);
        },
        afieldset["eastward_wind"],
        afieldset["eastward_wind_at_10m"]);

    oops::Log::trace() << "leaving uwind_at_10m_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void uwind_at_10m_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering uwind_at_10m_A::executeTL function"
        << std::endl;

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const int surfLevel = topDown ? configVariables_.getInt("nLevels") - 1 : 0;

    util::for_each_column(
        [surfLevel](const auto tl_eastward_wind_col,
           auto tl_uwind_at_10m_A_col) {
           tl_uwind_at_10m_A_col(0) = tl_eastward_wind_col(surfLevel);
        },
        afieldsetTL["eastward_wind"],
        afieldsetTL["eastward_wind_at_10m"]);

    oops::Log::trace() << "leaving uwind_at_10m_A::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void uwind_at_10m_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering uwind_at_10m_A::executeAD function"
        << std::endl;

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const int surfLevel = topDown ? configVariables_.getInt("nLevels") - 1 : 0;

    util::for_each_column(
        [surfLevel](auto ad_uwind_at_10m_A_col,
           auto ad_eastward_wind_col) {
           ad_eastward_wind_col(surfLevel) += ad_uwind_at_10m_A_col(0);
           ad_uwind_at_10m_A_col(0) = 0.0;
        },
        afieldsetAD["eastward_wind_at_10m"],
        afieldsetAD["eastward_wind"]);

    oops::Log::trace() << "leaving uwind_at_10m_A::executeAD function" << std::endl;
}

}  // namespace vader
