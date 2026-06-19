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
#include "vader/recipes/CloudIceMixingRatioWrtWetAir.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char CloudIceMixingRatioWrtWetAir_A::Name[] = "CloudIceMixingRatioWrtWetAir_A";
const oops::Variables CloudIceMixingRatioWrtWetAir_A::
    Ingredients{std::vector<std::string>{"cloud_ice_mixing_ratio_wrt_dry_air",
                                         "total_water_mixing_ratio_wrt_dry_air"}};

// Register the maker
static RecipeMaker<CloudIceMixingRatioWrtWetAir_A>
    makerTotalWater_A_(CloudIceMixingRatioWrtWetAir_A::Name);

CloudIceMixingRatioWrtWetAir_A::CloudIceMixingRatioWrtWetAir_A(
    const Parameters_ & params, const VaderConfigVars & configVariables)
{
    oops::Log::trace()
        << "CloudIceMixingRatioWrtWetAir_A::CloudIceMixingRatioWrtWetAir_A"
        << "(params, configVariables)"
        << std::endl;
}

std::string CloudIceMixingRatioWrtWetAir_A::name() const
{
    return CloudIceMixingRatioWrtWetAir_A::Name;
}

oops::Variable CloudIceMixingRatioWrtWetAir_A::product() const
{
    return oops::Variable{"cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"};
}

oops::Variables CloudIceMixingRatioWrtWetAir_A::ingredients() const
{
    return CloudIceMixingRatioWrtWetAir_A::Ingredients;
}

oops::Variables CloudIceMixingRatioWrtWetAir_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"cloud_ice_mixing_ratio_wrt_dry_air",
                                                    "total_water_mixing_ratio_wrt_dry_air"}};
}

size_t CloudIceMixingRatioWrtWetAir_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return (afieldset["total_water_mixing_ratio_wrt_dry_air"].shape(1));
}

atlas::FunctionSpace CloudIceMixingRatioWrtWetAir_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
    return afieldset["total_water_mixing_ratio_wrt_dry_air"].functionspace();
}
// -------------------------------------------------------------------------------------------------

void CloudIceMixingRatioWrtWetAir_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering CloudIceMixingRatioWrtWetAir_A::executeNL function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water_nl(afieldset);
    oops::Log::trace() << "leaving CloudIceMixingRatioWrtWetAir_A::executeNL function"
        << std::endl;
}
// -------------------------------------------------------------------------------------------------

void CloudIceMixingRatioWrtWetAir_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering CloudIceMixingRatioWrtWetAir_A::executeTL function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water_tl(afieldsetTL,
                                                                         afieldsetTraj);
    oops::Log::trace() << "leaving CloudIceMixingRatioWrtWetAir_A::executeTL function"
        << std::endl;
}
// -------------------------------------------------------------------------------------------------

void CloudIceMixingRatioWrtWetAir_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering CloudIceMixingRatioWrtWetAir ::executeAD function"
        << std::endl;
    mo::eval_cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water_ad(afieldsetAD,
                                                                         afieldsetTraj);
    oops::Log::trace() << "leaving CloudIceMixingRatioWrtWetAir_A::executeAD function"
        << std::endl;
}

}  // namespace vader
