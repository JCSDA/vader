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
#include "mo/eval_hydrostatic_balance.h"
#include "oops/util/Logger.h"
#include "vader/recipes/VirtualPotentialTemperature.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char VirtualPotentialTemperature_A::Name[] = "VirtualPotentialTemperature_A";
const oops::Variables VirtualPotentialTemperature_A::
    Ingredients{std::vector<std::string>{"height_above_mean_sea_level_levels",
                                         "hydrostatic_exner_levels"}};

// Register the maker
static RecipeMaker<VirtualPotentialTemperature_A>
    makerVirtualPotentialTemperature_A_(VirtualPotentialTemperature_A::Name);

VirtualPotentialTemperature_A::VirtualPotentialTemperature_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables)
{
    oops::Log::trace() <<
        "VirtualPotentialTemperature_A::VirtualPotentialTemperature_A(params)" << std::endl;
}

std::string VirtualPotentialTemperature_A::name() const
{
    return VirtualPotentialTemperature_A::Name;
}

oops::Variable VirtualPotentialTemperature_A::product() const
{
    return oops::Variable{"virtual_potential_temperature"};
}

oops::Variables VirtualPotentialTemperature_A::ingredients() const
{
    return VirtualPotentialTemperature_A::Ingredients;
}

oops::Variables VirtualPotentialTemperature_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"height_above_mean_sea_level_levels",
                                                    "virtual_potential_temperature"}};
}

size_t VirtualPotentialTemperature_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return (afieldset["height_above_mean_sea_level_levels"].shape(1) -1);
}

atlas::FunctionSpace VirtualPotentialTemperature_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["height_above_mean_sea_level_levels"].functionspace();
}
// -------------------------------------------------------------------------------------------------

void VirtualPotentialTemperature_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering VirtualPotentialTemperature_A::executeNL function"
      << std::endl;
    mo::eval_hydrobal_virtual_potential_temperature_nl(afieldset);
    oops::Log::trace() << "leaving VirtualPotentialTemperature_A::executeNL function" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void VirtualPotentialTemperature_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering VirtualPotentialTemperature_A::executeTL function"
        << std::endl;
    mo::eval_hydrobal_virtual_potential_temperature_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace() << "leaving VirtualPotentialTemperature_A::executeTL function" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void VirtualPotentialTemperature_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering VirtualPotentialTemperature::executeAD function"
        << std::endl;
    mo::eval_hydrobal_virtual_potential_temperature_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace() << "leaving VirtualPotentialTemperature_A::executeAD function" << std::endl;
}

}  // namespace vader
