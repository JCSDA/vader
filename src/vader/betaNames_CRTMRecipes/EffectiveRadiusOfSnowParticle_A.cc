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
#include "vader/betaNames_CRTMRecipes/EffectiveRadiusOfSnowParticle.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char EffectiveRadiusOfSnowParticle_A::Name[] =
    "EffectiveRadiusOfSnowParticle_A";
const oops::Variables EffectiveRadiusOfSnowParticle_A::Ingredients{
    std::vector<std::string>{"air_temperature"}};

// Register the maker
static RecipeMaker<EffectiveRadiusOfSnowParticle_A>
  makerEffectiveRadiusOfSnowParticle_A_(
      EffectiveRadiusOfSnowParticle_A::Name);

// -------------------------------------------------------------------------------------------------

EffectiveRadiusOfSnowParticle_A::EffectiveRadiusOfSnowParticle_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "EffectiveRadiusOfSnowParticle_A::"
                     << "EffectiveRadiusOfSnowParticle_A" << std::endl;
}

std::string EffectiveRadiusOfSnowParticle_A::name() const {
  return EffectiveRadiusOfSnowParticle_A::Name;
}

oops::Variable EffectiveRadiusOfSnowParticle_A::product() const {
  return oops::Variable{"effective_radius_of_snow_particle"};
}

oops::Variables EffectiveRadiusOfSnowParticle_A::ingredients() const {
  return EffectiveRadiusOfSnowParticle_A::Ingredients;
}

size_t EffectiveRadiusOfSnowParticle_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace EffectiveRadiusOfSnowParticle_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfSnowParticle_A::executeNL(
    atlas::FieldSet & afieldset) {
  oops::Log::trace() << "EffectiveRadiusOfSnowParticle_A::"
                     << "executeNL starting" << std::endl;

  const double T_freeze = 273.15;  // K
  const double T_cold = 233.15;    // -40C
  const double r_min = 100.0e-6;   // minimum radius (m)
  const double r_max = 1000.0e-6;  // maximum radius (m)

  util::for_each_value(
      [=](const double temp,
          double& reff) {
          if (temp < T_freeze) {
            const double temp_factor = (temp - T_cold) / (T_freeze - T_cold);
            reff = r_min + (r_max - r_min) * std::max(0.0, std::min(1.0, temp_factor));
          } else {
            reff = r_min;
          }
      },
      afieldset.field("air_temperature"),
      afieldset.field("effective_radius_of_snow_particle"));

  oops::Log::trace() << "EffectiveRadiusOfSnowParticle_A::"
                     << "executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

oops::Variables EffectiveRadiusOfSnowParticle_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"air_temperature"}};
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfSnowParticle_A::executeTL(
    atlas::FieldSet & afieldsetTL, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfSnowParticle_A::executeTL starting" << std::endl;

  const double T_freeze = 273.15;
  const double T_cold   = 233.15;
  const double r_min    = 100.0e-6;
  const double r_max    = 1000.0e-6;

  util::for_each_value(
      [=](const double temp_traj,
          const double temp_tl,
          double& reff_tl) {
          if (temp_traj < T_freeze && temp_traj > T_cold) {
            reff_tl = (r_max - r_min) / (T_freeze - T_cold) * temp_tl;
          } else {
            reff_tl = 0.0;
          }
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetTL.field("air_temperature"),
      afieldsetTL.field("effective_radius_of_snow_particle"));

  oops::Log::trace() << "EffectiveRadiusOfSnowParticle_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfSnowParticle_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfSnowParticle_A::executeAD starting" << std::endl;

  const double T_freeze = 273.15;
  const double T_cold   = 233.15;
  const double r_min    = 100.0e-6;
  const double r_max    = 1000.0e-6;

  util::for_each_value(
      [=](const double temp_traj,
          double& temp_ad,
          double& reff_ad) {
          if (temp_traj < T_freeze && temp_traj > T_cold) {
            temp_ad += (r_max - r_min) / (T_freeze - T_cold) * reff_ad;
          }
          reff_ad = 0.0;
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetAD.field("air_temperature"),
      afieldsetAD.field("effective_radius_of_snow_particle"));

  oops::Log::trace() << "EffectiveRadiusOfSnowParticle_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
