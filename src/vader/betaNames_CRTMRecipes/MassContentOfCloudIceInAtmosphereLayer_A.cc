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
#include "vader/betaNames_CRTMRecipes/MassContentOfCloudIceInAtmosphereLayer.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char MassContentOfCloudIceInAtmosphereLayer_A::Name[] =
    "MassContentOfCloudIceInAtmosphereLayer_A";
const oops::Variables MassContentOfCloudIceInAtmosphereLayer_A::Ingredients{
    std::vector<std::string>{
      "cloud_liquid_ice", "air_pressure_thickness"}};

// Register the maker
static RecipeMaker<MassContentOfCloudIceInAtmosphereLayer_A>
  makerMassContentOfCloudIceInAtmosphereLayer_A_(
      MassContentOfCloudIceInAtmosphereLayer_A::Name);

// -------------------------------------------------------------------------------------------------

MassContentOfCloudIceInAtmosphereLayer_A::MassContentOfCloudIceInAtmosphereLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_A::"
                     << "MassContentOfCloudIceInAtmosphereLayer_A" << std::endl;
}

std::string MassContentOfCloudIceInAtmosphereLayer_A::name() const {
  return MassContentOfCloudIceInAtmosphereLayer_A::Name;
}

oops::Variable MassContentOfCloudIceInAtmosphereLayer_A::product() const {
  return oops::Variable{"mass_content_of_cloud_ice_in_atmosphere_layer"};
}

oops::Variables MassContentOfCloudIceInAtmosphereLayer_A::ingredients() const {
  return MassContentOfCloudIceInAtmosphereLayer_A::Ingredients;
}

oops::Variables MassContentOfCloudIceInAtmosphereLayer_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"cloud_liquid_ice", "air_pressure_thickness"}};
}

size_t MassContentOfCloudIceInAtmosphereLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("cloud_liquid_ice").shape(1);
}

atlas::FunctionSpace MassContentOfCloudIceInAtmosphereLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("cloud_liquid_ice").functionspace();
}

// -------------------------------------------------------------------------------------------------

void MassContentOfCloudIceInAtmosphereLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_A::executeNL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double ice_mr,
          const double delta_p,
          double& ice_wp) {
          ice_wp = ice_mr * delta_p / g;
      },
      afieldset.field("cloud_liquid_ice"),
      afieldset.field("air_pressure_thickness"),
      afieldset.field("mass_content_of_cloud_ice_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_A::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfCloudIceInAtmosphereLayer_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                          const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_A::executeTL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          const double ice_mr_tl,
          double& ice_wp_tl) {
          ice_wp_tl = ice_mr_tl * delta_p / g;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetTL.field("cloud_liquid_ice"),
      afieldsetTL.field("mass_content_of_cloud_ice_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_A::executeTL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfCloudIceInAtmosphereLayer_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                          const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_A::executeAD starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          double& ice_mr_ad,
          double& ice_wp_ad) {
          ice_mr_ad += ice_wp_ad * delta_p / g;
          ice_wp_ad = 0.0;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetAD.field("cloud_liquid_ice"),
      afieldsetAD.field("mass_content_of_cloud_ice_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_A::executeAD done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
