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
#include "vader/recipes/WaterVaporMixingRatioWrtWetAir2m.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WaterVaporMixingRatioWrtWetAir2m_A::Name[] =
        "WaterVaporMixingRatioWrtWetAir2m_A";
const oops::Variables WaterVaporMixingRatioWrtWetAir2m_A::
        Ingredients{std::vector<std::string>{
                                    "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"}};

// Register the maker
static RecipeMaker<WaterVaporMixingRatioWrtWetAir2m_A> makerTotalWater_A_(
        WaterVaporMixingRatioWrtWetAir2m_A::Name);

WaterVaporMixingRatioWrtWetAir2m_A::
        WaterVaporMixingRatioWrtWetAir2m_A(const Parameters_ & params,
                                                        const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "WaterVaporMixingRatioWrtWetAir2m_A::"
        << "WaterVaporMixingRatioWrtWetAir2m_A(params, configVariables)"
        << std::endl;
}

std::string WaterVaporMixingRatioWrtWetAir2m_A::name() const
{
    return WaterVaporMixingRatioWrtWetAir2m_A::Name;
}

oops::Variable WaterVaporMixingRatioWrtWetAir2m_A::product() const
{
    return oops::Variable{"water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m"};
}

oops::Variables WaterVaporMixingRatioWrtWetAir2m_A::ingredients() const
{
    return WaterVaporMixingRatioWrtWetAir2m_A::Ingredients;
}

oops::Variables WaterVaporMixingRatioWrtWetAir2m_A::trajectoryVars() const
{
    return oops::Variables{};
}

size_t WaterVaporMixingRatioWrtWetAir2m_A::productLevels(
                                            const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace WaterVaporMixingRatioWrtWetAir2m_A::productFunctionSpace(
                                            const atlas::FieldSet
                                            & afieldset) const
{
    return afieldset["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"].functionspace();
}
// -------------------------------------------------------------------------------------------------

void WaterVaporMixingRatioWrtWetAir2m_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtWetAir2m_A::executeNL function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_nl(afieldset);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtWetAir2m_A::executeNL function"
        << std::endl;
}
// -------------------------------------------------------------------------------------------------

void WaterVaporMixingRatioWrtWetAir2m_A::executeTL(atlas::FieldSet & afieldsetTL,
                                     const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtWetAir2m_A::executeTL function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_tl(afieldsetTL,
                                                                                 afieldsetTraj);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtWetAir2m_A::executeTL function"
        << std::endl;
}
// -------------------------------------------------------------------------------------------------

void WaterVaporMixingRatioWrtWetAir2m_A::executeAD(atlas::FieldSet & afieldsetAD,
                                     const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtWetAir2m::executeAD function"
        << std::endl;
    mo::eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_ad(afieldsetAD,
                                                                                 afieldsetTraj);
    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtWetAir2m_A::executeAD function"
         << std::endl;
}

}  // namespace vader
