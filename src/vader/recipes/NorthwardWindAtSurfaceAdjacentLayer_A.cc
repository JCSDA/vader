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
#include "vader/recipes/NorthwardWindAtSurfaceAdjacentLayer.h"

namespace vader {

// Static attribute initialization
const char NorthwardWindAtSurfaceAdjacentLayer_A::Name[] =
    "NorthwardWindAtSurfaceAdjacentLayer_A";
const oops::Variables NorthwardWindAtSurfaceAdjacentLayer_A::Ingredients{
    std::vector<std::string>{"northward_wind"}};

// Register the maker
static RecipeMaker<NorthwardWindAtSurfaceAdjacentLayer_A>
    makerNorthwardWindAtSurfaceAdjacentLayer_A_(NorthwardWindAtSurfaceAdjacentLayer_A::Name);

NorthwardWindAtSurfaceAdjacentLayer_A::NorthwardWindAtSurfaceAdjacentLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) :
  configVariables_(configVariables) {
  oops::Log::trace() << "NorthwardWindAtSurfaceAdjacentLayer_A::"
                     << "NorthwardWindAtSurfaceAdjacentLayer_A(params)"
                     << std::endl;
}

std::string NorthwardWindAtSurfaceAdjacentLayer_A::name() const {
  return NorthwardWindAtSurfaceAdjacentLayer_A::Name;
}

oops::Variable NorthwardWindAtSurfaceAdjacentLayer_A::product() const {
  return oops::Variable{"northward_wind_at_surface_adjacent_layer"};
}

oops::Variables NorthwardWindAtSurfaceAdjacentLayer_A::ingredients() const {
  return NorthwardWindAtSurfaceAdjacentLayer_A::Ingredients;
}

oops::Variables NorthwardWindAtSurfaceAdjacentLayer_A::trajectoryVars() const {
  return oops::Variables{};
}

size_t NorthwardWindAtSurfaceAdjacentLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace NorthwardWindAtSurfaceAdjacentLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("northward_wind").functionspace();
}
// -------------------------------------------------------------------------------------------------

void NorthwardWindAtSurfaceAdjacentLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "NorthwardWindAtSurfaceAdjacentLayer_A::executeNL starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldset.field("northward_wind").shape(1) - 1 : 0;

  util::for_each_column(
      [surfLevel](const auto v_col,
         auto v_surf_col) {
          v_surf_col(0) = v_col(surfLevel);
      },
      afieldset.field("northward_wind"),
      afieldset.field("northward_wind_at_surface_adjacent_layer"));

  oops::Log::trace() << "NorthwardWindAtSurfaceAdjacentLayer_A::executeNL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void NorthwardWindAtSurfaceAdjacentLayer_A::executeTL(atlas::FieldSet & afieldsetTL,
                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "NorthwardWindAtSurfaceAdjacentLayer_A::executeTL starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldsetTL.field("northward_wind").shape(1) - 1 : 0;

  util::for_each_column(
      [surfLevel](const auto v_tl_col,
         auto v_surf_tl_col) {
          v_surf_tl_col(0) = v_tl_col(surfLevel);
      },
      afieldsetTL.field("northward_wind"),
      afieldsetTL.field("northward_wind_at_surface_adjacent_layer"));

  oops::Log::trace() << "NorthwardWindAtSurfaceAdjacentLayer_A::executeTL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void NorthwardWindAtSurfaceAdjacentLayer_A::executeAD(atlas::FieldSet & afieldsetAD,
                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "NorthwardWindAtSurfaceAdjacentLayer_A::executeAD starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldsetAD.field("northward_wind").shape(1) - 1 : 0;

  util::for_each_column(
      [surfLevel](auto v_ad_col,
         auto v_surf_ad_col) {
          v_ad_col(surfLevel) += v_surf_ad_col(0);
          v_surf_ad_col(0) = 0.0;
      },
      afieldsetAD.field("northward_wind"),
      afieldsetAD.field("northward_wind_at_surface_adjacent_layer"));

  oops::Log::trace() << "NorthwardWindAtSurfaceAdjacentLayer_A::executeAD done" << std::endl;
}

}  // namespace vader
