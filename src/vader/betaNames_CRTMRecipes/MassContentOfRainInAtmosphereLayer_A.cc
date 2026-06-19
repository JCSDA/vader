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
#include "vader/betaNames_CRTMRecipes/MassContentOfRainInAtmosphereLayer.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char MassContentOfRainInAtmosphereLayer_A::Name[] =
    "MassContentOfRainInAtmosphereLayer_A";
const oops::Variables MassContentOfRainInAtmosphereLayer_A::Ingredients{
    std::vector<std::string>{
      "rain_water", "air_pressure_thickness"}};

// Register the maker
static RecipeMaker<MassContentOfRainInAtmosphereLayer_A>
  makerMassContentOfRainInAtmosphereLayer_A_(
      MassContentOfRainInAtmosphereLayer_A::Name);

// -------------------------------------------------------------------------------------------------

MassContentOfRainInAtmosphereLayer_A::MassContentOfRainInAtmosphereLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "MassContentOfRainInAtmosphereLayer_A::"
                     << "MassContentOfRainInAtmosphereLayer_A" << std::endl;
}

std::string MassContentOfRainInAtmosphereLayer_A::name() const {
  return MassContentOfRainInAtmosphereLayer_A::Name;
}

oops::Variable MassContentOfRainInAtmosphereLayer_A::product() const {
  return oops::Variable{"mass_content_of_rain_in_atmosphere_layer"};
}

oops::Variables MassContentOfRainInAtmosphereLayer_A::ingredients() const {
  return MassContentOfRainInAtmosphereLayer_A::Ingredients;
}

oops::Variables MassContentOfRainInAtmosphereLayer_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"rain_water", "air_pressure_thickness"}};
}

size_t MassContentOfRainInAtmosphereLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("rain_water").shape(1);
}

atlas::FunctionSpace MassContentOfRainInAtmosphereLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("rain_water").functionspace();
}
// -------------------------------------------------------------------------------------------------

void MassContentOfRainInAtmosphereLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "MassContentOfRainInAtmosphereLayer_A::executeNL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double rain_mr,
          const double delta_p,
          double& rain_wp) {
          rain_wp = rain_mr * delta_p / g;
      },
      afieldset.field("rain_water"),
      afieldset.field("air_pressure_thickness"),
      afieldset.field("mass_content_of_rain_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfRainInAtmosphereLayer_A::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfRainInAtmosphereLayer_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                      const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfRainInAtmosphereLayer_A::executeTL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          const double rain_mr_tl,
          double& rain_wp_tl) {
          rain_wp_tl = rain_mr_tl * delta_p / g;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetTL.field("rain_water"),
      afieldsetTL.field("mass_content_of_rain_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfRainInAtmosphereLayer_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfRainInAtmosphereLayer_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                      const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfRainInAtmosphereLayer_A::executeAD starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          double& rain_mr_ad,
          double& rain_wp_ad) {
          rain_mr_ad += rain_wp_ad * delta_p / g;
          rain_wp_ad = 0.0;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetAD.field("rain_water"),
      afieldsetAD.field("mass_content_of_rain_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfRainInAtmosphereLayer_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
