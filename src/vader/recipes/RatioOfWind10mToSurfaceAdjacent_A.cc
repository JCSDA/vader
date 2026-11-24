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
#include "vader/recipes/RatioOfWind10mToSurfaceAdjacent.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char RatioOfWind10mToSurfaceAdjacent_A::Name[] = "RatioOfWind10mToSurfaceAdjacent_A";
const oops::Variables RatioOfWind10mToSurfaceAdjacent_A::Ingredients{
      std::vector<std::string>{"eastward_wind_at_10m",
                               "northward_wind_at_10m",
                               "eastward_wind",
                               "northward_wind"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<RatioOfWind10mToSurfaceAdjacent_A>
       makerRatioOfWind10mToSurfaceAdjacent_A_(RatioOfWind10mToSurfaceAdjacent_A::Name);

// -------------------------------------------------------------------------------------------------

RatioOfWind10mToSurfaceAdjacent_A::RatioOfWind10mToSurfaceAdjacent_A(
                            const RatioOfWind10mToSurfaceAdjacent_AParameters & params,
                            const VaderConfigVars & configVariables):
                            configVariables_{configVariables}
{
    oops::Log::trace() << "RatioOfWind10mToSurfaceAdjacent_A::RatioOfWind10mToSurfaceAdjacent_A"
                       << " Starting" << std::endl;
    oops::Log::trace() << "RatioOfWind10mToSurfaceAdjacent_A::RatioOfWind10mToSurfaceAdjacent_A"
                       << " Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string RatioOfWind10mToSurfaceAdjacent_A::name() const {
    return RatioOfWind10mToSurfaceAdjacent_A::Name;
}

// -------------------------------------------------------------------------------------------------

    // variable name used in yaml file
oops::Variable RatioOfWind10mToSurfaceAdjacent_A::product() const {
    return oops::Variable("ratio_of_wind_at_10m_to_wind_at_surface_adjacent_layer");
}

// -------------------------------------------------------------------------------------------------

oops::Variables RatioOfWind10mToSurfaceAdjacent_A::ingredients() const {
    return RatioOfWind10mToSurfaceAdjacent_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t RatioOfWind10mToSurfaceAdjacent_A::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("eastward_wind_at_10m").shape(1);
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace RatioOfWind10mToSurfaceAdjacent_A::productFunctionSpace(const atlas::FieldSet
& afieldset) const {
    return afieldset.field("eastward_wind_at_10m").functionspace();
}

// -------------------------------------------------------------------------------------------------

void RatioOfWind10mToSurfaceAdjacent_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "RatioOfWind10mToSurfaceAdjacent_A::executeNL Starting" << std::endl;

    // output:
    atlas::Field f10m = afieldset.field("ratio_of_wind_at_10m_to_wind_at_surface_adjacent_layer");
    f10m.metadata().set("units", "none");

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const int surfLevel = topDown ? configVariables_.getInt("nLevels") - 1 : 0;

    util::for_each_column(
      [&](const auto uu_10m_col,
          const auto vv_10m_col,
          const auto uu_col,
          const auto vv_col,
          auto f10m_col) {
        const double speed10m = std::sqrt(std::pow(uu_10m_col(0), 2)
                                          + std::pow(vv_10m_col(0), 2));
        const double speedLowest = std::sqrt(std::pow(uu_col(surfLevel), 2)
                                             + std::pow(vv_col(surfLevel), 2));
        // A calm 10 m wind gives a ratio of 0. The ratio is undefined when the
        // lowest-level wind is calm; fall back to 1.
        if (speedLowest > 0) {
            f10m_col(0) = speed10m / speedLowest;
        } else {
            f10m_col(0) = 1.0;
        }
      },
      afieldset["eastward_wind_at_10m"],
      afieldset["northward_wind_at_10m"],
      afieldset["eastward_wind"],
      afieldset["northward_wind"],
      f10m);

    oops::Log::trace() << "RatioOfWind10mToSurfaceAdjacent_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
