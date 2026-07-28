/*
 * (C) Copyright 2026 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/EffectiveRadiusOfCloudIceParticle.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char EffectiveRadiusOfCloudIceParticle_B::Name[] =
    "EffectiveRadiusOfCloudIceParticle_B";
const oops::Variables EffectiveRadiusOfCloudIceParticle_B::Ingredients{
    std::vector<std::string>{
      "air_temperature", "cloud_liquid_ice",
      "air_pressure", "water_vapor_mixing_ratio_wrt_moist_air"}};

// Register the maker
static RecipeMaker<EffectiveRadiusOfCloudIceParticle_B>
  makerEffectiveRadiusOfCloudIceParticle_B_(
      EffectiveRadiusOfCloudIceParticle_B::Name);

// -------------------------------------------------------------------------------------------------

EffectiveRadiusOfCloudIceParticle_B::EffectiveRadiusOfCloudIceParticle_B(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "EffectiveRadiusOfCloudIceParticle_B::"
                     << "EffectiveRadiusOfCloudIceParticle_B" << std::endl;
}

std::string EffectiveRadiusOfCloudIceParticle_B::name() const {
  return EffectiveRadiusOfCloudIceParticle_B::Name;
}

oops::Variable EffectiveRadiusOfCloudIceParticle_B::product() const {
  return oops::Variable{"effective_radius_of_cloud_ice_particle"};
}

oops::Variables EffectiveRadiusOfCloudIceParticle_B::ingredients() const {
  return EffectiveRadiusOfCloudIceParticle_B::Ingredients;
}

size_t EffectiveRadiusOfCloudIceParticle_B::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace EffectiveRadiusOfCloudIceParticle_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfCloudIceParticle_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "EffectiveRadiusOfCloudIceParticle_B::executeNL starting" << std::endl;

  // Reproduces fv3-jedi crtm_ade_efr `gsi` branch for qi_efr (microns).
  const double tice = configVariables_.getDouble("tice");
  const double rdry = configVariables_.getDouble("rdry");
  const double zvir = configVariables_.getDouble("zvir");
  const double min_qx = 1.0e-8;

  util::for_each_value(
      [=](const double temp,
          const double qi,
          const double pressure,
          const double qv,
          double& reff_out) {
          if (qi < min_qx) {
            reff_out = 0.0;
            return;
          }
          const double q_ge_0 = std::max(qv, 0.0);
          const double rho_air = pressure / (rdry * temp * (1.0 + zvir * q_ge_0));
          // Fortran declares `wcontent` as default `real` (single precision); reproduce the
          // intermediate float rounding here so the resulting effective radius bit-matches.
          const float wcontent = static_cast<float>(qi * rho_air);
          const double tem2 = temp - tice;
          double reff;
          if (tem2 < -50.0) {
            reff = (1250.0 / 9.917) * std::pow(static_cast<double>(wcontent), 0.109);
          } else if (tem2 < -40.0) {
            reff = (1250.0 / 9.337) * std::pow(static_cast<double>(wcontent), 0.08);
          } else if (tem2 < -30.0) {
            reff = (1250.0 / 9.208) * std::pow(static_cast<double>(wcontent), 0.055);
          } else {
            reff = (1250.0 / 9.387) * std::pow(static_cast<double>(wcontent), 0.031);
          }
          reff_out = std::max(5.0, reff);
      },
      afieldset.field("air_temperature"),
      afieldset.field("cloud_liquid_ice"),
      afieldset.field("air_pressure"),
      afieldset.field("water_vapor_mixing_ratio_wrt_moist_air"),
      afieldset.field("effective_radius_of_cloud_ice_particle"));

  oops::Log::trace() << "EffectiveRadiusOfCloudIceParticle_B::executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
