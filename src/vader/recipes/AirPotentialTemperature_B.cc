/*
 * (C) Copyright 2023  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirPotentialTemperature.h"

namespace vader
{
// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPotentialTemperature_B::Name[] = "AirPotentialTemperature_B";
const oops::Variables AirPotentialTemperature_B::Ingredients{
      std::vector<std::string>{"air_temperature", "air_pressure_to_kappa"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<AirPotentialTemperature_B> makerTempToPTemp_(AirPotentialTemperature_B::Name);

// -------------------------------------------------------------------------------------------------

AirPotentialTemperature_B::AirPotentialTemperature_B(const Parameters_ & params,
                                        const VaderConfigVars & configVariables) {
    oops::Log::trace() << "AirPotentialTemperature_B::AirPotentialTemperature_B(params)"
        << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPotentialTemperature_B::name() const {
    return AirPotentialTemperature_B::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable AirPotentialTemperature_B::product() const {
    return oops::Variable("air_potential_temperature");
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPotentialTemperature_B::ingredients() const {
    return AirPotentialTemperature_B::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPotentialTemperature_B::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_temperature").shape(1);
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPotentialTemperature_B::productFunctionSpace
                                                (const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirPotentialTemperature_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering AirPotentialTemperature_B::executeNL function" << std::endl;

    util::for_each_value(
      [](const double temp,
         const double ptk,
         double& ptemp) {
         ptemp = temp / ptk;
      },
      afieldset["air_temperature"],
      afieldset["air_pressure_to_kappa"],
      afieldset["air_potential_temperature"]);

    oops::Log::trace() << "leaving AirPotentialTemperature_B::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
