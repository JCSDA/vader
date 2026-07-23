/*
 * (C) Crown Copyright 2025 Met Office.
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
#include "mo/eval_dust_2mode_variables.h"
#include "mo/recipes/DustAccumulationModeNumberFraction.h"
#include "oops/util/Logger.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DustAccumulationModeNumberFraction_A::Name[] = "DustAccumulationModeNumberFraction_A";
const oops::Variables DustAccumulationModeNumberFraction_A::
    Ingredients{std::vector<std::string>{
                                    "mass_fraction_of_dust001_in_air"
                                }};

// Register the maker
static RecipeMaker<DustAccumulationModeNumberFraction_A>
    makerDustAccumulationModeNumberFraction_A_(DustAccumulationModeNumberFraction_A::Name);

DustAccumulationModeNumberFraction_A::DustAccumulationModeNumberFraction_A(const Parameters_
                                        & params, const VaderConfigVars & configVariables)
    : configVariables_(configVariables)
{
    oops::Log::trace() <<
        "DustAccumulationModeNumberFraction_A::DustAccumulationModeNumberFraction_A" << std::endl;
}

std::string DustAccumulationModeNumberFraction_A::name() const
{
    return DustAccumulationModeNumberFraction_A::Name;
}

oops::Variable DustAccumulationModeNumberFraction_A::product() const
{
    return oops::Variable{"number_fraction_of_accumulation_aerosol_particles_in_air"};
}

oops::Variables DustAccumulationModeNumberFraction_A::ingredients() const
{
    return DustAccumulationModeNumberFraction_A::Ingredients;
}

oops::Variables DustAccumulationModeNumberFraction_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "mass_fraction_of_dust001_in_air",
        "number_fraction_of_accumulation_aerosol_particles_in_air"}};
}

size_t DustAccumulationModeNumberFraction_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["mass_fraction_of_dust001_in_air"].shape(1);
}

atlas::FunctionSpace DustAccumulationModeNumberFraction_A::productFunctionSpace(
                                                                 const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["mass_fraction_of_dust001_in_air"].functionspace();
}

void DustAccumulationModeNumberFraction_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering DustAccumulationModeNumberFraction_A::executeNL function"
                       << std::endl;
    mo::eval_dust_accumulation_mode_number_fraction_nl(afieldset);
    oops::Log::trace() << "leaving DustAccumulationModeNumberFraction_A::executeNL function"
                       << std::endl;
}

void DustAccumulationModeNumberFraction_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering DustAccumulationModeNumberFraction_A::executeTL function"
        << std::endl;
    mo::eval_dust_accumulation_mode_number_fraction_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "leaving DustAccumulationModeNumberFraction_A::executeTL function"
        << std::endl;
}

void DustAccumulationModeNumberFraction_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << afieldsetAD.field_names()
        << afieldsetTraj.field_names()
        << "entering DustAccumulationModeNumberFraction_A::executeAD function"
        << std::endl;
    mo::eval_dust_accumulation_mode_number_fraction_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "leaving DustAccumulationModeNumberFraction_A::executeAD function"
        << std::endl;
}

}  // namespace vader
