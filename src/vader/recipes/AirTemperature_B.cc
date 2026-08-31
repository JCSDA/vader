/*
 * (C) Copyright 2022- UCAR.
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
const char AirTemperature_B::Name[] = "AirTemperature_B";
const oops::Variables AirTemperature_B::Ingredients{std::vector<std::string>{
                                                        "virtual_temperature",
                                                        "water_vapor_mixing_ratio_wrt_moist_air"}};

// Register the maker
static RecipeMaker<AirTemperature_B> makerAirTemperature_B_(AirTemperature_B::Name);

AirTemperature_B::AirTemperature_B(const Parameters_ & params,
                                   const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "AirTemperature_B::AirTemperature_B(params)" << std::endl;
}

std::string AirTemperature_B::name() const
{
    return AirTemperature_B::Name;
}

oops::Variable AirTemperature_B::product() const
{
    return oops::Variable{"air_temperature"};
}

oops::Variables AirTemperature_B::ingredients() const
{
    return AirTemperature_B::Ingredients;
}

oops::Variables AirTemperature_B::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"virtual_temperature",
                                                    "water_vapor_mixing_ratio_wrt_moist_air"}};
}

size_t AirTemperature_B::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("virtual_temperature").shape(1);
}

atlas::FunctionSpace AirTemperature_B::productFunctionSpace(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("virtual_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirTemperature_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering AirTemperature_B::executeNL function"
        << std::endl;

    const double epsilon = configVariables_.getDouble("epsilon");
    const double epsilon_star = 1.0 / epsilon - 1.0;

    util::for_each_value(
        [&](const double vTemp,
            const double spechum,
            double& temp) {
            temp = vTemp / (1.0 + epsilon_star * spechum);
        },
        afieldset["virtual_temperature"],
        afieldset["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldset["air_temperature"]);

    oops::Log::trace() << "leaving AirTemperature_B::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------
/*  NL: temp = vTemp / (1 + epsilon_star * spechum)
 *
 *  TL: tl_temp = tl_vTemp / (1 + epsilon_star * spechum) -
 *                vTemp * epsilon_star * tl_spechum / (1 + epsilon_star * spechum)^2
 */
void AirTemperature_B::executeTL(atlas::FieldSet & afieldsetTL,
                                 const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering AirTemperature_B::executeTL function"
        << std::endl;

    const double epsilon = configVariables_.getDouble("epsilon");
    const double epsilon_star = 1.0 / epsilon - 1.0;

    util::for_each_value(
        [&](const double traj_vTemp,
            const double traj_spechum,
            const double tl_vTemp,
            const double tl_spechum,
            double& tl_temp) {
            const double denom = 1.0 + epsilon_star * traj_spechum;
            tl_temp = tl_vTemp / denom -
                      traj_vTemp * epsilon_star * tl_spechum / (denom * denom);
        },
        afieldsetTraj["virtual_temperature"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetTL["virtual_temperature"],
        afieldsetTL["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetTL["air_temperature"]);

    oops::Log::trace() << "leaving AirTemperature_B::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirTemperature_B::executeAD(atlas::FieldSet & afieldsetAD,
                                 const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering AirTemperature_B::executeAD function"
        << std::endl;

    const double epsilon = configVariables_.getDouble("epsilon");
    const double epsilon_star = 1.0 / epsilon - 1.0;

    util::for_each_value(
        [&](const double traj_vTemp,
            const double traj_spechum,
            double& ad_temp,
            double& ad_vTemp,
            double& ad_spechum) {
            const double denom = 1.0 + epsilon_star * traj_spechum;
            ad_vTemp += ad_temp / denom;
            ad_spechum -= ad_temp * traj_vTemp * epsilon_star / (denom * denom);
            ad_temp = 0.0;
        },
        afieldsetTraj["virtual_temperature"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetAD["air_temperature"],
        afieldsetAD["virtual_temperature"],
        afieldsetAD["water_vapor_mixing_ratio_wrt_moist_air"]);

    oops::Log::trace() << "leaving AirTemperature_B::executeAD function" << std::endl;
}

}  // namespace vader
