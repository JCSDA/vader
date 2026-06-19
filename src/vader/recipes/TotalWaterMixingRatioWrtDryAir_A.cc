/*
 * (C) Crown Copyright 2023-2025 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field.h"
// #include "atlas/util/Metadata.h"
#include "mo/eval_total_water_mixing_ratio.h"
#include "oops/util/Logger.h"
#include "vader/recipes/TotalWaterMixingRatioWrtDryAir.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char TotalWaterMixingRatioWrtDryAir_A::Name[] = "TotalWaterMixingRatioWrtDryAir_A";
const oops::Variables TotalWaterMixingRatioWrtDryAir_A::
  Ingredients{std::vector<std::string>{"water_vapor_mixing_ratio_wrt_dry_air",
                                       "cloud_ice_mixing_ratio_wrt_dry_air",
                                       "cloud_liquid_water_mixing_ratio_wrt_dry_air",
                                       "rain_mixing_ratio_wrt_dry_air"}};

// Register the maker
static RecipeMaker<TotalWaterMixingRatioWrtDryAir_A>
  makerTotalWaterMixingRatioWrtDryAir_A_(TotalWaterMixingRatioWrtDryAir_A::Name);

TotalWaterMixingRatioWrtDryAir_A::TotalWaterMixingRatioWrtDryAir_A(
    const Parameters_ & params, const VaderConfigVars & configVariables)
{
  oops::Log::trace()
    << "TotalWaterMixingRatioWrtDryAir_A::TotalWaterMixingRatioWrtDryAir_A(params, configVariables)"
    << std::endl;
}

std::string TotalWaterMixingRatioWrtDryAir_A::name() const
{
  return TotalWaterMixingRatioWrtDryAir_A::Name;
}

oops::Variable TotalWaterMixingRatioWrtDryAir_A::product() const
{
  return oops::Variable{"total_water_mixing_ratio_wrt_dry_air"};
}

oops::Variables TotalWaterMixingRatioWrtDryAir_A::ingredients() const
{
  return TotalWaterMixingRatioWrtDryAir_A::Ingredients;
}


size_t TotalWaterMixingRatioWrtDryAir_A::productLevels(const atlas::FieldSet & afieldset) const
{
  return (afieldset["water_vapor_mixing_ratio_wrt_dry_air"].shape(1));
}

atlas::FunctionSpace TotalWaterMixingRatioWrtDryAir_A::productFunctionSpace(const atlas::FieldSet
                                                        & afieldset) const
{
  return afieldset["water_vapor_mixing_ratio_wrt_dry_air"].functionspace();
}
// -------------------------------------------------------------------------------------------------

void TotalWaterMixingRatioWrtDryAir_A::executeNL(atlas::FieldSet & afieldset)
{
  oops::Log::trace()
    << "entering TotalWaterMixingRatioWrtDryAir_A::executeNL function"
    << std::endl;
  mo::eval_total_water_mixing_ratio_wrt_dry_air_nl(afieldset);
  oops::Log::trace()
    << "leaving TotalWaterMixingRatioWrtDryAir_A::executeNL function"
    << std::endl;
}

}  // namespace vader
