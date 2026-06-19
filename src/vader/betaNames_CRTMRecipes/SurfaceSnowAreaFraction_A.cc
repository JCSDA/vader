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
#include "vader/betaNames_CRTMRecipes/SurfaceSnowAreaFraction.h"

namespace vader {

// Static attribute initialization
const char SurfaceSnowAreaFraction_A::Name[] = "SurfaceSnowAreaFraction_A";
const oops::Variables SurfaceSnowAreaFraction_A::Ingredients{
    std::vector<std::string>{"landmask", "surface_snow_thickness"}};

// Register the maker
static RecipeMaker<SurfaceSnowAreaFraction_A> makerSurfaceSnowAreaFraction_A_(
    SurfaceSnowAreaFraction_A::Name);

SurfaceSnowAreaFraction_A::SurfaceSnowAreaFraction_A(const Parameters_ & params,
                                                         const VaderConfigVars & configVariables) {
  oops::Log::trace() << "SurfaceSnowAreaFraction_A::SurfaceSnowAreaFraction_A(params)"
                     << std::endl;
}

std::string SurfaceSnowAreaFraction_A::name() const {
  return SurfaceSnowAreaFraction_A::Name;
}

oops::Variable SurfaceSnowAreaFraction_A::product() const {
  return oops::Variable{"surface_snow_area_fraction"};
}

oops::Variables SurfaceSnowAreaFraction_A::ingredients() const {
  return SurfaceSnowAreaFraction_A::Ingredients;
}

size_t SurfaceSnowAreaFraction_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

oops::Variables SurfaceSnowAreaFraction_A::trajectoryVars() const {
  return oops::Variables(std::vector<std::string>{"surface_snow_thickness"});
}

atlas::FunctionSpace SurfaceSnowAreaFraction_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("landmask").functionspace();
}
// -------------------------------------------------------------------------------------------------

void SurfaceSnowAreaFraction_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SurfaceSnowAreaFraction_A::executeNL starting" << std::endl;

  util::for_each_value(
      [](const double landmask,
         const double snow_thick,
         double& snow_frac) {
          double snow_indicator = (snow_thick > 0.0) ? 1.0 : 0.0;
          snow_frac = landmask * snow_indicator;
      },
      afieldset.field("landmask"),
      afieldset.field("surface_snow_thickness"),
      afieldset.field("surface_snow_area_fraction"));

  oops::Log::trace() << "SurfaceSnowAreaFraction_A::executeNL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SurfaceSnowAreaFraction_A::executeTL(atlas::FieldSet & afieldsetTL,
                                            const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SurfaceSnowAreaFraction_A::executeTL starting" << std::endl;

  util::for_each_value(
      [](const double snow_thick_traj,
         const double landmask_tl,
         double& snow_frac_tl) {
          double snow_indicator = (snow_thick_traj > 0.0) ? 1.0 : 0.0;
          snow_frac_tl = snow_indicator * landmask_tl;
      },
      afieldsetTraj.field("surface_snow_thickness"),
      afieldsetTL.field("landmask"),
      afieldsetTL.field("surface_snow_area_fraction"));

  oops::Log::trace() << "SurfaceSnowAreaFraction_A::executeTL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SurfaceSnowAreaFraction_A::executeAD(atlas::FieldSet & afieldsetAD,
                                            const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SurfaceSnowAreaFraction_A::executeAD starting" << std::endl;

  util::for_each_value(
      [](const double snow_thick_traj,
         double& landmask_ad,
         double& snow_frac_ad) {
          double snow_indicator = (snow_thick_traj > 0.0) ? 1.0 : 0.0;
          landmask_ad += snow_indicator * snow_frac_ad;
          snow_frac_ad = 0.0;
      },
      afieldsetTraj.field("surface_snow_thickness"),
      afieldsetAD.field("landmask"),
      afieldsetAD.field("surface_snow_area_fraction"));

  oops::Log::trace() << "SurfaceSnowAreaFraction_A::executeAD done" << std::endl;
}

}  // namespace vader
