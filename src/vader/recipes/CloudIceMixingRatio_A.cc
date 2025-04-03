/*
 * (C) Crown Copyright 2023 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field.h"
#include "mo/eval_cloud_ice_mixing_ratio.h"
#include "oops/util/Logger.h"
#include "vader/recipes/CloudIceMixingRatio.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char CloudIceMixingRatio_A::Name[] = "CloudIceMixingRatio_A";
const oops::Variables CloudIceMixingRatio_A::
    Ingredients{std::vector<std::string>{"cloud_ice_mixing_ratio_wrt_dry_air",
                                         "total_water_mixing_ratio_wrt_dry_air"}};

// Register the maker
static RecipeMaker<CloudIceMixingRatio_A> makerTotalWater_A_(CloudIceMixingRatio_A::Name);

CloudIceMixingRatio_A::CloudIceMixingRatio_A(const Parameters_ & params,
                                             const VaderConfigVars & configVariables)
{
    oops::Log::trace() <<
        "CloudIceMixingRatio_A::CloudIceMixingRatio_A(params, configVariables)" << std::endl;
}

std::string CloudIceMixingRatio_A::name() const
{
    return CloudIceMixingRatio_A::Name;
}

oops::Variable CloudIceMixingRatio_A::product() const
{
    return oops::Variable{"cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"};
}

oops::Variables CloudIceMixingRatio_A::ingredients() const
{
    return CloudIceMixingRatio_A::Ingredients;
}

oops::Variables CloudIceMixingRatio_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"cloud_ice_mixing_ratio_wrt_dry_air",
                                                    "total_water_mixing_ratio_wrt_dry_air"}};
}

size_t CloudIceMixingRatio_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return (afieldset["total_water_mixing_ratio_wrt_dry_air"].shape(1));
}

atlas::FunctionSpace CloudIceMixingRatio_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["total_water_mixing_ratio_wrt_dry_air"].functionspace();
}

void CloudIceMixingRatio_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering CloudIceMixingRatio_A::executeNL function"
      << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water_nl(afieldset);
    oops::Log::trace() << "leaving CloudIceMixingRatio_A::executeNL function" << std::endl;
}

void CloudIceMixingRatio_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering CloudIceMixingRatio_A::executeTL function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water_tl(afieldsetTL,
                                                                         afieldsetTraj);
    oops::Log::trace() << "leaving CloudIceMixingRatio_A::executeTL function" << std::endl;
}

void CloudIceMixingRatio_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering CloudIceMixingRatio::executeAD function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water_ad(afieldsetAD,
                                                                         afieldsetTraj);
    oops::Log::trace() << "leaving CloudIceMixingRatio_A::executeAD function" << std::endl;
}

}  // namespace vader
