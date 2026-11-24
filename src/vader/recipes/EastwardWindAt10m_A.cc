/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/EastwardWindAt10m.h"

namespace vader
{

namespace {
// Index of the lowest model level.
int lowestLevel(const VaderConfigVars & configVariables) {
    const bool topDown = configVariables.getBool("levels_are_top_down");
    return topDown ? configVariables.getInt("nLevels") - 1 : 0;
}
const char ratioName[] = "ratio_of_wind_at_10m_to_wind_at_surface_adjacent_layer";
}  // namespace

// ------------------------------------------------------------------------------------------------
// EastwardWindAt10m_A (nonlinear)
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char EastwardWindAt10m_A::Name[] = "EastwardWindAt10m_A";
const oops::Variables EastwardWindAt10m_A::Ingredients{std::vector<std::string>{
    "eastward_wind", ratioName}};

// Register the maker
static RecipeMaker<EastwardWindAt10m_A> makerEastwardWindAt10m_A_(EastwardWindAt10m_A::Name);

EastwardWindAt10m_A::EastwardWindAt10m_A(const Parameters_ & params,
                               const VaderConfigVars & configVariables) :
    configVariables_(configVariables)
{
    oops::Log::trace() << "EastwardWindAt10m_A::EastwardWindAt10m_A(params)" << std::endl;
}

std::string EastwardWindAt10m_A::name() const
{
    return EastwardWindAt10m_A::Name;
}

oops::Variable EastwardWindAt10m_A::product() const
{
    return oops::Variable{"eastward_wind_at_10m"};
}

oops::Variables EastwardWindAt10m_A::ingredients() const
{
    return EastwardWindAt10m_A::Ingredients;
}

size_t EastwardWindAt10m_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace EastwardWindAt10m_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["eastward_wind"].functionspace();
}

void EastwardWindAt10m_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering EastwardWindAt10m_A::executeNL function"
      << std::endl;

    const int surfLevel = lowestLevel(configVariables_);

    util::for_each_column(
        [surfLevel](const auto eastward_wind_col,
           const auto ratio_col,
           auto eastward_wind_at_10m_col) {
           eastward_wind_at_10m_col(0) = ratio_col(0) * eastward_wind_col(surfLevel);
        },
        afieldset["eastward_wind"],
        afieldset[ratioName],
        afieldset["eastward_wind_at_10m"]);

    oops::Log::trace() << "leaving EastwardWindAt10m_A::executeNL function" << std::endl;
}

// ------------------------------------------------------------------------------------------------
// EastwardWindAt10m_B (TL/AD of EastwardWindAt10m_A)
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char EastwardWindAt10m_B::Name[] = "EastwardWindAt10m_B";
const oops::Variables EastwardWindAt10m_B::Ingredients{std::vector<std::string>{"eastward_wind"}};

// Register the maker
static RecipeMaker<EastwardWindAt10m_B> makerEastwardWindAt10m_B_(EastwardWindAt10m_B::Name);

EastwardWindAt10m_B::EastwardWindAt10m_B(const Parameters_ & params,
                               const VaderConfigVars & configVariables) :
    configVariables_(configVariables)
{
    oops::Log::trace() << "EastwardWindAt10m_B::EastwardWindAt10m_B(params)" << std::endl;
}

std::string EastwardWindAt10m_B::name() const
{
    return EastwardWindAt10m_B::Name;
}

oops::Variable EastwardWindAt10m_B::product() const
{
    return oops::Variable{"eastward_wind_at_10m"};
}

oops::Variables EastwardWindAt10m_B::ingredients() const
{
    return EastwardWindAt10m_B::Ingredients;
}

oops::Variables EastwardWindAt10m_B::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{ratioName}};
}

size_t EastwardWindAt10m_B::productLevels(const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace EastwardWindAt10m_B::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["eastward_wind"].functionspace();
}

void EastwardWindAt10m_B::executeTL(atlas::FieldSet & afieldsetTL,
                               const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering EastwardWindAt10m_B::executeTL function"
        << std::endl;

    const int surfLevel = lowestLevel(configVariables_);

    util::for_each_column(
        [surfLevel](const auto ratio_col,
           const auto tl_eastward_wind_col,
           auto tl_eastward_wind_at_10m_col) {
           tl_eastward_wind_at_10m_col(0) = ratio_col(0) * tl_eastward_wind_col(surfLevel);
        },
        afieldsetTraj[ratioName],
        afieldsetTL["eastward_wind"],
        afieldsetTL["eastward_wind_at_10m"]);

    oops::Log::trace() << "leaving EastwardWindAt10m_B::executeTL function" << std::endl;
}

void EastwardWindAt10m_B::executeAD(atlas::FieldSet & afieldsetAD,
                               const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering EastwardWindAt10m_B::executeAD function"
        << std::endl;

    const int surfLevel = lowestLevel(configVariables_);

    util::for_each_column(
        [surfLevel](const auto ratio_col,
           auto ad_eastward_wind_at_10m_col,
           auto ad_eastward_wind_col) {
           ad_eastward_wind_col(surfLevel) += ratio_col(0) * ad_eastward_wind_at_10m_col(0);
           ad_eastward_wind_at_10m_col(0) = 0.0;
        },
        afieldsetTraj[ratioName],
        afieldsetAD["eastward_wind_at_10m"],
        afieldsetAD["eastward_wind"]);

    oops::Log::trace() << "leaving EastwardWindAt10m_B::executeAD function" << std::endl;
}

}  // namespace vader
