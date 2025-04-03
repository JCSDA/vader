/*
 * (C) Copyright 2022 UCAR.
 * (C) Crown Copyright 2023-2024 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "mo/eval_dry_air_density.h"
#include "oops/util/Logger.h"
#include "vader/recipes/DryAirDensityLevelsMinusOne.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DryAirDensityLevelsMinusOne_A::Name[] = "DryAirDensityLevelsMinusOne_A";
const oops::Variables DryAirDensityLevelsMinusOne_A::
    Ingredients{std::vector<std::string>{"height_above_mean_sea_level",
                   "height_above_mean_sea_level_levels", "air_potential_temperature",
                   "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
                   "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water",
                   "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water",
                   "air_pressure_levels_minus_one"}};
    // For renaming to CCPP standard in (hopefully) near future:
    // Ingredients = {"height_above_mean_sea_level_at_interface",
    //                "height_above_mean_sea_levels_extended_up_by_one",
    //                "air_potential_temperature_at_interface",
    //                "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    //                "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water",
    //                "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water",
    //                "air_pressure"}

// Register the maker
static RecipeMaker<DryAirDensityLevelsMinusOne_A>
    makerDryAirDensityLevelsMinusOne_A_(DryAirDensityLevelsMinusOne_A::Name);

DryAirDensityLevelsMinusOne_A::DryAirDensityLevelsMinusOne_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables) :
        configVariables_{configVariables}

{
    oops::Log::trace() <<
        "DryAirDensityLevelsMinusOne_A::DryAirDensityLevelsMinusOne_A(params)" << std::endl;
}

std::string DryAirDensityLevelsMinusOne_A::name() const
{
    return DryAirDensityLevelsMinusOne_A::Name;
}

oops::Variable DryAirDensityLevelsMinusOne_A::product() const
{
    return oops::Variable{"dry_air_density_levels_minus_one"};
    // For renaming to CCPP standard in (hopefully) near future:
    // return "dry_air_density";
}

oops::Variables DryAirDensityLevelsMinusOne_A::ingredients() const
{
    return DryAirDensityLevelsMinusOne_A::Ingredients;
}

oops::Variables DryAirDensityLevelsMinusOne_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"height_above_mean_sea_level",
                   "height_above_mean_sea_level_levels",
                   "air_potential_temperature",
                   "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
                   "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water",
                   "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water",
                   "air_pressure_levels_minus_one",
                   "dry_air_density_levels_minus_one"}};
}

size_t DryAirDensityLevelsMinusOne_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["air_pressure_levels_minus_one"].shape(1);
    // For renaming to CCPP standard in (hopefully) near future:
    //  return afieldset["air_pressure"].shape(1);
}

atlas::FunctionSpace DryAirDensityLevelsMinusOne_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset.field("air_pressure_levels_minus_one").functionspace();
}

void DryAirDensityLevelsMinusOne_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering DryAirDensityLevelsMinusOne_A::executeNL function"
      << std::endl;
    mo::eval_dry_air_density_from_pressure_levels_minus_one_nl(afieldset);
    oops::Log::trace() << "leaving DryAirDensityLevelsMinusOne_A::executeNL function" << std::endl;
}

void DryAirDensityLevelsMinusOne_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensityLevelsMinusOne_A::executeTL function"
        << std::endl;
    mo::eval_dry_air_density_from_pressure_levels_minus_one_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace() << "leaving DryAirDensityLevelsMinusOne_A::executeTL function" << std::endl;
}

void DryAirDensityLevelsMinusOne_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensityLevelsMinusOne::executeAD function"
        << std::endl;
    mo::eval_dry_air_density_from_pressure_levels_minus_one_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace() << "leaving DryAirDensityLevelsMinusOne_A::executeAD function" << std::endl;
}

}  // namespace vader
