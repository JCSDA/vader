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
#include "vader/recipes/EastwardWindAtSurfaceAdjacentLayer.h"

namespace vader {

// Static attribute initialization
const char EastwardWindAtSurfaceAdjacentLayer_A::Name[] =
    "EastwardWindAtSurfaceAdjacentLayer_A";
const oops::Variables EastwardWindAtSurfaceAdjacentLayer_A::Ingredients{
    std::vector<std::string>{"eastward_wind"}};

// Register the maker
static RecipeMaker<EastwardWindAtSurfaceAdjacentLayer_A>
    makerEastwardWindAtSurfaceAdjacentLayer_A_(EastwardWindAtSurfaceAdjacentLayer_A::Name);

EastwardWindAtSurfaceAdjacentLayer_A::EastwardWindAtSurfaceAdjacentLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) :
  configVariables_(configVariables) {
  oops::Log::trace() << "EastwardWindAtSurfaceAdjacentLayer_A::"
                     << "EastwardWindAtSurfaceAdjacentLayer_A(params)"
                     << std::endl;
}

std::string EastwardWindAtSurfaceAdjacentLayer_A::name() const {
  return EastwardWindAtSurfaceAdjacentLayer_A::Name;
}

oops::Variable EastwardWindAtSurfaceAdjacentLayer_A::product() const {
  return oops::Variable{"eastward_wind_at_surface_adjacent_layer"};
}

oops::Variables EastwardWindAtSurfaceAdjacentLayer_A::ingredients() const {
  return EastwardWindAtSurfaceAdjacentLayer_A::Ingredients;
}

oops::Variables EastwardWindAtSurfaceAdjacentLayer_A::trajectoryVars() const {
  return oops::Variables{};
}

size_t EastwardWindAtSurfaceAdjacentLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace EastwardWindAtSurfaceAdjacentLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("eastward_wind").functionspace();
}
// -------------------------------------------------------------------------------------------------

void EastwardWindAtSurfaceAdjacentLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "EastwardWindAtSurfaceAdjacentLayer_A::executeNL starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldset.field("eastward_wind").shape(1) - 1 : 0;

  util::for_each_column(
      [surfLevel](const auto u_col,
         auto u_surf_col) {
          u_surf_col(0) = u_col(surfLevel);
      },
      afieldset.field("eastward_wind"),
      afieldset.field("eastward_wind_at_surface_adjacent_layer"));

  oops::Log::trace() << "EastwardWindAtSurfaceAdjacentLayer_A::executeNL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void EastwardWindAtSurfaceAdjacentLayer_A::executeTL(atlas::FieldSet & afieldsetTL,
                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EastwardWindAtSurfaceAdjacentLayer_A::executeTL starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldsetTL.field("eastward_wind").shape(1) - 1 : 0;

  util::for_each_column(
      [surfLevel](const auto u_tl_col,
         auto u_surf_tl_col) {
          u_surf_tl_col(0) = u_tl_col(surfLevel);
      },
      afieldsetTL.field("eastward_wind"),
      afieldsetTL.field("eastward_wind_at_surface_adjacent_layer"));

  oops::Log::trace() << "EastwardWindAtSurfaceAdjacentLayer_A::executeTL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void EastwardWindAtSurfaceAdjacentLayer_A::executeAD(atlas::FieldSet & afieldsetAD,
                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EastwardWindAtSurfaceAdjacentLayer_A::executeAD starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldsetAD.field("eastward_wind").shape(1) - 1 : 0;

  util::for_each_column(
      [surfLevel](auto u_ad_col,
         auto u_surf_ad_col) {
          u_ad_col(surfLevel) += u_surf_ad_col(0);
          u_surf_ad_col(0) = 0.0;
      },
      afieldsetAD.field("eastward_wind"),
      afieldsetAD.field("eastward_wind_at_surface_adjacent_layer"));

  oops::Log::trace() << "EastwardWindAtSurfaceAdjacentLayer_A::executeAD done" << std::endl;
}

}  // namespace vader
