/*
 * (C) Crown Copyright 2025 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field.h"
#include "mo/eval_cloud_liquid_water_mixing_ratio.h"
#include "mo/functions.h"
#include "oops/util/Logger.h"
#include "vader/recipes/CloudLiquidWaterMixingRatioWrtDryAir.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char CloudLiquidWaterMixingRatioWrtDryAir_A::Name[] =
                        "CloudLiquidWaterMixingRatioWrtDryAir_A";
const oops::Variables CloudLiquidWaterMixingRatioWrtDryAir_A::
    Ingredients{std::vector<std::string>{
      "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"
      }};

// Register the maker
static RecipeMaker<CloudLiquidWaterMixingRatioWrtDryAir_A> makerTotalWater_A_(
                         CloudLiquidWaterMixingRatioWrtDryAir_A::Name);

CloudLiquidWaterMixingRatioWrtDryAir_A::
        CloudLiquidWaterMixingRatioWrtDryAir_A(const Parameters_ & params,
                                         const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "CloudLiquidWaterMixingRatioWrtDryAir_A::"
        << "CloudLiquidWaterMixingRatioWrtDryAir_A(params, configVariables)"
        << std::endl;
}

std::string CloudLiquidWaterMixingRatioWrtDryAir_A::name() const
{
    return CloudLiquidWaterMixingRatioWrtDryAir_A::Name;
}

oops::Variable CloudLiquidWaterMixingRatioWrtDryAir_A::product() const
{
    return oops::Variable{"cloud_liquid_water_mixing_ratio_wrt_dry_air"};
}

oops::Variables CloudLiquidWaterMixingRatioWrtDryAir_A::ingredients() const
{
    return CloudLiquidWaterMixingRatioWrtDryAir_A::Ingredients;
}

oops::Variables CloudLiquidWaterMixingRatioWrtDryAir_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "total_water_mixing_ratio_wrt_dry_air",
        "cloud_liquid_water_mixing_ratio_wrt_dry_air"}};
}

size_t CloudLiquidWaterMixingRatioWrtDryAir_A::productLevels(
    const atlas::FieldSet & afieldset) const
{
    return (afieldset
        ["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"].shape(1));
}

atlas::FunctionSpace CloudLiquidWaterMixingRatioWrtDryAir_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
    return (afieldset
        ["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"].functionspace());
}

// -------------------------------------------------------------------------------------------------

void CloudLiquidWaterMixingRatioWrtDryAir_A::executeTL(atlas::FieldSet & afieldsetTL,
    const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering CloudLiquidWaterMixingRatioWrtDryAir_A::executeTL function"
        << std::endl;
    mo::eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_inv_tl(
        afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "leaving CloudLiquidWaterMixingRatioWrtDryAir_A::executeTL function"
        << std::endl;
}

// -------------------------------------------------------------------------------------------------

void CloudLiquidWaterMixingRatioWrtDryAir_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << afieldsetAD.field_names()
        << afieldsetTraj.field_names()
        << "entering CloudLiquidWaterMixingRatioWrtDryAir::executeAD function"
        << std::endl;
    mo::eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_inv_ad(
        afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "leaving CloudLiquidWaterMixingRatioWrtDryAir_A::executeAD function"
        << std::endl;
}

}  // namespace vader
