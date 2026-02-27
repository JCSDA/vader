/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/WaterVaporMixingRatioWrtDryAir.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WaterVaporMixingRatioWrtDryAir_C::Name[] = "WaterVaporMixingRatioWrtDryAir_C";
const oops::Variables WaterVaporMixingRatioWrtDryAir_C::Ingredients{
                      {"water_vapor_mixing_ratio_wrt_moist_air"}};

// Register the maker
static RecipeMaker<WaterVaporMixingRatioWrtDryAir_C> makerWaterVaporMixingRatioWrtDryAir_(
                   WaterVaporMixingRatioWrtDryAir_C::Name);

WaterVaporMixingRatioWrtDryAir_C::WaterVaporMixingRatioWrtDryAir_C(const Parameters_ & params,
                                       const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace()
          << "WaterVaporMixingRatioWrtDryAir_C::WaterVaporMixingRatioWrtDryAir_C(params)"
          << std::endl;
}

std::string WaterVaporMixingRatioWrtDryAir_C::name() const
{
    return WaterVaporMixingRatioWrtDryAir_C::Name;
}

oops::Variable WaterVaporMixingRatioWrtDryAir_C::product() const
{
    return oops::Variable{"water_vapor_mixing_ratio_wrt_dry_air"};
}

oops::Variables WaterVaporMixingRatioWrtDryAir_C::ingredients() const
{
    return WaterVaporMixingRatioWrtDryAir_C::Ingredients;
}

size_t WaterVaporMixingRatioWrtDryAir_C::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("water_vapor_mixing_ratio_wrt_moist_air").shape(1);
}

atlas::FunctionSpace WaterVaporMixingRatioWrtDryAir_C::productFunctionSpace
                                              (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("water_vapor_mixing_ratio_wrt_moist_air").functionspace();
}

void WaterVaporMixingRatioWrtDryAir_C::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
          << "entering WaterVaporMixingRatioWrtDryAir_C::executeNL function"
          << std::endl;

    util::for_each_value(
        [](const double q,
           double& mixr) {
           mixr = q / (1. - q);
        },
        afieldset["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldset["water_vapor_mixing_ratio_wrt_dry_air"]);

    oops::Log::trace()
          << "leaving WaterVaporMixingRatioWrtDryAir_C::executeNL function"
          << std::endl;
}

}  // namespace vader
