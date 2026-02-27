/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirVirtualTemperature.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirVirtualTemperature_A::Name[] = "AirVirtualTemperature_A";
// Note that water_vapor_mixing_ratio_wrt_moist_air is referred to as "specific humidity"
// by some groups.
const oops::Variables AirVirtualTemperature_A::Ingredients{
      std::vector<std::string>{"air_temperature", "water_vapor_mixing_ratio_wrt_moist_air"}};

// Register the maker
static RecipeMaker<AirVirtualTemperature_A> makerTempToVTemp_(AirVirtualTemperature_A::Name);

AirVirtualTemperature_A::AirVirtualTemperature_A(const Parameters_ & params,
                                    const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "AirVirtualTemperature_A::AirVirtualTemperature_A(params)" << std::endl;
}

std::string AirVirtualTemperature_A::name() const
{
    return AirVirtualTemperature_A::Name;
}

oops::Variable AirVirtualTemperature_A::product() const
{
    return oops::Variable{"virtual_temperature"};
}

oops::Variables AirVirtualTemperature_A::ingredients() const
{
    return AirVirtualTemperature_A::Ingredients;
}

oops::Variables AirVirtualTemperature_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"air_temperature",
                                                    "water_vapor_mixing_ratio_wrt_moist_air"}};
}

size_t AirVirtualTemperature_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace AirVirtualTemperature_A::productFunctionSpace
                                              (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirVirtualTemperature_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering AirVirtualTemperature_A::executeNL function"
        << std::endl;

    // TODO(vahl) :"epsilon" should be changed to more CCPP
    //           "ratio_of_dry_air_gas_to_water_vapor_constants"
    const double epsilon = configVariables_.getDouble("epsilon");

    util::for_each_value(
        [&](const double temp,
            const double spechum,
            double& vTemp) {
            vTemp = temp * (1.0 + epsilon * spechum);
        },
        afieldset["air_temperature"],
        afieldset["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldset["virtual_temperature"]);

    oops::Log::trace() << "leaving AirVirtualTemperature_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirVirtualTemperature_A::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering AirVirtualTemperature_A::executeTL function"
        << std::endl;

    const double epsilon = configVariables_.getDouble("epsilon");

    util::for_each_value(
        [&](const double traj_temp,
            const double traj_spechum,
            const double tl_temp,
            const double tl_spechum,
            double& tl_vTemp) {
            tl_vTemp = tl_temp * (1.0 + epsilon * traj_spechum) +
                       traj_temp * epsilon * tl_spechum;
        },
        afieldsetTraj["air_temperature"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetTL["air_temperature"],
        afieldsetTL["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetTL["virtual_temperature"]);

    oops::Log::trace() << "leaving AirVirtualTemperature_A::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirVirtualTemperature_A::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering AirVirtualTemperature_A::executeAD function"
        << std::endl;

    const double epsilon = configVariables_.getDouble("epsilon");

    util::for_each_value(
        [&](const double traj_temp,
            const double traj_spechum,
            double& ad_vTemp,
            double& ad_temp,
            double& ad_spechum) {
            ad_temp += ad_vTemp * (1.0 + epsilon * traj_spechum);
            ad_spechum += ad_vTemp * epsilon * traj_temp;
            ad_vTemp = 0.0;
        },
        afieldsetTraj["air_temperature"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetAD["virtual_temperature"],
        afieldsetAD["air_temperature"],
        afieldsetAD["water_vapor_mixing_ratio_wrt_moist_air"]);

    oops::Log::trace() << "leaving AirVirtualTemperature_A::executeAD function" << std::endl;
}

}  // namespace vader
