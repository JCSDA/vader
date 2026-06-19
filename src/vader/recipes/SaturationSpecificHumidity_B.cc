/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/SaturationSpecificHumidity.h"

namespace vader {

// Static attribute initialization
const char SaturationSpecificHumidity_B::Name[] = "SaturationSpecificHumidity_B";
const oops::Variables SaturationSpecificHumidity_B::Ingredients{
  std::vector<std::string>{"svp", "air_pressure"}};

// Register the maker
static RecipeMaker<SaturationSpecificHumidity_B>
  makerSaturationSpecificHumidity_B_(SaturationSpecificHumidity_B::Name);

SaturationSpecificHumidity_B::SaturationSpecificHumidity_B(const Parameters_ & params,
  const VaderConfigVars & configVariables) :
  configVariables_{configVariables} {
  oops::Log::trace() << "SaturationSpecificHumidity_B::SaturationSpecificHumidity_B"
                     << std::endl;
}

std::string SaturationSpecificHumidity_B::name() const {
  return SaturationSpecificHumidity_B::Name;
}

oops::Variable SaturationSpecificHumidity_B::product() const {
  return oops::Variable{"water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"};
}

oops::Variables SaturationSpecificHumidity_B::ingredients() const {
  return Ingredients;
}

oops::Variables SaturationSpecificHumidity_B::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{
    "svp",
    "air_pressure",
    "water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"}};
}

size_t SaturationSpecificHumidity_B::productLevels(const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_pressure").shape(1);
}

atlas::FunctionSpace SaturationSpecificHumidity_B::productFunctionSpace(
  const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_pressure").functionspace();
}

// -------------------------------------------------------------------------------------------------

void SaturationSpecificHumidity_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SaturationSpecificHumidity_B::executeNL Starting" << std::endl;

  // Exact formula: qsat = eps * svp / (pressure - c * svp)
  const double eps = configVariables_.getDouble("epsilon");
  const double c = 1.0 - eps;

    util::for_each_value(
        [=](const double svp,
            const double pressure,
            double& qsat) {
            const double denom = pressure - c * svp;
            if (denom > 1.0) {
              qsat = eps * svp / denom;
            } else {
              qsat = 0.0;
            }
            // Ensure non-negative and bounded
            qsat = std::max(0.0, std::min(1.0, qsat));
        },
        afieldset["svp"],
        afieldset["air_pressure"],
        afieldset["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"]);

  oops::Log::trace() << "SaturationSpecificHumidity_B::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SaturationSpecificHumidity_B::executeTL(atlas::FieldSet & afieldsetTL,
                                               const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SaturationSpecificHumidity_B::executeTL Starting" << std::endl;

  const double eps = configVariables_.getDouble("epsilon");
  const double c = 1.0 - eps;

    util::for_each_value(
        [=](const double svp,
            const double pressure,
            const double qsat,
            const double svp_tl,
            const double pressure_tl,
            double& qsat_tl) {
            const double denom = pressure - c * svp;
            if (denom > 1.0 && qsat > 0.0 && qsat < 1.0) {
              const double denom2 = denom * denom;
              const double dqsat_dsvp = eps * pressure / denom2;
              const double dqsat_dp = -eps * svp / denom2;
              qsat_tl = dqsat_dsvp * svp_tl + dqsat_dp * pressure_tl;
            } else {
              qsat_tl = 0.0;
            }
        },
        afieldsetTraj["svp"],
        afieldsetTraj["air_pressure"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"],
        afieldsetTL["svp"],
        afieldsetTL["air_pressure"],
        afieldsetTL["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"]);

  oops::Log::trace() << "SaturationSpecificHumidity_B::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SaturationSpecificHumidity_B::executeAD(atlas::FieldSet & afieldsetAD,
                                               const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SaturationSpecificHumidity_B::executeAD Starting" << std::endl;

  const double eps = configVariables_.getDouble("epsilon");
  const double c = 1.0 - eps;

    util::for_each_value(
        [=](const double svp,
            const double pressure,
            const double qsat,
            double& svp_ad,
            double& pressure_ad,
            double& qsat_ad) {
            const double denom = pressure - c * svp;
            if (denom > 1.0 && qsat > 0.0 && qsat < 1.0) {
              const double denom2 = denom * denom;
              const double dqsat_dsvp = eps * pressure / denom2;
              const double dqsat_dp = -eps * svp / denom2;
              svp_ad += dqsat_dsvp * qsat_ad;
              pressure_ad += dqsat_dp * qsat_ad;
            }
            qsat_ad = 0.0;
        },
        afieldsetTraj["svp"],
        afieldsetTraj["air_pressure"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"],
        afieldsetAD["svp"],
        afieldsetAD["air_pressure"],
        afieldsetAD["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"]);

  oops::Log::trace() << "SaturationSpecificHumidity_B::executeAD Done" << std::endl;
}

}  // namespace vader
