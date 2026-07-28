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
#include "vader/betaNames_CRTMRecipes/MassContentOfCloudIceInAtmosphereLayer.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char MassContentOfCloudIceInAtmosphereLayer_B::Name[] =
    "MassContentOfCloudIceInAtmosphereLayer_B";
const oops::Variables MassContentOfCloudIceInAtmosphereLayer_B::Ingredients{
    std::vector<std::string>{
      "cloud_liquid_ice", "air_pressure_thickness", "slmsk"}};

// Register the maker
static RecipeMaker<MassContentOfCloudIceInAtmosphereLayer_B>
  makerMassContentOfCloudIceInAtmosphereLayer_B_(
      MassContentOfCloudIceInAtmosphereLayer_B::Name);

// -------------------------------------------------------------------------------------------------

MassContentOfCloudIceInAtmosphereLayer_B::MassContentOfCloudIceInAtmosphereLayer_B(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables},
    maskOver_{params.maskOver.value()} {
  ASSERT_MSG(maskOver_ == "land" || maskOver_ == "sea" || maskOver_ == "none",
             "MassContentOfCloudIceInAtmosphereLayer_B: 'mask over' must be \"land\", \"sea\","
             " or \"none\", got \"" + maskOver_ + "\"");
  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_B::"
                     << "MassContentOfCloudIceInAtmosphereLayer_B"
                     << " (mask over = " << maskOver_ << ")" << std::endl;
}

std::string MassContentOfCloudIceInAtmosphereLayer_B::name() const {
  return MassContentOfCloudIceInAtmosphereLayer_B::Name;
}

oops::Variable MassContentOfCloudIceInAtmosphereLayer_B::product() const {
  return oops::Variable{"mass_content_of_cloud_ice_in_atmosphere_layer"};
}

oops::Variables MassContentOfCloudIceInAtmosphereLayer_B::ingredients() const {
  return MassContentOfCloudIceInAtmosphereLayer_B::Ingredients;
}

size_t MassContentOfCloudIceInAtmosphereLayer_B::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("cloud_liquid_ice").shape(1);
}

atlas::FunctionSpace MassContentOfCloudIceInAtmosphereLayer_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("cloud_liquid_ice").functionspace();
}

// -------------------------------------------------------------------------------------------------

void MassContentOfCloudIceInAtmosphereLayer_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_B::executeNL starting"
                     << std::endl;

  const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
  const double min_qx = 1.0e-8;

  const bool maskLand = (maskOver_ == "land");
  const bool maskSea  = (maskOver_ == "sea");

  const size_t nz = afieldset.field("cloud_liquid_ice").shape(1);

  util::for_each_column(
      [&](const auto qi_col,
          const auto delp_col,
          const auto slmsk_col,
          auto out_col) {
          const bool seamask = (std::abs(slmsk_col(0)) < 0.5);
          const bool maskedOut = (maskLand && !seamask) || (maskSea && seamask);
          for (size_t jl = 0; jl < nz; ++jl) {
            if (maskedOut || qi_col(jl) < min_qx) {
              out_col(jl) = 0.0;
            } else {
              // Match Fortran order: kgkg_to_kgm2 = delp/grav; qwp = q * kgkg_to_kgm2
              const double kgkg_to_kgm2 = delp_col(jl) / grav;
              out_col(jl) = qi_col(jl) * kgkg_to_kgm2;
            }
          }
      },
      afieldset.field("cloud_liquid_ice"),
      afieldset.field("air_pressure_thickness"),
      afieldset.field("slmsk"),
      afieldset.field("mass_content_of_cloud_ice_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfCloudIceInAtmosphereLayer_B::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
