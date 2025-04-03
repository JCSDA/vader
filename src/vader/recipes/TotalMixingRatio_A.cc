/*
 * (C) Crown Copyright 2023 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field.h"
// #include "atlas/util/Metadata.h"
#include "mo/eval_total_mixing_ratio.h"
#include "oops/util/Logger.h"
#include "vader/recipes/TotalMixingRatio.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char TotalMixingRatio_A::Name[] = "TotalMixingRatio_A";
const oops::Variables TotalMixingRatio_A::
    Ingredients{std::vector<std::string>{"water_vapor_mixing_ratio_wrt_dry_air",
                                         "cloud_ice_mixing_ratio_wrt_dry_air",
                                         "cloud_liquid_water_mixing_ratio_wrt_dry_air",
                                         "rain_mixing_ratio_wrt_dry_air"}};

// Register the maker
static RecipeMaker<TotalMixingRatio_A> makerTotalMixingRatio_A_(TotalMixingRatio_A::Name);

TotalMixingRatio_A::TotalMixingRatio_A(const Parameters_ & params,
                           const VaderConfigVars & configVariables)
{
    oops::Log::trace() <<
        "TotalMixingRatio_A::TotalMixingRatio_A(params, configVariables)" << std::endl;
}

std::string TotalMixingRatio_A::name() const
{
    return TotalMixingRatio_A::Name;
}

oops::Variable TotalMixingRatio_A::product() const
{
    return oops::Variable{"total_water_mixing_ratio_wrt_dry_air"};
}

oops::Variables TotalMixingRatio_A::ingredients() const
{
    return TotalMixingRatio_A::Ingredients;
}

oops::Variables TotalMixingRatio_A::trajectoryVars() const
{
    return oops::Variables{};
}

size_t TotalMixingRatio_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return (afieldset["water_vapor_mixing_ratio_wrt_dry_air"].shape(1));
}

atlas::FunctionSpace TotalMixingRatio_A::productFunctionSpace(const atlas::FieldSet
                                                        & afieldset) const
{
    return afieldset["water_vapor_mixing_ratio_wrt_dry_air"].functionspace();
}

void TotalMixingRatio_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering TotalMixingRatio_A::executeNL function"
      << std::endl;
    mo::eval_total_mixing_ratio_nl(afieldset);
    oops::Log::trace() << "leaving TotalMixingRatio_A::executeNL function" << std::endl;
}

void TotalMixingRatio_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering TotalMixingRatio_A::executeTL function"
        << std::endl;
    mo::eval_total_mixing_ratio_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace() << "leaving TotalMixingRatio_A::executeTL function" << std::endl;
}

void TotalMixingRatio_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering TotalMixingRatio::executeAD function"
        << std::endl;
    mo::eval_total_mixing_ratio_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace() << "leaving TotalMixingRatio_A::executeAD function" << std::endl;
}

}  // namespace vader
