/*
 * (C) Crown Copyright 2025 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field.h"
#include "mo/eval_cloud_ice_mixing_ratio.h"
#include "mo/functions.h"
#include "oops/util/Logger.h"
#include "vader/recipes/CloudIceMixingRatioWrtDryAir.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char CloudIceMixingRatioWrtDryAir_B::Name[] =
                        "CloudIceMixingRatioWrtDryAir_B";

// Note that we require "total_water_mixing_ratio_wrt_moist_air_and_condensed_water"
// for executeNL but not for executeTL and executeAD
const oops::Variables CloudIceMixingRatioWrtDryAir_B::
    Ingredients{std::vector<std::string>{
      "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water",
      "total_water_mixing_ratio_wrt_moist_air_and_condensed_water"
      }};

// Register the maker
static RecipeMaker<CloudIceMixingRatioWrtDryAir_B> makerTotalWater_A_(
                         CloudIceMixingRatioWrtDryAir_B::Name);

CloudIceMixingRatioWrtDryAir_B::
        CloudIceMixingRatioWrtDryAir_B(const Parameters_ & params,
                                         const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "CloudIceMixingRatioWrtDryAir_B::"
         << "CloudIceMixingRatioWrtDryAir_B(params, configVariables)"
         << std::endl;
}

std::string CloudIceMixingRatioWrtDryAir_B::name() const
{
    return CloudIceMixingRatioWrtDryAir_B::Name;
}

oops::Variable CloudIceMixingRatioWrtDryAir_B::product() const
{
    return oops::Variable{"cloud_ice_mixing_ratio_wrt_dry_air"};
}

oops::Variables CloudIceMixingRatioWrtDryAir_B::ingredients() const
{
    return CloudIceMixingRatioWrtDryAir_B::Ingredients;
}

oops::Variables CloudIceMixingRatioWrtDryAir_B::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water",
        "total_water_mixing_ratio_wrt_moist_air_and_condensed_water"
       }};
}

size_t CloudIceMixingRatioWrtDryAir_B::productLevels(
    const atlas::FieldSet & afieldset) const
{
    return (afieldset["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"].shape(1));
}

atlas::FunctionSpace CloudIceMixingRatioWrtDryAir_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
    return afieldset["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"].functionspace();
}
// -------------------------------------------------------------------------------------------------

void CloudIceMixingRatioWrtDryAir_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
        << "entering CloudIceMixingRatioWrtDryAir_B::executeNL function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_dry_air_nl(afieldset);
    oops::Log::trace()
        << "exiting CloudIceMixingRatioWrtDryAir_B::executeNL function"
        << std::endl;
}
// -------------------------------------------------------------------------------------------------

void CloudIceMixingRatioWrtDryAir_B::executeTL(atlas::FieldSet & afieldsetTL,
    const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering CloudIceMixingRatioWrtDryAir_B::executeTL function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_dry_air_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "exiting CloudIceMixingRatioWrtDryAir_B::executeTL function"
        << std::endl;
}
// -------------------------------------------------------------------------------------------------

void CloudIceMixingRatioWrtDryAir_B::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering CloudIceMixingRatioWrtDryAir_B::executeAD function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_dry_air_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "exiting CloudIceMixingRatioWrtDryAir_B::executeAD function"
        << std::endl;
}

}  // namespace vader
