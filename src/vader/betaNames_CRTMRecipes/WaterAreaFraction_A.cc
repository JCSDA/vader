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
#include "vader/betaNames_CRTMRecipes/WaterAreaFraction.h"

namespace vader {

// Static attribute initialization
const char WaterAreaFraction_A::Name[] = "WaterAreaFraction_A";
const oops::Variables WaterAreaFraction_A::Ingredients{
    std::vector<std::string>{"landmask", "seaice_fraction"}};

// Register the maker
static RecipeMaker<WaterAreaFraction_A> makerWaterAreaFraction_A_(
    WaterAreaFraction_A::Name);

WaterAreaFraction_A::WaterAreaFraction_A(const Parameters_ & params,
                                           const VaderConfigVars & configVariables) {
  oops::Log::trace() << "WaterAreaFraction_A::WaterAreaFraction_A(params)"
                     << std::endl;
}

std::string WaterAreaFraction_A::name() const {
  return WaterAreaFraction_A::Name;
}

oops::Variable WaterAreaFraction_A::product() const {
  return oops::Variable{"water_area_fraction"};
}

oops::Variables WaterAreaFraction_A::ingredients() const {
  return WaterAreaFraction_A::Ingredients;
}

size_t WaterAreaFraction_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

oops::Variables WaterAreaFraction_A::trajectoryVars() const {
  return oops::Variables(std::vector<std::string>{"landmask", "seaice_fraction"});
}

atlas::FunctionSpace WaterAreaFraction_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("landmask").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WaterAreaFraction_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "WaterAreaFraction_A::executeNL starting" << std::endl;

  util::for_each_value(
    [](const double landmask,
       const double seaice,
       double& water_frac) {
       water_frac = (1.0 - landmask) * (1.0 - seaice);
    },
    afieldset["landmask"],
    afieldset["seaice_fraction"],
    afieldset["water_area_fraction"]);

  oops::Log::trace() << "WaterAreaFraction_A::executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WaterAreaFraction_A::executeTL(atlas::FieldSet & afieldsetTL,
                                     const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WaterAreaFraction_A::executeTL starting" << std::endl;

  util::for_each_value(
    [](const double landmask,
       const double seaice,
       const double landmask_tl,
       const double seaice_tl,
       double& water_frac_tl) {
       water_frac_tl = -(1.0 - seaice) * landmask_tl - (1.0 - landmask) * seaice_tl;
    },
    afieldsetTraj["landmask"],
    afieldsetTraj["seaice_fraction"],
    afieldsetTL["landmask"],
    afieldsetTL["seaice_fraction"],
    afieldsetTL["water_area_fraction"]);

  oops::Log::trace() << "WaterAreaFraction_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WaterAreaFraction_A::executeAD(atlas::FieldSet & afieldsetAD,
                                     const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WaterAreaFraction_A::executeAD starting" << std::endl;

  util::for_each_value(
    [](const double landmask,
       const double seaice,
       double& landmask_ad,
       double& seaice_ad,
       double& water_frac_ad) {
       landmask_ad += -(1.0 - seaice) * water_frac_ad;
       seaice_ad += -(1.0 - landmask) * water_frac_ad;
       water_frac_ad = 0.0;
    },
    afieldsetTraj["landmask"],
    afieldsetTraj["seaice_fraction"],
    afieldsetAD["landmask"],
    afieldsetAD["seaice_fraction"],
    afieldsetAD["water_area_fraction"]);

  oops::Log::trace() << "WaterAreaFraction_A::executeAD done" << std::endl;
}

}  // namespace vader
