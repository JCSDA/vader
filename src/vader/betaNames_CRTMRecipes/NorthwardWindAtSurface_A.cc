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
#include "vader/betaNames_CRTMRecipes/NorthwardWindAtSurface.h"

namespace vader {

// Static attribute initialization
const char vwind_at_surface_A::Name[] = "vwind_at_surface_A";
const oops::Variables vwind_at_surface_A::Ingredients{
    std::vector<std::string>{"northward_wind"}};

// Register the maker
static RecipeMaker<vwind_at_surface_A> makervwind_at_surface_A_(
    vwind_at_surface_A::Name);

vwind_at_surface_A::vwind_at_surface_A(const Parameters_ & params,
                                       const VaderConfigVars & configVariables) :
  configVariables_(configVariables) {
  oops::Log::trace() << "vwind_at_surface_A::vwind_at_surface_A(params)"
                     << std::endl;
}

std::string vwind_at_surface_A::name() const {
  return vwind_at_surface_A::Name;
}

oops::Variable vwind_at_surface_A::product() const {
  return oops::Variable{"northward_wind_at_surface"};
}

oops::Variables vwind_at_surface_A::ingredients() const {
  return vwind_at_surface_A::Ingredients;
}

oops::Variables vwind_at_surface_A::trajectoryVars() const {
  return oops::Variables{};
}

size_t vwind_at_surface_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace vwind_at_surface_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("northward_wind").functionspace();
}
// -------------------------------------------------------------------------------------------------

void vwind_at_surface_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "vwind_at_surface_A::executeNL starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldset.field("northward_wind").shape(1) - 1 : 0;

  util::for_each_column(
      [surfLevel](const auto v_col,
         auto v_surf_col) {
          v_surf_col(0) = v_col(surfLevel);
      },
      afieldset.field("northward_wind"),
      afieldset.field("northward_wind_at_surface"));

  oops::Log::trace() << "vwind_at_surface_A::executeNL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void vwind_at_surface_A::executeTL(atlas::FieldSet & afieldsetTL,
                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "vwind_at_surface_A::executeTL starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldsetTL.field("northward_wind").shape(1) - 1 : 0;

  util::for_each_column(
      [surfLevel](const auto v_tl_col,
         auto v_surf_tl_col) {
          v_surf_tl_col(0) = v_tl_col(surfLevel);
      },
      afieldsetTL.field("northward_wind"),
      afieldsetTL.field("northward_wind_at_surface"));

  oops::Log::trace() << "vwind_at_surface_A::executeTL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void vwind_at_surface_A::executeAD(atlas::FieldSet & afieldsetAD,
                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "vwind_at_surface_A::executeAD starting" << std::endl;

  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldsetAD.field("northward_wind").shape(1) - 1 : 0;

  util::for_each_column(
      [surfLevel](auto v_ad_col,
         auto v_surf_ad_col) {
          v_ad_col(surfLevel) += v_surf_ad_col(0);
          v_surf_ad_col(0) = 0.0;
      },
      afieldsetAD.field("northward_wind"),
      afieldsetAD.field("northward_wind_at_surface"));

  oops::Log::trace() << "vwind_at_surface_A::executeAD done" << std::endl;
}

}  // namespace vader
