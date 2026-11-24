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
#include "vader/recipes/NorthwardWindAt10m.h"

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
// NorthwardWindAt10m_A (nonlinear)
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char NorthwardWindAt10m_A::Name[] = "NorthwardWindAt10m_A";
const oops::Variables NorthwardWindAt10m_A::Ingredients{std::vector<std::string>{
    "northward_wind", ratioName}};

// Register the maker
static RecipeMaker<NorthwardWindAt10m_A> makerNorthwardWindAt10m_A_(NorthwardWindAt10m_A::Name);

NorthwardWindAt10m_A::NorthwardWindAt10m_A(const Parameters_ & params,
                               const VaderConfigVars & configVariables) :
    configVariables_(configVariables)
{
    oops::Log::trace() << "NorthwardWindAt10m_A::NorthwardWindAt10m_A(params)" << std::endl;
}

std::string NorthwardWindAt10m_A::name() const
{
    return NorthwardWindAt10m_A::Name;
}

oops::Variable NorthwardWindAt10m_A::product() const
{
    return oops::Variable{"northward_wind_at_10m"};
}

oops::Variables NorthwardWindAt10m_A::ingredients() const
{
    return NorthwardWindAt10m_A::Ingredients;
}

size_t NorthwardWindAt10m_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace NorthwardWindAt10m_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["northward_wind"].functionspace();
}

void NorthwardWindAt10m_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering NorthwardWindAt10m_A::executeNL function"
      << std::endl;

    const int surfLevel = lowestLevel(configVariables_);

    util::for_each_column(
        [surfLevel](const auto northward_wind_col,
           const auto ratio_col,
           auto northward_wind_at_10m_col) {
           northward_wind_at_10m_col(0) = ratio_col(0) * northward_wind_col(surfLevel);
        },
        afieldset["northward_wind"],
        afieldset[ratioName],
        afieldset["northward_wind_at_10m"]);

    oops::Log::trace() << "leaving NorthwardWindAt10m_A::executeNL function" << std::endl;
}

// ------------------------------------------------------------------------------------------------
// NorthwardWindAt10m_B (TL/AD of NorthwardWindAt10m_A)
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char NorthwardWindAt10m_B::Name[] = "NorthwardWindAt10m_B";
const oops::Variables NorthwardWindAt10m_B::Ingredients{std::vector<std::string>{"northward_wind"}};

// Register the maker
static RecipeMaker<NorthwardWindAt10m_B> makerNorthwardWindAt10m_B_(NorthwardWindAt10m_B::Name);

NorthwardWindAt10m_B::NorthwardWindAt10m_B(const Parameters_ & params,
                               const VaderConfigVars & configVariables) :
    configVariables_(configVariables)
{
    oops::Log::trace() << "NorthwardWindAt10m_B::NorthwardWindAt10m_B(params)" << std::endl;
}

std::string NorthwardWindAt10m_B::name() const
{
    return NorthwardWindAt10m_B::Name;
}

oops::Variable NorthwardWindAt10m_B::product() const
{
    return oops::Variable{"northward_wind_at_10m"};
}

oops::Variables NorthwardWindAt10m_B::ingredients() const
{
    return NorthwardWindAt10m_B::Ingredients;
}

oops::Variables NorthwardWindAt10m_B::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{ratioName}};
}

size_t NorthwardWindAt10m_B::productLevels(const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace NorthwardWindAt10m_B::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["northward_wind"].functionspace();
}

void NorthwardWindAt10m_B::executeTL(atlas::FieldSet & afieldsetTL,
                               const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering NorthwardWindAt10m_B::executeTL function"
        << std::endl;

    const int surfLevel = lowestLevel(configVariables_);

    util::for_each_column(
        [surfLevel](const auto ratio_col,
           const auto tl_northward_wind_col,
           auto tl_northward_wind_at_10m_col) {
           tl_northward_wind_at_10m_col(0) = ratio_col(0) * tl_northward_wind_col(surfLevel);
        },
        afieldsetTraj[ratioName],
        afieldsetTL["northward_wind"],
        afieldsetTL["northward_wind_at_10m"]);

    oops::Log::trace() << "leaving NorthwardWindAt10m_B::executeTL function" << std::endl;
}

void NorthwardWindAt10m_B::executeAD(atlas::FieldSet & afieldsetAD,
                               const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering NorthwardWindAt10m_B::executeAD function"
        << std::endl;

    const int surfLevel = lowestLevel(configVariables_);

    util::for_each_column(
        [surfLevel](const auto ratio_col,
           auto ad_northward_wind_at_10m_col,
           auto ad_northward_wind_col) {
           ad_northward_wind_col(surfLevel) += ratio_col(0) * ad_northward_wind_at_10m_col(0);
           ad_northward_wind_at_10m_col(0) = 0.0;
        },
        afieldsetTraj[ratioName],
        afieldsetAD["northward_wind_at_10m"],
        afieldsetAD["northward_wind"]);

    oops::Log::trace() << "leaving NorthwardWindAt10m_B::executeAD function" << std::endl;
}

}  // namespace vader
