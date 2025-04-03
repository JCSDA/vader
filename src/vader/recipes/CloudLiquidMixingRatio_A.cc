/*
 * (C) Crown Copyright 2023 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field.h"
#include "mo/eval_cloud_liquid_mixing_ratio.h"
#include "oops/util/Logger.h"
#include "vader/recipes/CloudLiquidMixingRatio.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char CloudLiquidMixingRatio_A::Name[] = "CloudLiquidMixingRatio_A";
const oops::Variables CloudLiquidMixingRatio_A::
    Ingredients{std::vector<std::string>{"cloud_liquid_water_mixing_ratio_wrt_dry_air",
                                         "total_water_mixing_ratio_wrt_dry_air"}};

// Register the maker
static RecipeMaker<CloudLiquidMixingRatio_A> makerTotalWater_A_(CloudLiquidMixingRatio_A::Name);

CloudLiquidMixingRatio_A::CloudLiquidMixingRatio_A(const Parameters_ & params,
                                             const VaderConfigVars & configVariables)
{
    oops::Log::trace() <<
        "CloudLiquidMixingRatio_A::CloudLiquidMixingRatio_A(params, configVariables)" << std::endl;
}

std::string CloudLiquidMixingRatio_A::name() const
{
    return CloudLiquidMixingRatio_A::Name;
}

oops::Variable CloudLiquidMixingRatio_A::product() const
{
    return oops::Variable{"cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"};
}

oops::Variables CloudLiquidMixingRatio_A::ingredients() const
{
    return CloudLiquidMixingRatio_A::Ingredients;
}

oops::Variables CloudLiquidMixingRatio_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"cloud_liquid_water_mixing_ratio_wrt_dry_air",
                                                    "total_water_mixing_ratio_wrt_dry_air"}};
}

size_t CloudLiquidMixingRatio_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return (afieldset["total_water_mixing_ratio_wrt_dry_air"].shape(1));
}

atlas::FunctionSpace CloudLiquidMixingRatio_A::productFunctionSpace(const atlas::FieldSet
                                                                    & afieldset) const
{
    return afieldset["total_water_mixing_ratio_wrt_dry_air"].functionspace();
}

void CloudLiquidMixingRatio_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering CloudLiquidMixingRatio_A::executeNL function"
      << std::endl;
    mo::eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl(afieldset);
    oops::Log::trace() << "leaving CloudLiquidMixingRatio_A::executeNL function" << std::endl;
}

void CloudLiquidMixingRatio_A::executeTL(atlas::FieldSet & afieldsetTL,
                                         const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering CloudLiquidMixingRatio_A::executeTL function"
        << std::endl;
    mo::eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_tl(afieldsetTL,
                                                                                  afieldsetTraj);
    oops::Log::trace() << "leaving CloudLiquidMixingRatio_A::executeTL function" << std::endl;
}

void CloudLiquidMixingRatio_A::executeAD(atlas::FieldSet & afieldsetAD,
                                         const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering CloudLiquidMixingRatio::executeAD function"
        << std::endl;
    mo::eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_ad(afieldsetAD,
                                                                                  afieldsetTraj);
    oops::Log::trace() << "leaving CloudLiquidMixingRatio_A::executeAD function" << std::endl;
}

}  // namespace vader
