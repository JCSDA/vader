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
const char CloudIceMixingRatioWrtDryAir_A::Name[] =
                        "CloudIceMixingRatioWrtDryAir_A";
const oops::Variables CloudIceMixingRatioWrtDryAir_A::
    Ingredients{std::vector<std::string>{
        "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"
      }};

// Register the maker
static RecipeMaker<CloudIceMixingRatioWrtDryAir_A> makerTotalWater_A_(
                         CloudIceMixingRatioWrtDryAir_A::Name);

CloudIceMixingRatioWrtDryAir_A::CloudIceMixingRatioWrtDryAir_A(
    const Parameters_ & params, const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "CloudIceMixingRatioWrtDryAir_A::"
        << "CloudIceMixingRatioWrtDryAir_A(params, configVariables)"
        << std::endl;
}

std::string CloudIceMixingRatioWrtDryAir_A::name() const
{
    return CloudIceMixingRatioWrtDryAir_A::Name;
}

oops::Variable CloudIceMixingRatioWrtDryAir_A::product() const
{
    return oops::Variable{"cloud_ice_mixing_ratio_wrt_dry_air"};
}

oops::Variables CloudIceMixingRatioWrtDryAir_A::ingredients() const
{
    return CloudIceMixingRatioWrtDryAir_A::Ingredients;
}

oops::Variables CloudIceMixingRatioWrtDryAir_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "total_water_mixing_ratio_wrt_dry_air",
        "cloud_ice_mixing_ratio_wrt_dry_air"}};
}

size_t CloudIceMixingRatioWrtDryAir_A::productLevels(
    const atlas::FieldSet & afieldset) const
{
    return (afieldset["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"].shape(1));
}

atlas::FunctionSpace CloudIceMixingRatioWrtDryAir_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
    return afieldset["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"].functionspace();
}

void CloudIceMixingRatioWrtDryAir_A::executeTL(atlas::FieldSet & afieldsetTL,
    const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering CloudIceMixingRatioWrtDryAir_A::executeTL function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water_inv_tl(
        afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "leaving CloudIceMixingRatioWrtDryAir_A::executeTL function"
        << std::endl;
}

void CloudIceMixingRatioWrtDryAir_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << afieldsetAD.field_names()
        << afieldsetTraj.field_names()
        << "entering CloudIceMixingRatioWrtDryAir::executeAD function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water_inv_ad(
        afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "leaving CloudIceMixingRatioWrtDryAir_A::executeAD function"
        << std::endl;
}

}  // namespace vader
