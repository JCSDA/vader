/*
 * (C) Copyright 2025 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/MassContentOfHailInAtmosphereLayer.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char MassContentOfHailInAtmosphereLayer_A::Name[] =
    "MassContentOfHailInAtmosphereLayer_A";
const oops::Variables MassContentOfHailInAtmosphereLayer_A::Ingredients{
    std::vector<std::string>{
      "hail", "air_pressure_thickness"}};

// Register the maker
static RecipeMaker<MassContentOfHailInAtmosphereLayer_A>
  makerMassContentOfHailInAtmosphereLayer_A_(
      MassContentOfHailInAtmosphereLayer_A::Name);

// -------------------------------------------------------------------------------------------------

MassContentOfHailInAtmosphereLayer_A::MassContentOfHailInAtmosphereLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "MassContentOfHailInAtmosphereLayer_A::"
                     << "MassContentOfHailInAtmosphereLayer_A" << std::endl;
}

std::string MassContentOfHailInAtmosphereLayer_A::name() const {
  return MassContentOfHailInAtmosphereLayer_A::Name;
}

oops::Variable MassContentOfHailInAtmosphereLayer_A::product() const {
  return oops::Variable{"mass_content_of_hail_in_atmosphere_layer"};
}

oops::Variables MassContentOfHailInAtmosphereLayer_A::ingredients() const {
  return MassContentOfHailInAtmosphereLayer_A::Ingredients;
}

oops::Variables MassContentOfHailInAtmosphereLayer_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"hail", "air_pressure_thickness"}};
}

size_t MassContentOfHailInAtmosphereLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("hail").shape(1);
}

atlas::FunctionSpace MassContentOfHailInAtmosphereLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("hail").functionspace();
}
// -------------------------------------------------------------------------------------------------

void MassContentOfHailInAtmosphereLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "MassContentOfHailInAtmosphereLayer_A::executeNL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double hail_mr,
          const double delta_p,
          double& hail_wp) {
          hail_wp = hail_mr * delta_p / g;
      },
      afieldset.field("hail"),
      afieldset.field("air_pressure_thickness"),
      afieldset.field("mass_content_of_hail_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfHailInAtmosphereLayer_A::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfHailInAtmosphereLayer_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                      const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfHailInAtmosphereLayer_A::executeTL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          const double hail_mr_tl,
          double& hail_wp_tl) {
          hail_wp_tl = hail_mr_tl * delta_p / g;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetTL.field("hail"),
      afieldsetTL.field("mass_content_of_hail_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfHailInAtmosphereLayer_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfHailInAtmosphereLayer_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                      const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfHailInAtmosphereLayer_A::executeAD starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          double& hail_mr_ad,
          double& hail_wp_ad) {
          hail_mr_ad += hail_wp_ad * delta_p / g;
          hail_wp_ad = 0.0;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetAD.field("hail"),
      afieldsetAD.field("mass_content_of_hail_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfHailInAtmosphereLayer_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
