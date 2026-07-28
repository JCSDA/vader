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
#include "vader/betaNames_CRTMRecipes/EffectiveRadiusOfCloudLiquidWaterParticle.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char EffectiveRadiusOfCloudLiquidWaterParticle_B::Name[] =
    "EffectiveRadiusOfCloudLiquidWaterParticle_B";
const oops::Variables EffectiveRadiusOfCloudLiquidWaterParticle_B::Ingredients{
    std::vector<std::string>{"air_temperature", "cloud_liquid_water"}};

// Register the maker
static RecipeMaker<EffectiveRadiusOfCloudLiquidWaterParticle_B>
  makerEffectiveRadiusOfCloudLiquidWaterParticle_B_(
      EffectiveRadiusOfCloudLiquidWaterParticle_B::Name);

// -------------------------------------------------------------------------------------------------

EffectiveRadiusOfCloudLiquidWaterParticle_B::EffectiveRadiusOfCloudLiquidWaterParticle_B(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_B::"
                     << "EffectiveRadiusOfCloudLiquidWaterParticle_B" << std::endl;
}

std::string EffectiveRadiusOfCloudLiquidWaterParticle_B::name() const {
  return EffectiveRadiusOfCloudLiquidWaterParticle_B::Name;
}

oops::Variable EffectiveRadiusOfCloudLiquidWaterParticle_B::product() const {
  return oops::Variable{"effective_radius_of_cloud_liquid_water_particle"};
}

oops::Variables EffectiveRadiusOfCloudLiquidWaterParticle_B::ingredients() const {
  return EffectiveRadiusOfCloudLiquidWaterParticle_B::Ingredients;
}

size_t EffectiveRadiusOfCloudLiquidWaterParticle_B::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace EffectiveRadiusOfCloudLiquidWaterParticle_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfCloudLiquidWaterParticle_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_B::executeNL starting"
                     << std::endl;

  // Match the fv3-jedi GSI branch in crtm_ade_efr:
  //   if ql < 1e-8: ql_efr = 0
  //   else:         ql_efr = max(1, 5 + 5*min(1, (tice - T)*0.05))   [microns]
  const double tice = configVariables_.getDouble("tice");
  const double min_qx = 1.0e-8;

  util::for_each_value(
      [=](const double temp,
          const double ql,
          double& reff_out) {
          if (ql < min_qx) {
            reff_out = 0.0;
          } else {
            const double tem1 = (tice - temp) * 0.05;
            const double reff = 5.0 + 5.0 * std::min(1.0, tem1);
            reff_out = std::max(1.0, reff);
          }
      },
      afieldset.field("air_temperature"),
      afieldset.field("cloud_liquid_water"),
      afieldset.field("effective_radius_of_cloud_liquid_water_particle"));

  oops::Log::trace() << "EffectiveRadiusOfCloudLiquidWaterParticle_B::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
