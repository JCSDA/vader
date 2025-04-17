/*
 * (C) Crown Copyright 2023-2025 Met Office.
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
const char WaterVaporMixingRatioWrtDryAir_B::Name[] =
                        "WaterVaporMixingRatioWrtDryAir_B";

// Note that we require "total_water_mixing_ratio_wrt_moist_air_and_condensed_water"
// for executeNL but not for executeTL and executeAD
const oops::Variables WaterVaporMixingRatioWrtDryAir_B::
    Ingredients{std::vector<std::string>{
      "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
      "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"
      }};

// Register the maker
static RecipeMaker<WaterVaporMixingRatioWrtDryAir_B> makerTotalWater_A_(
                         WaterVaporMixingRatioWrtDryAir_B::Name);

WaterVaporMixingRatioWrtDryAir_B::
        WaterVaporMixingRatioWrtDryAir_B(const Parameters_ & params,
                                         const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "WaterVaporMixingRatioWrtDryAir_B::"
         << "WaterVaporMixingRatioWrtDryAir_B(params, configVariables)"
         << std::endl;
}

std::string WaterVaporMixingRatioWrtDryAir_B::name() const
{
    return WaterVaporMixingRatioWrtDryAir_B::Name;
}

oops::Variable WaterVaporMixingRatioWrtDryAir_B::product() const
{
    return oops::Variable{"water_vapor_mixing_ratio_wrt_dry_air"};
}

oops::Variables WaterVaporMixingRatioWrtDryAir_B::ingredients() const
{
    return WaterVaporMixingRatioWrtDryAir_B::Ingredients;
}

oops::Variables WaterVaporMixingRatioWrtDryAir_B::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
        "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"}};
}

size_t WaterVaporMixingRatioWrtDryAir_B::productLevels(
    const atlas::FieldSet & afieldset) const
{
    return (afieldset["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"].shape(1));
}

atlas::FunctionSpace WaterVaporMixingRatioWrtDryAir_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
    return afieldset["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"].functionspace();
}

void WaterVaporMixingRatioWrtDryAir_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtDryAir_B::executeNL function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_dry_air_nl(afieldset);
    oops::Log::trace()
        << "exiting WaterVaporMixingRatioWrtDryAir_B::executeNL function"
        << std::endl;
}

void WaterVaporMixingRatioWrtDryAir_B::executeTL(atlas::FieldSet & afieldsetTL,
    const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtDryAir_B::executeTL function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_dry_air_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "exiting WaterVaporMixingRatioWrtDryAir_B::executeTL function"
        << std::endl;
}

void WaterVaporMixingRatioWrtDryAir_B::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtDryAir_B::executeAD function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_dry_air_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "exiting WaterVaporMixingRatioWrtDryAir_B::executeAD function"
        << std::endl;
}

}  // namespace vader
