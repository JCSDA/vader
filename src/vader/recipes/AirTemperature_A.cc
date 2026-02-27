/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirTemperature.h"

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

// -------------------------------------------------------------------------------------------------

void AirTemperature_A::executeNL(atlas::FieldSet & fields)
{
    oops::Log::trace() << "entering AirTemperature_A::executeNL function" << std::endl;

    util::for_each_value(
        [](const double apt,
           const double exner,
           double& temp) {
           temp = apt * exner;
        },
        fields[APT],
        fields[DEF],
        fields[AT]);

    oops::Log::trace() << "leaving AirTemperature_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirTemperature_A::executeTL(atlas::FieldSet & fieldsTL,
                                 const atlas::FieldSet & trajFields)
{
    oops::Log::trace() << "entering AirTemperature_A::executeTL function" << std::endl;

    util::for_each_value(
        [](const double traj_exner,
           const double traj_apt,
           const double tl_apt,
           const double tl_exner,
           double& tl_temp) {
           tl_temp = traj_apt * tl_exner + tl_apt * traj_exner;
        },
        trajFields[DEF],
        trajFields[APT],
        fieldsTL[APT],
        fieldsTL[DEF],
        fieldsTL[AT]);

    oops::Log::trace() << "leaving AirTemperature_A::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirTemperature_A::executeAD(atlas::FieldSet & fieldsAD,
                                 const atlas::FieldSet & trajFields)
{
    oops::Log::trace() << "entering AirTemperature_A::executeAD function" << std::endl;

    util::for_each_value(
        [](const double traj_exner,
           const double traj_apt,
           double& ad_temp,
           double& ad_apt,
           double& ad_exner) {
           ad_exner += traj_apt * ad_temp;
           ad_apt += ad_temp * traj_exner;
           ad_temp = 0.0;
        },
        trajFields[DEF],
        trajFields[APT],
        fieldsAD[AT],
        fieldsAD[APT],
        fieldsAD[DEF]);

    oops::Log::trace() << "leaving AirTemperature_A::executeAD function" << std::endl;
}

}  // namespace vader
