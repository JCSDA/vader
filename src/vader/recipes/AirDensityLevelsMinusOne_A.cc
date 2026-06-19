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
#include "mo/eval_air_density.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirDensityLevelsMinusOne.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirDensityLevelsMinusOne_A::Name[] = "AirDensityLevelsMinusOne_A";
const oops::Variables AirDensityLevelsMinusOne_A::
    Ingredients{std::vector<std::string>{"height_above_mean_sea_level",
                                         "height_above_mean_sea_level_levels",
                                         "virtual_potential_temperature",
                                         "air_pressure_levels_minus_one"}};

// Register the maker
static RecipeMaker<AirDensityLevelsMinusOne_A>
    makerAirDensityLevelsMinusOne_A_(AirDensityLevelsMinusOne_A::Name);

AirDensityLevelsMinusOne_A::AirDensityLevelsMinusOne_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables) :
        configVariables_{configVariables}

{
    oops::Log::trace() <<
        "AirDensityLevelsMinusOne_A::AirDensityLevelsMinusOne_A(params)" << std::endl;
}

std::string AirDensityLevelsMinusOne_A::name() const
{
    return AirDensityLevelsMinusOne_A::Name;
}

oops::Variable AirDensityLevelsMinusOne_A::product() const
{
    return oops::Variable("air_density_levels_minus_one");
}

oops::Variables AirDensityLevelsMinusOne_A::ingredients() const
{
    return AirDensityLevelsMinusOne_A::Ingredients;
}

size_t AirDensityLevelsMinusOne_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["air_pressure_levels_minus_one"].shape(1);
}

atlas::FunctionSpace AirDensityLevelsMinusOne_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset.field("air_pressure_levels_minus_one").functionspace();
}
// -------------------------------------------------------------------------------------------------

void AirDensityLevelsMinusOne_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering AirDensityLevelsMinusOne_A::executeNL function"
      << std::endl;
    mo::eval_air_density_from_pressure_levels_minus_one_nl(afieldset);
    oops::Log::trace() << "leaving AirDensityLevelsMinusOne_A::executeNL function" << std::endl;
}

}  // namespace vader
