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
#include "mo/recipes/DustAccumulationModeMassFraction.h"
#include "oops/util/Logger.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DustAccumulationModeMassFraction_A::Name[] = "DustAccumulationModeMassFraction_A";
const oops::Variables DustAccumulationModeMassFraction_A::
    Ingredients{std::vector<std::string>{
                                    "mass_fraction_of_dust001_in_air"
                                }};

// Register the maker
static RecipeMaker<DustAccumulationModeMassFraction_A>
    makerDustAccumulationModeMassFraction_A_(DustAccumulationModeMassFraction_A::Name);

DustAccumulationModeMassFraction_A::DustAccumulationModeMassFraction_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables)
    : configVariables_(configVariables)
{
    oops::Log::trace() <<
        "DustAccumulationModeMassFraction_A::DustAccumulationModeMassFraction_A" << std::endl;
}

std::string DustAccumulationModeMassFraction_A::name() const
{
    return DustAccumulationModeMassFraction_A::Name;
}

oops::Variable DustAccumulationModeMassFraction_A::product() const
{
    return oops::Variable{"mass_fraction_of_dust_accumulation_aerosol_particles_in_air"};
}

oops::Variables DustAccumulationModeMassFraction_A::ingredients() const
{
    return DustAccumulationModeMassFraction_A::Ingredients;
}

oops::Variables DustAccumulationModeMassFraction_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "mass_fraction_of_dust001_in_air",
        "mass_fraction_of_dust_accumulation_aerosol_particles_in_air"}};
}

size_t DustAccumulationModeMassFraction_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["mass_fraction_of_dust001_in_air"].shape(1);
}

atlas::FunctionSpace DustAccumulationModeMassFraction_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["mass_fraction_of_dust001_in_air"].functionspace();
}

void DustAccumulationModeMassFraction_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering DustAccumulationModeMassFraction_A::executeNL function"
                       << std::endl;
    mo::eval_dust_accumulation_mode_mass_fraction_nl(afieldset);
    oops::Log::trace() << "leaving DustAccumulationModeMassFraction_A::executeNL function"
                       << std::endl;
}

void DustAccumulationModeMassFraction_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering DustAccumulationModeMassFraction_A::executeTL function"
        << std::endl;
    mo::eval_dust_accumulation_mode_mass_fraction_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "leaving DustAccumulationModeMassFraction_A::executeTL function"
        << std::endl;
}

void DustAccumulationModeMassFraction_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << afieldsetAD.field_names()
        << afieldsetTraj.field_names()
        << "entering DustAccumulationModeMassFraction_A::executeAD function"
        << std::endl;
    mo::eval_dust_accumulation_mode_mass_fraction_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "leaving DustAccumulationModeMassFraction_A::executeAD function"
        << std::endl;
}

}  // namespace vader
