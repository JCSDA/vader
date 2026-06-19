/*
 * (C) Crown Copyright 2023 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "mo/eval_virtual_potential_temperature.h"
#include "oops/util/Logger.h"
#include "vader/recipes/VirtualPotentialTemperature.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char VirtualPotentialTemperature_B::Name[] = "VirtualPotentialTemperature_B";
const oops::Variables VirtualPotentialTemperature_B::
    Ingredients{std::vector<std::string>{
                                    "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water",
                                    "air_potential_temperature"}};

// Register the maker
static RecipeMaker<VirtualPotentialTemperature_B>
    makerVirtualPotentialTemperature_B_(VirtualPotentialTemperature_B::Name);

VirtualPotentialTemperature_B::VirtualPotentialTemperature_B(const Parameters_ & params,
                                        const VaderConfigVars & configVariables)
{
    oops::Log::trace() <<
        "VirtualPotentialTemperature_B::VirtualPotentialTemperature_B" << std::endl;
}

std::string VirtualPotentialTemperature_B::name() const
{
    return VirtualPotentialTemperature_B::Name;
}

oops::Variable VirtualPotentialTemperature_B::product() const
{
    return oops::Variable{"virtual_potential_temperature"};
}

oops::Variables VirtualPotentialTemperature_B::ingredients() const
{
    return VirtualPotentialTemperature_B::Ingredients;
}

oops::Variables VirtualPotentialTemperature_B::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water", "air_potential_temperature"}};
}

size_t VirtualPotentialTemperature_B::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["air_potential_temperature"].shape(1);
}

atlas::FunctionSpace VirtualPotentialTemperature_B::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["air_potential_temperature"].functionspace();
}
// -------------------------------------------------------------------------------------------------

void VirtualPotentialTemperature_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering VirtualPotentialTemperature_B::executeNL function"
                       << std::endl;
    mo::eval_virtual_potential_temperature_nl(afieldset);
    oops::Log::trace() << "leaving VirtualPotentialTemperature_B::executeNL function" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void VirtualPotentialTemperature_B::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering VirtualPotentialTemperature_B::executeTL function"
                       << std::endl;
    mo::eval_virtual_potential_temperature_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace() << "leaving VirtualPotentialTemperature_B::executeTL function" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void VirtualPotentialTemperature_B::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering VirtualPotentialTemperature_B::executeAD function"
                       << std::endl;
    mo::eval_virtual_potential_temperature_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace() << "leaving VirtualPotentialTemperature_B::executeAD function"
                       << std::endl;
}

}  // namespace vader
