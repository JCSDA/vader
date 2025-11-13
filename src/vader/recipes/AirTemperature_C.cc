/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field.h"

#include "mo/constants.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirTemperature.h"

using atlas::array::make_view;
using atlas::idx_t;

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

void AirTemperature_C::executeNL(atlas::FieldSet & fields)
{
    oops::Log::trace() << "entering AirTemperature_C::executeNL function" << std::endl;

    auto air_pressure_view = make_view<const double, 2>(fields[AP]);
    auto air_potential_temperature_view = make_view<const double, 2>(fields[APT]);
    auto temp_view = make_view<double, 2>(fields[AT]);

    for (idx_t jn = 0; jn < fields[AT].shape(0) ; ++jn) {
      for (idx_t jl = 0; jl < fields[AT].shape(1); ++jl) {
        temp_view(jn, jl) = air_potential_temperature_view(jn, jl) *
                            pow(air_pressure_view(jn, jl) / mo::constants::p_zero,
                            mo::constants::rd_over_cp);
      }
    }
    oops::Log::trace() << "leaving AirTemperature_C::executeNL function" << std::endl;
}

}  // namespace vader
