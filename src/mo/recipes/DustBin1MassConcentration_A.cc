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
#include "mo/eval_dust_2bin_mass_concentration.h"
#include "mo/recipes/DustBin1MassConcentration.h"
#include "oops/util/Logger.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DustBin1MassConcentration_A::Name[] = "DustBin1MassConcentration_A";
const oops::Variables DustBin1MassConcentration_A::
    Ingredients{std::vector<std::string>{
                                    "mass_fraction_of_dust_coarse_aerosol_particles_in_air",
                                    "mass_fraction_of_dust_accumulation_aerosol_particles_in_air",
                                    "number_fraction_of_coarse_aerosol_particles_in_air",
                                    "number_fraction_of_accumulation_aerosol_particles_in_air"}};

// Register the maker
static RecipeMaker<DustBin1MassConcentration_A>
    makerDustBin1MassConcentration_A_(DustBin1MassConcentration_A::Name);

DustBin1MassConcentration_A::DustBin1MassConcentration_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables)
    : configVariables_(configVariables)
{
    oops::Log::trace() <<
        "DustBin1MassConcentration_A::DustBin1MassConcentration_A" << std::endl;
}

std::string DustBin1MassConcentration_A::name() const
{
    return DustBin1MassConcentration_A::Name;
}

oops::Variable DustBin1MassConcentration_A::product() const
{
    return oops::Variable{"mass_fraction_of_dust001_in_air"};
}

oops::Variables DustBin1MassConcentration_A::ingredients() const
{
    return DustBin1MassConcentration_A::Ingredients;
}

size_t DustBin1MassConcentration_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["mass_fraction_of_dust_coarse_aerosol_particles_in_air"].shape(1);
}

atlas::FunctionSpace DustBin1MassConcentration_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["mass_fraction_of_dust_coarse_aerosol_particles_in_air"].functionspace();
}

void DustBin1MassConcentration_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering DustBin1MassConcentration_A::executeNL function"
                       << std::endl;
    mo::eval_dust_bin_mass_concentration_nl(afieldset, mo::DustBin::bin1);
    oops::Log::trace() << "leaving DustBin1MassConcentration_A::executeNL function" << std::endl;
}
}  // namespace vader
