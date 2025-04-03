/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field.h"

#include "oops/util/Logger.h"
#include "vader/recipes/AirTemperature.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirTemperature_A::Name[] = "AirTemperature_A";
// CCPP Names for (hopefully near) future:
// const char AT[] = "air_temperature_at_interface";
// const char APT[] = "air_potential_temperature_at_interface";
// const char DEF[] = "dimensionless_exner_function_at_interface";
const char AT[] = "air_temperature";
const char APT[] = "air_potential_temperature";
const char DEF[] = "dimensionless_exner_function";
const oops::Variables AirTemperature_A::Ingredients{std::vector<std::string>{APT, DEF}};

// Register the maker
static RecipeMaker<AirTemperature_A> makerAirTemperature_A_(AirTemperature_A::Name);

AirTemperature_A::AirTemperature_A(const Parameters_ & params,
                                   const VaderConfigVars & configVariables)
{
}

std::string AirTemperature_A::name() const
{
    return AirTemperature_A::Name;
}

oops::Variable AirTemperature_A::product() const
{
    return oops::Variable{"air_temperature"};
    // CCPP Names for (hopefully near) future:
    // return "air_temperature_at_interface"
}

oops::Variables AirTemperature_A::ingredients() const
{
    return AirTemperature_A::Ingredients;
}

oops::Variables AirTemperature_A::trajectoryVars() const
{
    return oops::Variables(std::vector<std::string>{APT, DEF});
}

size_t AirTemperature_A::productLevels(const atlas::FieldSet & fields) const
{
    return fields[APT].shape(1);
}

atlas::FunctionSpace AirTemperature_A::productFunctionSpace(const atlas::FieldSet & fields) const
{
    return fields[APT].functionspace();
}

void AirTemperature_A::executeNL(atlas::FieldSet & fields)
{
    oops::Log::trace() << "entering AirTemperature_A::executeNL function" << std::endl;

    auto exner_view = make_view<const double, 2>(fields[DEF]);
    auto potential_temperature_view = make_view<const double, 2>(fields[APT]);
    auto temp_view = make_view<double, 2>(fields[AT]);

    for (idx_t jn = 0; jn < fields[AT].shape(0) ; ++jn) {
      for (idx_t jl = 0; jl < fields[AT].shape(1); ++jl) {
        temp_view(jn, jl) = potential_temperature_view(jn, jl) *
                            exner_view(jn, jl);
      }
    }
    oops::Log::trace() << "leaving AirTemperature_A::executeNL function" << std::endl;
}

void AirTemperature_A::executeTL(atlas::FieldSet & fields,
                                const atlas::FieldSet & trajFields)
{
    oops::Log::trace() << "entering AirTemperature_A::executeTL function" << std::endl;

    auto exner_traj_view = make_view<const double, 2>(trajFields[DEF]);
    auto potential_temperature_traj_view = make_view<const double, 2>(trajFields[APT]);
    auto exner_view = make_view<const double, 2>(fields[DEF]);
    auto potential_temperature_view = make_view<const double, 2>(fields[APT]);
    auto temp_view = make_view<double, 2>(fields[AT]);

    for (idx_t jn = 0; jn < fields[AT].shape(0) ; ++jn) {
      for (idx_t jl = 0; jl < fields[AT].shape(1); ++jl) {
        temp_view(jn, jl) = potential_temperature_traj_view(jn, jl) *
                            exner_view(jn, jl) +
                            potential_temperature_view(jn, jl) *
                            exner_traj_view(jn, jl);
      }
    }
    oops::Log::trace() << "leaving AirTemperature_A::executeTL function" << std::endl;
}


void AirTemperature_A::executeAD(atlas::FieldSet & fields,
                                 const atlas::FieldSet & trajFields)
{
    oops::Log::trace() << "entering AirTemperature_A::executeAD function" << std::endl;

    auto exner_traj_view = make_view<const double, 2>(trajFields[DEF]);
    auto potential_temperature_traj_view = make_view<const double, 2>(trajFields[APT]);
    auto exner_view = make_view<double, 2>(fields[DEF]);
    auto potential_temperature_view = make_view<double, 2>(fields[APT]);
    auto temp_view = make_view<double, 2>(fields[AT]);

    for (idx_t jn = 0; jn < fields[AT].shape(0) ; ++jn) {
      for (idx_t jl = 0; jl < fields[AT].shape(1); ++jl) {
        exner_view(jn, jl) += potential_temperature_traj_view(jn, jl) *
                              temp_view(jn, jl);
        potential_temperature_view(jn, jl) += temp_view(jn, jl) *
                                              exner_traj_view(jn, jl);
        temp_view(jn, jl) = 0.0;
      }
    }
    oops::Log::trace() << "leaving AirTemperature_A::executeAD function" << std::endl;
}

}  // namespace vader
