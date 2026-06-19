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
#include "vader/betaNames_CRTMRecipes/MassContentOfSnowInAtmosphereLayer.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char MassContentOfSnowInAtmosphereLayer_A::Name[] =
    "MassContentOfSnowInAtmosphereLayer_A";
const oops::Variables MassContentOfSnowInAtmosphereLayer_A::Ingredients{
    std::vector<std::string>{
      "snow_water", "air_pressure_thickness"}};

// Register the maker
static RecipeMaker<MassContentOfSnowInAtmosphereLayer_A>
  makerMassContentOfSnowInAtmosphereLayer_A_(
      MassContentOfSnowInAtmosphereLayer_A::Name);

// -------------------------------------------------------------------------------------------------

MassContentOfSnowInAtmosphereLayer_A::MassContentOfSnowInAtmosphereLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "MassContentOfSnowInAtmosphereLayer_A::"
                     << "MassContentOfSnowInAtmosphereLayer_A" << std::endl;
}

std::string MassContentOfSnowInAtmosphereLayer_A::name() const {
  return MassContentOfSnowInAtmosphereLayer_A::Name;
}

oops::Variable MassContentOfSnowInAtmosphereLayer_A::product() const {
  return oops::Variable{"mass_content_of_snow_in_atmosphere_layer"};
}

oops::Variables MassContentOfSnowInAtmosphereLayer_A::ingredients() const {
  return MassContentOfSnowInAtmosphereLayer_A::Ingredients;
}

oops::Variables MassContentOfSnowInAtmosphereLayer_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"snow_water", "air_pressure_thickness"}};
}

size_t MassContentOfSnowInAtmosphereLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("snow_water").shape(1);
}

atlas::FunctionSpace MassContentOfSnowInAtmosphereLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("snow_water").functionspace();
}
// -------------------------------------------------------------------------------------------------

void MassContentOfSnowInAtmosphereLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "MassContentOfSnowInAtmosphereLayer_A::executeNL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double snow_mr,
          const double delta_p,
          double& snow_wp) {
          snow_wp = snow_mr * delta_p / g;
      },
      afieldset.field("snow_water"),
      afieldset.field("air_pressure_thickness"),
      afieldset.field("mass_content_of_snow_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfSnowInAtmosphereLayer_A::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfSnowInAtmosphereLayer_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                      const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfSnowInAtmosphereLayer_A::executeTL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          const double snow_mr_tl,
          double& snow_wp_tl) {
          snow_wp_tl = snow_mr_tl * delta_p / g;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetTL.field("snow_water"),
      afieldsetTL.field("mass_content_of_snow_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfSnowInAtmosphereLayer_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfSnowInAtmosphereLayer_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                      const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfSnowInAtmosphereLayer_A::executeAD starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          double& snow_mr_ad,
          double& snow_wp_ad) {
          snow_mr_ad += snow_wp_ad * delta_p / g;
          snow_wp_ad = 0.0;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetAD.field("snow_water"),
      afieldsetAD.field("mass_content_of_snow_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfSnowInAtmosphereLayer_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
