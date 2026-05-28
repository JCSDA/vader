/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/NorthwardWindAt10m.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char vwind_at_10m_A::Name[] = "vwind_at_10m_A";
const oops::Variables vwind_at_10m_A::Ingredients{std::vector<std::string>{"northward_wind"}};

// Register the maker
static RecipeMaker<vwind_at_10m_A> makeruwind_at_10m_A_(vwind_at_10m_A::Name);

vwind_at_10m_A::vwind_at_10m_A(const Parameters_ & params,
                               const VaderConfigVars & configVariables) :
    configVariables_(configVariables)
{
    oops::Log::trace() << "vwind_at_10m_A::vwind_at_10m_A(params)" << std::endl;
}

std::string vwind_at_10m_A::name() const
{
    return vwind_at_10m_A::Name;
}

oops::Variable vwind_at_10m_A::product() const
{
    return oops::Variable{"northward_wind_at_10m"};
}

oops::Variables vwind_at_10m_A::ingredients() const
{
    return vwind_at_10m_A::Ingredients;
}

oops::Variables vwind_at_10m_A::trajectoryVars() const
{
    return oops::Variables{};
}

size_t vwind_at_10m_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace vwind_at_10m_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["northward_wind"].functionspace();
}

// -------------------------------------------------------------------------------------------------

void vwind_at_10m_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering vwind_at_10m_A::executeNL function"
      << std::endl;

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const int surfLevel = topDown ? configVariables_.getInt("nLevels") - 1 : 0;

    util::for_each_column(
      [surfLevel](const auto northward_wind_col,
         auto northward_wind_at_10m_col) {
         northward_wind_at_10m_col(0) = northward_wind_col(surfLevel);
      },
      afieldset["northward_wind"],
      afieldset["northward_wind_at_10m"]);

    oops::Log::trace() << "leaving vwind_at_10m_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void vwind_at_10m_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering vwind_at_10m_A::executeTL function"
        << std::endl;

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const int surfLevel = topDown ? configVariables_.getInt("nLevels") - 1 : 0;

    util::for_each_column(
      [surfLevel](const auto tl_northward_wind_col,
         auto tl_northward_wind_at_10m_col) {
         tl_northward_wind_at_10m_col(0) = tl_northward_wind_col(surfLevel);
      },
      afieldsetTL["northward_wind"],
      afieldsetTL["northward_wind_at_10m"]);

    oops::Log::trace() << "leaving vwind_at_10m_A::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void vwind_at_10m_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering vwind_at_10m_A::executeAD function"
        << std::endl;

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const int surfLevel = topDown ? configVariables_.getInt("nLevels") - 1 : 0;

    util::for_each_column(
      [surfLevel](auto ad_northward_wind_at_10m_col,
         auto ad_northward_wind_col) {
         ad_northward_wind_col(surfLevel) += ad_northward_wind_at_10m_col(0);
         ad_northward_wind_at_10m_col(0) = 0.0;
      },
      afieldsetAD["northward_wind_at_10m"],
      afieldsetAD["northward_wind"]);

    oops::Log::trace() << "leaving vwind_at_10m_A::executeAD function" << std::endl;
}

}  // namespace vader
