/*
 * (C) Crown Copyright 2025 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field.h"
#include "mo/eval_water_vapor_mixing_ratio.h"
#include "mo/functions.h"
#include "oops/util/Logger.h"
#include "vader/recipes/WaterVaporMixingRatioWrtDryAir.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WaterVaporMixingRatioWrtDryAir_A::Name[] =
                        "WaterVaporMixingRatioWrtDryAir_A";
const oops::Variables WaterVaporMixingRatioWrtDryAir_A::
    Ingredients{std::vector<std::string>{
      "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"
      }};

// Register the maker
static RecipeMaker<WaterVaporMixingRatioWrtDryAir_A> makerTotalWater_A_(
                         WaterVaporMixingRatioWrtDryAir_A::Name);

WaterVaporMixingRatioWrtDryAir_A::
        WaterVaporMixingRatioWrtDryAir_A(const Parameters_ & params,
                                         const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "WaterVaporMixingRatioWrtDryAir_A::"
         << "WaterVaporMixingRatioWrtDryAir_A(params, configVariables)"
         << std::endl;
}

std::string WaterVaporMixingRatioWrtDryAir_A::name() const
{
    return WaterVaporMixingRatioWrtDryAir_A::Name;
}

oops::Variable WaterVaporMixingRatioWrtDryAir_A::product() const
{
    return oops::Variable{"water_vapor_mixing_ratio_wrt_dry_air"};
}

oops::Variables WaterVaporMixingRatioWrtDryAir_A::ingredients() const
{
    return WaterVaporMixingRatioWrtDryAir_A::Ingredients;
}

oops::Variables WaterVaporMixingRatioWrtDryAir_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "total_water_mixing_ratio_wrt_dry_air",
        "water_vapor_mixing_ratio_wrt_dry_air"}};
}

size_t WaterVaporMixingRatioWrtDryAir_A::productLevels(
    const atlas::FieldSet & afieldset) const
{
    return (afieldset["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"].shape(1));
}

atlas::FunctionSpace WaterVaporMixingRatioWrtDryAir_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
    return afieldset["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"].functionspace();
}
// -------------------------------------------------------------------------------------------------

void WaterVaporMixingRatioWrtDryAir_A::executeTL(atlas::FieldSet & afieldsetTL,
    const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtDryAir_A::executeTL function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_inv_tl(
        afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtDryAir_A::executeTL function"
        << std::endl;
}
// -------------------------------------------------------------------------------------------------

void WaterVaporMixingRatioWrtDryAir_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << afieldsetAD.field_names()
        << afieldsetTraj.field_names()
        << "entering WaterVaporMixingRatioWrtDryAir::executeAD function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_inv_ad(
        afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtDryAir_A::executeAD function"
        << std::endl;
}

}  // namespace vader
