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
#include "vader/betaNames_CRTMRecipes/EffectiveRadiusOfCloudLiquidWaterParticle.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char EffectiveRadiusOfCloudLiquidWaterParticle_A::Name[] =
    "EffectiveRadiusOfCloudLiquidWaterParticle_A";
const oops::Variables EffectiveRadiusOfCloudLiquidWaterParticle_A::Ingredients{
    std::vector<std::string>{"air_temperature"}};

// Register the maker
static RecipeMaker<EffectiveRadiusOfCloudLiquidWaterParticle_A>
  makerEffectiveRadiusOfCloudLiquidWaterParticle_A_(
      EffectiveRadiusOfCloudLiquidWaterParticle_A::Name);

// -------------------------------------------------------------------------------------------------

EffectiveRadiusOfCloudLiquidWaterParticle_A::EffectiveRadiusOfCloudLiquidWaterParticle_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_A::"
                     << "EffectiveRadiusOfCloudLiquidWaterParticle_A" << std::endl;
}

std::string EffectiveRadiusOfCloudLiquidWaterParticle_A::name() const {
  return EffectiveRadiusOfCloudLiquidWaterParticle_A::Name;
}

oops::Variable EffectiveRadiusOfCloudLiquidWaterParticle_A::product() const {
  return oops::Variable{"effective_radius_of_cloud_liquid_water_particle"};
}

oops::Variables EffectiveRadiusOfCloudLiquidWaterParticle_A::ingredients() const {
  return EffectiveRadiusOfCloudLiquidWaterParticle_A::Ingredients;
}

size_t EffectiveRadiusOfCloudLiquidWaterParticle_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace EffectiveRadiusOfCloudLiquidWaterParticle_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfCloudLiquidWaterParticle_A::executeNL(
    atlas::FieldSet & afieldset) {
  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_A::"
                     << "executeNL starting" << std::endl;

  const double T_freeze = 273.15;  // K
  const double r_min = 4.0e-6;     // minimum radius (m)
  const double r_max = 15.0e-6;    // maximum radius (m)

  util::for_each_value(
      [=](const double temp,
          double& reff) {
          if (temp > T_freeze) {
            const double T_range = 30.0;  // Temperature range for scaling
            const double temp_factor = std::min(1.0, (temp - T_freeze) / T_range);
            reff = r_min + (r_max - r_min) * temp_factor;
          } else {
            reff = r_min;
          }
      },
      afieldset.field("air_temperature"),
      afieldset.field("effective_radius_of_cloud_liquid_water_particle"));

  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_A::"
                     << "executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

oops::Variables EffectiveRadiusOfCloudLiquidWaterParticle_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"air_temperature"}};
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfCloudLiquidWaterParticle_A::executeTL(
    atlas::FieldSet & afieldsetTL, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_A::executeTL starting"
                     << std::endl;

  const double T_freeze = 273.15;
  const double T_range  = 30.0;
  const double r_min    = 4.0e-6;
  const double r_max    = 15.0e-6;

  util::for_each_value(
      [=](const double temp_traj,
          const double temp_tl,
          double& reff_tl) {
          if (temp_traj > T_freeze && (temp_traj - T_freeze) < T_range) {
            reff_tl = (r_max - r_min) / T_range * temp_tl;
          } else {
            reff_tl = 0.0;
          }
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetTL.field("air_temperature"),
      afieldsetTL.field("effective_radius_of_cloud_liquid_water_particle"));

  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_A::executeTL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfCloudLiquidWaterParticle_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_A::executeAD starting"
                     << std::endl;

  const double T_freeze = 273.15;
  const double T_range  = 30.0;
  const double r_min    = 4.0e-6;
  const double r_max    = 15.0e-6;

  util::for_each_value(
      [=](const double temp_traj,
          double& temp_ad,
          double& reff_ad) {
          if (temp_traj > T_freeze && (temp_traj - T_freeze) < T_range) {
            temp_ad += (r_max - r_min) / T_range * reff_ad;
          }
          reff_ad = 0.0;
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetAD.field("air_temperature"),
      afieldsetAD.field("effective_radius_of_cloud_liquid_water_particle"));

  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_A::executeAD done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
