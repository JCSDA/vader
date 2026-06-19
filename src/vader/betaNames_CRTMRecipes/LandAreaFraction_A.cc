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
#include "vader/betaNames_CRTMRecipes/LandAreaFraction.h"

namespace vader {

// Static attribute initialization
const char LandAreaFraction_A::Name[] = "LandAreaFraction_A";
const oops::Variables LandAreaFraction_A::Ingredients{
    std::vector<std::string>{"landmask", "surface_snow_thickness"}};

// Register the maker
static RecipeMaker<LandAreaFraction_A> makerLandAreaFraction_A_(
    LandAreaFraction_A::Name);

LandAreaFraction_A::LandAreaFraction_A(const Parameters_ & params,
                                          const VaderConfigVars & configVariables) {
  oops::Log::trace() << "LandAreaFraction_A::LandAreaFraction_A(params)"
                     << std::endl;
}

std::string LandAreaFraction_A::name() const {
  return LandAreaFraction_A::Name;
}

oops::Variable LandAreaFraction_A::product() const {
  return oops::Variable{"land_area_fraction"};
}

oops::Variables LandAreaFraction_A::ingredients() const {
  return LandAreaFraction_A::Ingredients;
}

size_t LandAreaFraction_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

oops::Variables LandAreaFraction_A::trajectoryVars() const {
  return oops::Variables(std::vector<std::string>{"surface_snow_thickness"});
}

atlas::FunctionSpace LandAreaFraction_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("landmask").functionspace();
}
// -------------------------------------------------------------------------------------------------

void LandAreaFraction_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "LandAreaFraction_A::executeNL starting" << std::endl;

  util::for_each_value(
      [](const double landmask,
         const double snow_thick,
         double& land_frac) {
          double snow_indicator = (snow_thick > 0.0) ? 1.0 : 0.0;
          land_frac = landmask * (1.0 - snow_indicator);
      },
      afieldset.field("landmask"),
      afieldset.field("surface_snow_thickness"),
      afieldset.field("land_area_fraction"));

  oops::Log::trace() << "LandAreaFraction_A::executeNL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void LandAreaFraction_A::executeTL(atlas::FieldSet & afieldsetTL,
                                     const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "LandAreaFraction_A::executeTL starting" << std::endl;

  util::for_each_value(
      [](const double snow_thick_traj,
         const double landmask_tl,
         double& land_frac_tl) {
          double snow_indicator = (snow_thick_traj > 0.0) ? 1.0 : 0.0;
          land_frac_tl = (1.0 - snow_indicator) * landmask_tl;
      },
      afieldsetTraj.field("surface_snow_thickness"),
      afieldsetTL.field("landmask"),
      afieldsetTL.field("land_area_fraction"));

  oops::Log::trace() << "LandAreaFraction_A::executeTL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void LandAreaFraction_A::executeAD(atlas::FieldSet & afieldsetAD,
                                     const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "LandAreaFraction_A::executeAD starting" << std::endl;

  util::for_each_value(
      [](const double snow_thick_traj,
         double& landmask_ad,
         double& land_frac_ad) {
          double snow_indicator = (snow_thick_traj > 0.0) ? 1.0 : 0.0;
          landmask_ad += (1.0 - snow_indicator) * land_frac_ad;
          land_frac_ad = 0.0;
      },
      afieldsetTraj.field("surface_snow_thickness"),
      afieldsetAD.field("landmask"),
      afieldsetAD.field("land_area_fraction"));

  oops::Log::trace() << "LandAreaFraction_A::executeAD done" << std::endl;
}

}  // namespace vader
