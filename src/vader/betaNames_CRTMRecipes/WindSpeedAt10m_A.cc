/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/WindSpeedAt10m.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WindSpeedAt10m_A::Name[] = "WindSpeedAt10m_A";
const oops::Variables WindSpeedAt10m_A::Ingredients{std::vector<std::string>{
                            "eastward_wind", "northward_wind",
                            "ratio_of_wind_at_surface_adjacent_layer_to_wind_at_10m"}};

// Register the maker
static RecipeMaker<WindSpeedAt10m_A> makerWindSpeedAt10m_A_(
                                WindSpeedAt10m_A::Name);

WindSpeedAt10m_A::WindSpeedAt10m_A(const Parameters_ & params,
                   const VaderConfigVars & configVariables)
                   : configVariables_(configVariables) {
  oops::Log::trace() << "WindSpeedAt10m_A::WindSpeedAt10m_A(params)"
                     << std::endl;
}

std::string WindSpeedAt10m_A::name() const {
  return WindSpeedAt10m_A::Name;
}

oops::Variable WindSpeedAt10m_A::product() const {
  return oops::Variable{"wind_speed_at_10m"};
}

oops::Variables WindSpeedAt10m_A::ingredients() const {
  return WindSpeedAt10m_A::Ingredients;
}

size_t WindSpeedAt10m_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace WindSpeedAt10m_A::productFunctionSpace(const atlas::FieldSet &
                                          afieldset) const {
  return afieldset.field("eastward_wind").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAt10m_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "WindSpeedAt10m_A::executeNL starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldset.field("eastward_wind").shape(1) - 1 : 0;

  util::for_each_column(
    [surfLevel](const auto u_col,
                const auto v_col,
                const auto ratio_col,
                auto speed_col) {
       speed_col(0) = ratio_col(0) *
                     std::sqrt(u_col(surfLevel) * u_col(surfLevel) +
                                v_col(surfLevel) * v_col(surfLevel));
    },
    afieldset["eastward_wind"],
    afieldset["northward_wind"],
    afieldset["ratio_of_wind_at_surface_adjacent_layer_to_wind_at_10m"],
    afieldset["wind_speed_at_10m"]);

  oops::Log::trace() << "WindSpeedAt10m_A::executeNL done" << std::endl;
}

}  // namespace vader
