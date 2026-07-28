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
#include "vader/betaNames_CRTMRecipes/MassContentOfCloudLiquidWaterInAtmosphereLayer.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char MassContentOfCloudLiquidWaterInAtmosphereLayer_B::Name[] =
    "MassContentOfCloudLiquidWaterInAtmosphereLayer_B";
const oops::Variables MassContentOfCloudLiquidWaterInAtmosphereLayer_B::Ingredients{
    std::vector<std::string>{
      "cloud_liquid_water", "air_pressure_thickness", "slmsk"}};

// Register the maker
static RecipeMaker<MassContentOfCloudLiquidWaterInAtmosphereLayer_B>
  makerMassContentOfCloudLiquidWaterInAtmosphereLayer_B_(
      MassContentOfCloudLiquidWaterInAtmosphereLayer_B::Name);

// -------------------------------------------------------------------------------------------------

MassContentOfCloudLiquidWaterInAtmosphereLayer_B::MassContentOfCloudLiquidWaterInAtmosphereLayer_B(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables},
    maskOver_{params.maskOver.value()} {
  ASSERT_MSG(maskOver_ == "land" || maskOver_ == "sea" || maskOver_ == "none",
             "MassContentOfCloudLiquidWaterInAtmosphereLayer_B: 'mask over' must be"
             " \"land\", \"sea\", or \"none\", got \"" + maskOver_ + "\"");
  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_B::"
                     << "MassContentOfCloudLiquidWaterInAtmosphereLayer_B"
                     << " (mask over = " << maskOver_ << ")" << std::endl;
}

std::string MassContentOfCloudLiquidWaterInAtmosphereLayer_B::name() const {
  return MassContentOfCloudLiquidWaterInAtmosphereLayer_B::Name;
}

oops::Variable MassContentOfCloudLiquidWaterInAtmosphereLayer_B::product() const {
  return oops::Variable{"mass_content_of_cloud_liquid_water_in_atmosphere_layer"};
}

oops::Variables MassContentOfCloudLiquidWaterInAtmosphereLayer_B::ingredients() const {
  return MassContentOfCloudLiquidWaterInAtmosphereLayer_B::Ingredients;
}

size_t MassContentOfCloudLiquidWaterInAtmosphereLayer_B::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("cloud_liquid_water").shape(1);
}

atlas::FunctionSpace MassContentOfCloudLiquidWaterInAtmosphereLayer_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("cloud_liquid_water").functionspace();
}

// -------------------------------------------------------------------------------------------------

void MassContentOfCloudLiquidWaterInAtmosphereLayer_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_B::executeNL starting"
                     << std::endl;

  // Match the fv3-jedi GSI branch in crtm_ade_efr / hydro_mixr_to_wpath:
  //   ql_ade(i,j,k) = (ql >= 1e-8) ? ql * delp / g : 0
  //   apply land/sea mask (seamask = sea_frac >= 0.99, i.e. slmsk == 0)
  const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
  const double min_qx = 1.0e-8;

  const bool maskLand = (maskOver_ == "land");
  const bool maskSea  = (maskOver_ == "sea");

  const size_t nz = afieldset.field("cloud_liquid_water").shape(1);

  util::for_each_column(
      [&](const auto ql_col,
          const auto delp_col,
          const auto slmsk_col,
          auto out_col) {
          // sea_frac is 1.0 where slmsk == 0 (ocean), else 0.0 ;  seamask = sea_frac >= 0.99
          const bool seamask = (std::abs(slmsk_col(0)) < 0.5);
          const bool maskedOut = (maskLand && !seamask) || (maskSea && seamask);
          for (size_t jl = 0; jl < nz; ++jl) {
            if (maskedOut || ql_col(jl) < min_qx) {
              out_col(jl) = 0.0;
            } else {
              // Match Fortran order: kgkg_to_kgm2 = delp/grav; qwp = q * kgkg_to_kgm2
              const double kgkg_to_kgm2 = delp_col(jl) / grav;
              out_col(jl) = ql_col(jl) * kgkg_to_kgm2;
            }
          }
      },
      afieldset.field("cloud_liquid_water"),
      afieldset.field("air_pressure_thickness"),
      afieldset.field("slmsk"),
      afieldset.field("mass_content_of_cloud_liquid_water_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfCloudLiquidWaterInAtmosphereLayer_B::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
