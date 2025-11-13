/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/field/for_each.h"
#include "atlas/util/Metadata.h"
#include "oops/util/Logger.h"
#include "vader/recipes/WaterVaporMixingRatioWrtMoistAir.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WaterVaporMixingRatioWrtMoistAir_A::Name[] = "WaterVaporMixingRatioWrtMoistAir_A";
const oops::Variables WaterVaporMixingRatioWrtMoistAir_A::Ingredients{
                      {"water_vapor_mixing_ratio_wrt_dry_air"}};

// Register the maker
static RecipeMaker<WaterVaporMixingRatioWrtMoistAir_A> makerWaterVaporMixingRatioWrtMoistAir_(
                   WaterVaporMixingRatioWrtMoistAir_A::Name);

WaterVaporMixingRatioWrtMoistAir_A::WaterVaporMixingRatioWrtMoistAir_A(const Parameters_ & params,
                                       const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace()
          << "WaterVaporMixingRatioWrtMoistAir_A::WaterVaporMixingRatioWrtMoistAir_A(params)"
          << std::endl;
}

std::string WaterVaporMixingRatioWrtMoistAir_A::name() const
{
    return WaterVaporMixingRatioWrtMoistAir_A::Name;
}

oops::Variable WaterVaporMixingRatioWrtMoistAir_A::product() const
{
    return oops::Variable{"water_vapor_mixing_ratio_wrt_moist_air"};
}

oops::Variables WaterVaporMixingRatioWrtMoistAir_A::ingredients() const
{
    return WaterVaporMixingRatioWrtMoistAir_A::Ingredients;
}

size_t WaterVaporMixingRatioWrtMoistAir_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("water_vapor_mixing_ratio_wrt_dry_air").shape(1);
}

atlas::FunctionSpace WaterVaporMixingRatioWrtMoistAir_A::productFunctionSpace
                                              (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("water_vapor_mixing_ratio_wrt_dry_air").functionspace();
}

void WaterVaporMixingRatioWrtMoistAir_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
          << "entering WaterVaporMixingRatioWrtMoistAir_A::executeNL function"
          << std::endl;

    // water_vapor_mixing_ratio_wrt_dry_air in kg/kg;
    // water_vapor_mixing_ratio_wrt_moist_air in kg/kg;
    atlas::field::for_each_value(afieldset["water_vapor_mixing_ratio_wrt_dry_air"],
                                 afieldset["water_vapor_mixing_ratio_wrt_moist_air"],
                                 [&](const double mixr, double& q) {
        q = mixr / (1. + mixr);
    });

    oops::Log::trace()
          << "leaving WaterVaporMixingRatioWrtMoistAir_A::executeNL function"
          << std::endl;
}

}  // namespace vader
