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
#include "vader/betaNames_CRTMRecipes/MassContentOfCloudLiquidWaterInAtmosphereLayer.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char MassContentOfCloudLiquidWaterInAtmosphereLayer_A::Name[] =
    "MassContentOfCloudLiquidWaterInAtmosphereLayer_A";
const oops::Variables MassContentOfCloudLiquidWaterInAtmosphereLayer_A::Ingredients{
    std::vector<std::string>{
      "cloud_liquid_water", "air_pressure_thickness"}};

// Register the maker
static RecipeMaker<MassContentOfCloudLiquidWaterInAtmosphereLayer_A>
  makerMassContentOfCloudLiquidWaterInAtmosphereLayer_A_(
      MassContentOfCloudLiquidWaterInAtmosphereLayer_A::Name);

// -------------------------------------------------------------------------------------------------

MassContentOfCloudLiquidWaterInAtmosphereLayer_A::MassContentOfCloudLiquidWaterInAtmosphereLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_A::"
                     << "MassContentOfCloudLiquidWaterInAtmosphereLayer_A" << std::endl;
}

std::string MassContentOfCloudLiquidWaterInAtmosphereLayer_A::name() const {
  return MassContentOfCloudLiquidWaterInAtmosphereLayer_A::Name;
}

oops::Variable MassContentOfCloudLiquidWaterInAtmosphereLayer_A::product() const {
  return oops::Variable{"mass_content_of_cloud_liquid_water_in_atmosphere_layer"};
}

oops::Variables MassContentOfCloudLiquidWaterInAtmosphereLayer_A::ingredients() const {
  return MassContentOfCloudLiquidWaterInAtmosphereLayer_A::Ingredients;
}

oops::Variables MassContentOfCloudLiquidWaterInAtmosphereLayer_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"cloud_liquid_water", "air_pressure_thickness"}};
}

size_t MassContentOfCloudLiquidWaterInAtmosphereLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("cloud_liquid_water").shape(1);
}

atlas::FunctionSpace MassContentOfCloudLiquidWaterInAtmosphereLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("cloud_liquid_water").functionspace();
}
// -------------------------------------------------------------------------------------------------

void MassContentOfCloudLiquidWaterInAtmosphereLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_A::executeNL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double clw_mr,
          const double delta_p,
          double& clw_wp) {
          clw_wp = clw_mr * delta_p / g;
      },
      afieldset.field("cloud_liquid_water"),
      afieldset.field("air_pressure_thickness"),
      afieldset.field("mass_content_of_cloud_liquid_water_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_A::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfCloudLiquidWaterInAtmosphereLayer_A::executeTL(
    atlas::FieldSet & afieldsetTL, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_A::executeTL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          const double clw_mr_tl,
          double& clw_wp_tl) {
          clw_wp_tl = clw_mr_tl * delta_p / g;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetTL.field("cloud_liquid_water"),
      afieldsetTL.field("mass_content_of_cloud_liquid_water_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_A::executeTL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfCloudLiquidWaterInAtmosphereLayer_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_A::executeAD starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          double& clw_mr_ad,
          double& clw_wp_ad) {
          clw_mr_ad += clw_wp_ad * delta_p / g;
          clw_wp_ad = 0.0;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetAD.field("cloud_liquid_water"),
      afieldsetAD.field("mass_content_of_cloud_liquid_water_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_A::executeAD done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
