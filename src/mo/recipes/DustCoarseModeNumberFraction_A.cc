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
#include "mo/recipes/DustCoarseModeNumberFraction.h"
#include "oops/util/Logger.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DustCoarseModeNumberFraction_A::Name[] = "DustCoarseModeNumberFraction_A";
const oops::Variables DustCoarseModeNumberFraction_A::
    Ingredients{std::vector<std::string>{
                                    "mass_fraction_of_dust001_in_air",
                                    "mass_fraction_of_dust002_in_air"
                                }};

// Register the maker
static RecipeMaker<DustCoarseModeNumberFraction_A>
    makerDustCoarseModeNumberFraction_A_(DustCoarseModeNumberFraction_A::Name);

DustCoarseModeNumberFraction_A::DustCoarseModeNumberFraction_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables)
    : configVariables_(configVariables)
{
    oops::Log::trace() <<
        "DustCoarseModeNumberFraction_A::DustCoarseModeNumberFraction_A" << std::endl;
}

std::string DustCoarseModeNumberFraction_A::name() const
{
    return DustCoarseModeNumberFraction_A::Name;
}

oops::Variable DustCoarseModeNumberFraction_A::product() const
{
    return oops::Variable{"number_fraction_of_coarse_aerosol_particles_in_air"};
}

oops::Variables DustCoarseModeNumberFraction_A::ingredients() const
{
    return DustCoarseModeNumberFraction_A::Ingredients;
}

oops::Variables DustCoarseModeNumberFraction_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "mass_fraction_of_dust001_in_air",
        "mass_fraction_of_dust002_in_air",
        "number_fraction_of_coarse_aerosol_particles_in_air"}};
}

size_t DustCoarseModeNumberFraction_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["mass_fraction_of_dust001_in_air"].shape(1);
}

atlas::FunctionSpace DustCoarseModeNumberFraction_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["mass_fraction_of_dust001_in_air"].functionspace();
}

void DustCoarseModeNumberFraction_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering DustCoarseModeNumberFraction_A::executeNL function"
                       << std::endl;
    mo::eval_dust_coarse_mode_number_fraction_nl(afieldset);
    oops::Log::trace() << "leaving DustCoarseModeNumberFraction_A::executeNL function" << std::endl;
}

void DustCoarseModeNumberFraction_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering DustCoarseModeNumberFraction_A::executeTL function"
        << std::endl;
    mo::eval_dust_coarse_mode_number_fraction_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "leaving DustCoarseModeNumberFraction_A::executeTL function"
        << std::endl;
}

void DustCoarseModeNumberFraction_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << afieldsetAD.field_names()
        << afieldsetTraj.field_names()
        << "entering DustCoarseModeNumberFraction_A::executeAD function"
        << std::endl;
    mo::eval_dust_coarse_mode_number_fraction_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "leaving DustCoarseModeNumberFraction_A::executeAD function"
        << std::endl;
}

}  // namespace vader
