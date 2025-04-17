/*
 * (C) Crown Copyright 2023-2025 Met Office.
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
const char CloudLiquidWaterMixingRatioWrtDryAir_B::Name[] =
                        "CloudLiquidWaterMixingRatioWrtDryAir_B";

// Note that we require "total_water_mixing_ratio_wrt_moist_air_and_condensed_water"
// for executeNL but not for executeTL and executeAD
const oops::Variables CloudLiquidWaterMixingRatioWrtDryAir_B::
    Ingredients{std::vector<std::string>{
      "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
      "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"
      }};

// Register the maker
static RecipeMaker<CloudLiquidWaterMixingRatioWrtDryAir_B> makerTotalWater_A_(
                         CloudLiquidWaterMixingRatioWrtDryAir_B::Name);

CloudLiquidWaterMixingRatioWrtDryAir_B::
        CloudLiquidWaterMixingRatioWrtDryAir_B(const Parameters_ & params,
                                         const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "CloudLiquidWaterMixingRatioWrtDryAir_B::"
         << "CloudLiquidWaterMixingRatioWrtDryAir_B(params, configVariables)"
         << std::endl;
}

std::string CloudLiquidWaterMixingRatioWrtDryAir_B::name() const
{
    return CloudLiquidWaterMixingRatioWrtDryAir_B::Name;
}

oops::Variable CloudLiquidWaterMixingRatioWrtDryAir_B::product() const
{
    return oops::Variable{"cloud_liquid_water_mixing_ratio_wrt_dry_air"};
}

oops::Variables CloudLiquidWaterMixingRatioWrtDryAir_B::ingredients() const
{
    return CloudLiquidWaterMixingRatioWrtDryAir_B::Ingredients;
}

oops::Variables CloudLiquidWaterMixingRatioWrtDryAir_B::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
        "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"}};
}

size_t CloudLiquidWaterMixingRatioWrtDryAir_B::productLevels(
    const atlas::FieldSet & afieldset) const
{
    return (afieldset
        ["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"].shape(1));
}

atlas::FunctionSpace CloudLiquidWaterMixingRatioWrtDryAir_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
    return afieldset
        ["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"].functionspace();
}

void CloudLiquidWaterMixingRatioWrtDryAir_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
        << "entering CloudLiquidWaterMixingRatioWrtDryAir_B::executeNL function"
        << std::endl;
    mo::eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_nl(afieldset);
    oops::Log::trace()
        << "exiting CloudLiquidWaterMixingRatioWrtDryAir_B::executeNL function"
        << std::endl;
}

void CloudLiquidWaterMixingRatioWrtDryAir_B::executeTL(atlas::FieldSet & afieldsetTL,
    const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering CloudLiquidWaterMixingRatioWrtDryAir_B::executeTL function"
        << std::endl;
    mo::eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "exiting CloudLiquidWaterMixingRatioWrtDryAir_B::executeTL function"
        << std::endl;
}

void CloudLiquidWaterMixingRatioWrtDryAir_B::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering CloudLiquidWaterMixingRatioWrtDryAir_B::executeAD function"
        << std::endl;
    mo::eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "exiting CloudLiquidWaterMixingRatioWrtDryAir_B::executeAD function"
        << std::endl;
}

}  // namespace vader
