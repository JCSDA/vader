/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "mo/constants.h"

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirTemperature.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirTemperature_C::Name[] = "AirTemperature_C";
const char AT[] = "air_temperature";
const char APT[] = "air_potential_temperature";
const char AP[] = "air_pressure";
const oops::Variables AirTemperature_C::Ingredients{std::vector<std::string>{APT, AP}};

// Register the maker
static RecipeMaker<AirTemperature_C> makerAirTemperature_C_(AirTemperature_C::Name);

AirTemperature_C::AirTemperature_C(const Parameters_ & params,
                                   const VaderConfigVars & configVariables):
                                   configVariables_{configVariables}
{
    oops::Log::trace() << "AirTemperature_C::AirTemperature_C(params)" << std::endl;
}

std::string AirTemperature_C::name() const
{
    return AirTemperature_C::Name;
}

oops::Variable AirTemperature_C::product() const
{
    return oops::Variable("air_temperature");
}

oops::Variables AirTemperature_C::ingredients() const
{
    return AirTemperature_C::Ingredients;
}

size_t AirTemperature_C::productLevels(const atlas::FieldSet & fields) const
{
    return fields[APT].levels();
}

atlas::FunctionSpace AirTemperature_C::productFunctionSpace(const atlas::FieldSet & fields) const
{
    return fields[APT].functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirTemperature_C::executeNL(atlas::FieldSet & fields)
{
    oops::Log::trace() << "entering AirTemperature_C::executeNL function" << std::endl;

    util::for_each_value(
        [](const double apt,
           const double ap,
           double& temp) {
           temp = apt * std::pow(ap / mo::constants::p_zero, mo::constants::rd_over_cp);
        },
        fields[APT],
        fields[AP],
        fields[AT]);

    oops::Log::trace() << "leaving AirTemperature_C::executeNL function" << std::endl;
}

}  // namespace vader
