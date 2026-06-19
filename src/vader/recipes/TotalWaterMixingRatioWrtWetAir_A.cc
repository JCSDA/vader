/*
 * (C) Crown Copyright 2025 Met Office.
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
#include "vader/recipes/TotalWaterMixingRatioWrtWetAir.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char TotalWaterMixingRatioWrtWetAir_A::Name[] =
    "TotalWaterMixingRatioWrtWetAir_A";
const oops::Variables TotalWaterMixingRatioWrtWetAir_A::
  Ingredients{std::vector<std::string>{
    "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water",
    "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water",
    "qrain"}};

// Register the maker
static RecipeMaker<TotalWaterMixingRatioWrtWetAir_A>
  makerTotalWaterMixingRatioWrtWetAir_A_(
    TotalWaterMixingRatioWrtWetAir_A::Name);

TotalWaterMixingRatioWrtWetAir_A::
    TotalWaterMixingRatioWrtWetAir_A(
    const Parameters_ & params, const VaderConfigVars & configVariables)
{
  oops::Log::trace()
    << "TotalWaterMixingRatioWrtWetAir_A::"
    << "TotalWaterMixingRatioWrtWetAir_A(params, configVariables)"
    << std::endl;
}

std::string TotalWaterMixingRatioWrtWetAir_A::name() const
{
  return TotalWaterMixingRatioWrtWetAir_A::Name;
}

oops::Variable TotalWaterMixingRatioWrtWetAir_A::product() const
{
  return oops::Variable{"total_water_mixing_ratio_wrt_moist_air_and_condensed_water"};
}

oops::Variables TotalWaterMixingRatioWrtWetAir_A::ingredients() const
{
  return TotalWaterMixingRatioWrtWetAir_A::Ingredients;
}

size_t TotalWaterMixingRatioWrtWetAir_A::productLevels(
    const atlas::FieldSet & afieldset) const
{
  return (afieldset["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"].shape(1));
}

atlas::FunctionSpace TotalWaterMixingRatioWrtWetAir_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
  return afieldset["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"].functionspace();
}
// -------------------------------------------------------------------------------------------------

void TotalWaterMixingRatioWrtWetAir_A::executeNL(atlas::FieldSet & afieldset)
{
  oops::Log::trace()
    << "entering TotalWaterMixingRatioWrtWetAir_A::executeNL function"
    << std::endl;
  mo::eval_total_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl(afieldset);
  oops::Log::trace()
    << "leaving TotalWaterMixingRatioWrtWetAir_A::executeNL function"
    << std::endl;
}

}  // namespace vader
