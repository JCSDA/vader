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
#include "oops/util/Logger.h"
#include "vader/recipes/WaterVaporMixingRatioWrtWetAir.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WaterVaporMixingRatioWrtWetAir_A::Name[] =
                        "WaterVaporMixingRatioWrtWetAir_A";
const oops::Variables WaterVaporMixingRatioWrtWetAir_A::
    Ingredients{std::vector<std::string>{"water_vapor_mixing_ratio_wrt_dry_air",
                                         "total_water_mixing_ratio_wrt_dry_air"}};

// Register the maker
static RecipeMaker<WaterVaporMixingRatioWrtWetAir_A> makerTotalWater_A_(
                         WaterVaporMixingRatioWrtWetAir_A::Name);

WaterVaporMixingRatioWrtWetAir_A::
        WaterVaporMixingRatioWrtWetAir_A(const Parameters_ & params,
                                                            const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "WaterVaporMixingRatioWrtWetAir_A::"
         << "WaterVaporMixingRatioWrtWetAir_A(params, configVariables)"
         << std::endl;
}

std::string WaterVaporMixingRatioWrtWetAir_A::name() const
{
    return WaterVaporMixingRatioWrtWetAir_A::Name;
}

oops::Variable WaterVaporMixingRatioWrtWetAir_A::product() const
{
    return oops::Variable{"water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"};
}

oops::Variables WaterVaporMixingRatioWrtWetAir_A::ingredients() const
{
    return WaterVaporMixingRatioWrtWetAir_A::Ingredients;
}

oops::Variables WaterVaporMixingRatioWrtWetAir_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"water_vapor_mixing_ratio_wrt_dry_air",
                                                    "total_water_mixing_ratio_wrt_dry_air"}};
}

size_t WaterVaporMixingRatioWrtWetAir_A::productLevels(
                                           const atlas::FieldSet & afieldset) const
{
    return (afieldset["total_water_mixing_ratio_wrt_dry_air"].shape(1));
}

atlas::FunctionSpace WaterVaporMixingRatioWrtWetAir_A::productFunctionSpace(
                                           const atlas::FieldSet
                                           & afieldset) const
{
    return afieldset["total_water_mixing_ratio_wrt_dry_air"].functionspace();
}

void WaterVaporMixingRatioWrtWetAir_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtWetAir_A::executeNL function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl(afieldset);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtWetAir_A::executeNL function"
        << std::endl;
}

void WaterVaporMixingRatioWrtWetAir_A::executeTL(atlas::FieldSet & afieldsetTL,
                                   const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtWetAir_A::executeTL function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_tl(afieldsetTL,
                                                                           afieldsetTraj);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtWetAir_A::executeTL function"
        << std::endl;
}

void WaterVaporMixingRatioWrtWetAir_A::executeAD(atlas::FieldSet & afieldsetAD,
                                   const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtWetAir::executeAD function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_ad(afieldsetAD,
                                                                           afieldsetTraj);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtWetAir_A::executeAD function"
        << std::endl;
}

}  // namespace vader
