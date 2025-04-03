/*
 * (C) Crown Copyright 2023 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field.h"
#include "mo/eval_water_vapor_mixing_ratio.h"
#include "oops/util/Logger.h"
#include "vader/recipes/WaterVaporMixingRatioWrtMoistAirAndCondensedWater.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::Name[] =
                        "WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A";
const oops::Variables WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::
    Ingredients{std::vector<std::string>{"water_vapor_mixing_ratio_wrt_dry_air",
                                         "total_water_mixing_ratio_wrt_dry_air"}};

// Register the maker
static RecipeMaker<WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A> makerTotalWater_A_(
                         WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::Name);

WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::
        WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A(const Parameters_ & params,
                                                            const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::"
         << "WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A(params, configVariables)"
         << std::endl;
}

std::string WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::name() const
{
    return WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::Name;
}

oops::Variable WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::product() const
{
    return oops::Variable{"water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"};
}

oops::Variables WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::ingredients() const
{
    return WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::Ingredients;
}

oops::Variables WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"water_vapor_mixing_ratio_wrt_dry_air",
                                                    "total_water_mixing_ratio_wrt_dry_air"}};
}

size_t WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::productLevels(
                                           const atlas::FieldSet & afieldset) const
{
    return (afieldset["total_water_mixing_ratio_wrt_dry_air"].shape(1));
}

atlas::FunctionSpace WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::productFunctionSpace(
                                           const atlas::FieldSet
                                           & afieldset) const
{
    return afieldset["total_water_mixing_ratio_wrt_dry_air"].functionspace();
}

void WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::executeNL function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl(afieldset);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::executeNL function"
        << std::endl;
}

void WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::executeTL(atlas::FieldSet & afieldsetTL,
                                   const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::executeTL function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_tl(afieldsetTL,
                                                                           afieldsetTraj);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::executeTL function"
        << std::endl;
}

void WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::executeAD(atlas::FieldSet & afieldsetAD,
                                   const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtMoistAirAndCondensedWater::executeAD function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_ad(afieldsetAD,
                                                                           afieldsetTraj);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::executeAD function"
        << std::endl;
}

}  // namespace vader
