/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/field/for_each.h"
#include "atlas/util/Metadata.h"
#include "oops/util/Logger.h"
#include "vader/recipes/SulfateMassFraction.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char SulfateMassFraction_A::Name[] = "SulfateMassFraction_A";
const oops::Variables SulfateMassFraction_A::Ingredients{{"sulfate_ppmv"}};

// Register the maker
static RecipeMaker<SulfateMassFraction_A> makerSulfateMassFraction_(SulfateMassFraction_A::Name);

SulfateMassFraction_A::SulfateMassFraction_A(const Parameters_ & params,
                                       const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "SulfateMassFraction_A::SulfateMassFraction_A(params)" << std::endl;
}

std::string SulfateMassFraction_A::name() const
{
    return SulfateMassFraction_A::Name;
}

oops::Variable SulfateMassFraction_A::product() const
{
    return oops::Variable{"mass_fraction_of_sulfate_in_air"};
}

oops::Variables SulfateMassFraction_A::ingredients() const
{
    return SulfateMassFraction_A::Ingredients;
}

size_t SulfateMassFraction_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("sulfate_ppmv").shape(1);
}

atlas::FunctionSpace SulfateMassFraction_A::productFunctionSpace
                                              (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("sulfate_ppmv").functionspace();
}

void SulfateMassFraction_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering SulfateMassFraction_A::executeNL function" << std::endl;

    const double sulfmw = configVariables_.getDouble("molecular_weight_of_so4");
    const double airmw = configVariables_.getDouble("molecular_weight_of_air");

    atlas::field::for_each_value(afieldset["sulfate_ppmv"],
                                 afieldset["mass_fraction_of_sulfate_in_air"],
                                 [&](const double sulf_ppmv, double& sulf_ugkg) {
        sulf_ugkg = sulf_ppmv * sulfmw / airmw * 1e3;
    });

    oops::Log::trace() << "leaving SulfateMassFraction_A::executeNL function" << std::endl;
}

}  // namespace vader
