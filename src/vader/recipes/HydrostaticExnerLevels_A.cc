/*
 * (C) Crown Copyright 2023-2025 Met Office.
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
#include "vader/recipes/HydrostaticExnerLevels.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char HydrostaticExnerLevels_A::Name[] = "HydrostaticExnerLevels_A";
const oops::Variables HydrostaticExnerLevels_A::
    Ingredients{std::vector<std::string>{"height_above_mean_sea_level_levels",
                                         "virtual_potential_temperature", "air_pressure_levels"}};

// Register the maker
static RecipeMaker<HydrostaticExnerLevels_A>
    makerHydrostaticExnerLevels_A_(HydrostaticExnerLevels_A::Name);

HydrostaticExnerLevels_A::HydrostaticExnerLevels_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables)
{
    oops::Log::trace() <<
        "HydrostaticExnerLevels_A::HydrostaticExnerLevels_A(params, configVariables)" << std::endl;
}

std::string HydrostaticExnerLevels_A::name() const
{
    return HydrostaticExnerLevels_A::Name;
}

oops::Variable HydrostaticExnerLevels_A::product() const
{
    return oops::Variable{"hydrostatic_exner_levels"};
}

oops::Variables HydrostaticExnerLevels_A::ingredients() const
{
    return HydrostaticExnerLevels_A::Ingredients;
}

size_t HydrostaticExnerLevels_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return (afieldset["height_above_mean_sea_level_levels"].shape(1));
}

atlas::FunctionSpace HydrostaticExnerLevels_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
    return afieldset["air_pressure_levels"].functionspace();
}

void HydrostaticExnerLevels_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering HydrostaticExnerLevels_A::executeNL function"
      << std::endl;
    mo::eval_hydrobal_hydrostatic_exner_levels_nl(afieldset);
    oops::Log::trace() << "leaving HydrostaticExnerLevels_A::executeNL function" << std::endl;
}

void HydrostaticExnerLevels_A::executeTL(atlas::FieldSet & afieldsetTL,
                                         const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering HydrostaticExnerLevels_A::executeTL function"
        << std::endl;
    mo::eval_hydrobal_hydrostatic_exner_levels_tl(afieldsetTL, afieldsetTraj);
    oops::Log::trace() << "leaving HydrostaticExnerLevels_A::executeTL function" << std::endl;
}

void HydrostaticExnerLevels_A::executeAD(atlas::FieldSet & afieldsetAD,
                                         const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering HydrostaticExnerLevels::executeAD function"
        << std::endl;
    mo::eval_hydrobal_hydrostatic_exner_levels_ad(afieldsetAD, afieldsetTraj);
    oops::Log::trace() << "leaving HydrostaticExnerLevels_A::executeAD function" << std::endl;
}

}  // namespace vader
