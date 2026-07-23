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
#include "mo/recipes/DustCoarseModeMassFraction.h"
#include "oops/util/Logger.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DustCoarseModeMassFraction_A::Name[] = "DustCoarseModeMassFraction_A";
const oops::Variables DustCoarseModeMassFraction_A::
    Ingredients{std::vector<std::string>{
                                    "mass_fraction_of_dust001_in_air",
                                    "mass_fraction_of_dust002_in_air"
                                }};

// Register the maker
static RecipeMaker<DustCoarseModeMassFraction_A>
    makerDustCoarseModeMassFraction_A_(DustCoarseModeMassFraction_A::Name);

DustCoarseModeMassFraction_A::DustCoarseModeMassFraction_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables)
    : configVariables_(configVariables)
{
    oops::Log::trace() <<
        "DustCoarseModeMassFraction_A::DustCoarseModeMassFraction_A" << std::endl;
}

std::string DustCoarseModeMassFraction_A::name() const
{
    return DustCoarseModeMassFraction_A::Name;
}

oops::Variable DustCoarseModeMassFraction_A::product() const
{
    return oops::Variable{"mass_fraction_of_dust_coarse_aerosol_particles_in_air"};
}

oops::Variables DustCoarseModeMassFraction_A::ingredients() const
{
    return DustCoarseModeMassFraction_A::Ingredients;
}

oops::Variables DustCoarseModeMassFraction_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{
        "mass_fraction_of_dust001_in_air",
        "mass_fraction_of_dust002_in_air",
        "mass_fraction_of_dust_coarse_aerosol_particles_in_air"}};
}

size_t DustCoarseModeMassFraction_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["mass_fraction_of_dust001_in_air"].shape(1);
}

atlas::FunctionSpace DustCoarseModeMassFraction_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["mass_fraction_of_dust001_in_air"].functionspace();
}

void DustCoarseModeMassFraction_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering DustCoarseModeMassFraction_A::executeNL function"
                       << std::endl;
    mo::eval_dust_coarse_mode_mass_fraction_nl(afieldset);
    oops::Log::trace() << "leaving DustCoarseModeMassFraction_A::executeNL function" << std::endl;
}

void DustCoarseModeMassFraction_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering DustCoarseModeMassFraction_A::executeTL function"
        << std::endl;
    mo::eval_dust_coarse_mode_mass_fraction_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace()
        << "leaving DustCoarseModeMassFraction_A::executeTL function"
        << std::endl;
}

void DustCoarseModeMassFraction_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << afieldsetAD.field_names()
        << afieldsetTraj.field_names()
        << "entering DustCoarseModeMassFraction_A::executeAD function"
        << std::endl;
    mo::eval_dust_coarse_mode_mass_fraction_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace()
        << "leaving DustCoarseModeMassFraction_A::executeAD function"
        << std::endl;
}

}  // namespace vader
