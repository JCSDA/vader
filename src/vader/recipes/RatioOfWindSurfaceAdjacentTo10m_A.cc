/*
 * (C) Copyright 2024  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/RatioOfWindSurfaceAdjacentTo10m.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char RatioOfWindSurfaceAdjacentTo10m_A::Name[] = "RatioOfWindSurfaceAdjacentTo10m_A";
const oops::Variables RatioOfWindSurfaceAdjacentTo10m_A::Ingredients{
      std::vector<std::string>{"eastward_wind_at_10m",
                               "northward_wind_at_10m",
                               "eastward_wind",
                               "northward_wind"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<RatioOfWindSurfaceAdjacentTo10m_A>
       makerRatioOfWindSurfaceAdjacentTo10m_A_(RatioOfWindSurfaceAdjacentTo10m_A::Name);

// -------------------------------------------------------------------------------------------------

RatioOfWindSurfaceAdjacentTo10m_A::RatioOfWindSurfaceAdjacentTo10m_A(
                            const RatioOfWindSurfaceAdjacentTo10m_AParameters & params,
                            const VaderConfigVars & configVariables):
                            configVariables_{configVariables}
{
    oops::Log::trace() << "RatioOfWindSurfaceAdjacentTo10m_A::RatioOfWindSurfaceAdjacentTo10m_A"
                       << " Starting" << std::endl;
    oops::Log::trace() << "RatioOfWindSurfaceAdjacentTo10m_A::RatioOfWindSurfaceAdjacentTo10m_A"
                       << " Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string RatioOfWindSurfaceAdjacentTo10m_A::name() const {
    return RatioOfWindSurfaceAdjacentTo10m_A::Name;
}

// -------------------------------------------------------------------------------------------------

    // variable name used in yaml file
oops::Variable RatioOfWindSurfaceAdjacentTo10m_A::product() const {
    return oops::Variable("ratio_of_wind_at_surface_adjacent_layer_to_wind_at_10m");
}

// -------------------------------------------------------------------------------------------------

oops::Variables RatioOfWindSurfaceAdjacentTo10m_A::ingredients() const {
    return RatioOfWindSurfaceAdjacentTo10m_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t RatioOfWindSurfaceAdjacentTo10m_A::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("eastward_wind_at_10m").shape(1);
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace RatioOfWindSurfaceAdjacentTo10m_A::productFunctionSpace(const atlas::FieldSet
& afieldset) const {
    return afieldset.field("eastward_wind_at_10m").functionspace();
}

// -------------------------------------------------------------------------------------------------

void RatioOfWindSurfaceAdjacentTo10m_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "RatioOfWindSurfaceAdjacentTo10m_A::executeNL Starting" << std::endl;

    // output:
    atlas::Field f10m = afieldset.field("ratio_of_wind_at_surface_adjacent_layer_to_wind_at_10m");
    f10m.metadata().set("units", "none");

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const int surfLevel = topDown ? configVariables_.getInt("nLevels") - 1 : 0;

    util::for_each_column(
      [&](const auto uu_10m_col,
          const auto vv_10m_col,
          const auto uu_col,
          const auto vv_col,
          auto f10m_col) {
        f10m_col(0) = std::sqrt(std::pow(uu_10m_col(0), 2) + std::pow(vv_10m_col(0), 2));
        if (f10m_col(0) > 0) {
            f10m_col(0) = f10m_col(0) / std::sqrt(std::pow(uu_col(surfLevel), 2)
                                      + std::pow(vv_col(surfLevel), 2));
        } else {
            f10m_col(0) = 1.0;
        }
      },
      afieldset["eastward_wind_at_10m"],
      afieldset["northward_wind_at_10m"],
      afieldset["eastward_wind"],
      afieldset["northward_wind"],
      f10m);

    oops::Log::trace() << "RatioOfWindSurfaceAdjacentTo10m_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
