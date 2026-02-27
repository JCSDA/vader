/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/WaterVaporMixingRatioWrtMoistAir.h"

namespace vader
{

// ------------------------------------------------------------------------------------------------
// Static attribute initialization
const char WaterVaporMixingRatioWrtMoistAir_A::Name[] = "WaterVaporMixingRatioWrtMoistAir_A";

// -------------------------------------------------------------------------------------------------

const oops::Variables WaterVaporMixingRatioWrtMoistAir_A::Ingredients{
                      {"water_vapor_mixing_ratio_wrt_dry_air"}};


// -------------------------------------------------------------------------------------------------
// Register the maker
static RecipeMaker<WaterVaporMixingRatioWrtMoistAir_A> makerWaterVaporMixingRatioWrtMoistAir_(
                   WaterVaporMixingRatioWrtMoistAir_A::Name);

// -------------------------------------------------------------------------------------------------

WaterVaporMixingRatioWrtMoistAir_A::WaterVaporMixingRatioWrtMoistAir_A(const Parameters_ & params,
                                       const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace()
          << "WaterVaporMixingRatioWrtMoistAir_A::WaterVaporMixingRatioWrtMoistAir_A(params)"
          << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string WaterVaporMixingRatioWrtMoistAir_A::name() const
{
    return WaterVaporMixingRatioWrtMoistAir_A::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable WaterVaporMixingRatioWrtMoistAir_A::product() const
{
    return oops::Variable{"water_vapor_mixing_ratio_wrt_moist_air"};
}

// -------------------------------------------------------------------------------------------------

oops::Variables WaterVaporMixingRatioWrtMoistAir_A::ingredients() const
{
    return WaterVaporMixingRatioWrtMoistAir_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

oops::Variables WaterVaporMixingRatioWrtMoistAir_A::trajectoryVars() const {
    return oops::Variables{std::vector<std::string>{"water_vapor_mixing_ratio_wrt_dry_air"}};
}

// -------------------------------------------------------------------------------------------------

size_t WaterVaporMixingRatioWrtMoistAir_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("water_vapor_mixing_ratio_wrt_dry_air").shape(1);
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace WaterVaporMixingRatioWrtMoistAir_A::productFunctionSpace
                                              (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("water_vapor_mixing_ratio_wrt_dry_air").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WaterVaporMixingRatioWrtMoistAir_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
          << "entering WaterVaporMixingRatioWrtMoistAir_A::executeNL function"
          << std::endl;

    // water_vapor_mixing_ratio_wrt_dry_air in kg/kg;
    // water_vapor_mixing_ratio_wrt_moist_air in kg/kg;
    util::for_each_value(
        [&](const double mixr,
            double& q) {
            q = mixr / (1. + mixr);
        },
        afieldset["water_vapor_mixing_ratio_wrt_dry_air"],
        afieldset["water_vapor_mixing_ratio_wrt_moist_air"]);

    oops::Log::trace()
          << "leaving WaterVaporMixingRatioWrtMoistAir_A::executeNL function"
          << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WaterVaporMixingRatioWrtMoistAir_A::executeTL(atlas::FieldSet & afieldsetTL,
    const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering WaterVaporMixingRatioWrtMoistAir_A::executeTL function"
        << std::endl;

    util::for_each_value(
        [&](const double mixr,
            const double mixr_tl,
            double& q_tl) {
            const double fac = 1.0 / ((1.0 + mixr) * (1.0 + mixr));
            q_tl = fac * mixr_tl;
        },
        afieldsetTraj["water_vapor_mixing_ratio_wrt_dry_air"],
        afieldsetTL["water_vapor_mixing_ratio_wrt_dry_air"],
        afieldsetTL["water_vapor_mixing_ratio_wrt_moist_air"]);

    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtMoistAir_A::executeTL function"
        << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WaterVaporMixingRatioWrtMoistAir_A::executeAD(atlas::FieldSet & afieldsetAD,
    const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
          << "entering WaterVaporMixingRatioWrtMoistAir_A::executeAD function"
          << std::endl;

    util::for_each_value(
        [&](const double mixr,
            double& q_ad,
            double& mixr_ad) {
            const double fac = 1.0 / ((1.0 + mixr) * (1.0 + mixr));
            mixr_ad += fac * q_ad;
            q_ad = 0.0;
        },
        afieldsetTraj["water_vapor_mixing_ratio_wrt_dry_air"],
        afieldsetAD["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetAD["water_vapor_mixing_ratio_wrt_dry_air"]);

    oops::Log::trace()
        << "leaving WaterVaporMixingRatioWrtMoistAir_A::executeAD function"
        << std::endl;
}

}  // namespace vader
