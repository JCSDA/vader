/*
 * (C) Copyright 2021-2023  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirPressureThickness.h"

namespace vader
{

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPressureThickness_A::Name[] = "AirPressureThickness_A";
const oops::Variables AirPressureThickness_A::Ingredients{
      std::vector<std::string>{"air_pressure_levels"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<AirPressureThickness_A>
                    makerAirPressureThickness_A_(AirPressureThickness_A::Name);

// -------------------------------------------------------------------------------------------------

AirPressureThickness_A::AirPressureThickness_A(const AirPressureThickness_AParameters & params,
                                        const VaderConfigVars & configVariables) {
    oops::Log::trace() << "AirPressureThickness_A::AirPressureThickness_A(params)" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressureThickness_A::name() const {
    return AirPressureThickness_A::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable AirPressureThickness_A::product() const {
    return oops::Variable{"air_pressure_thickness"};
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressureThickness_A::ingredients() const {
    return AirPressureThickness_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPressureThickness_A::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_levels").shape(1) - 1;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPressureThickness_A::productFunctionSpace
                                            (const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_levels").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirPressureThickness_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering AirPressureThickness_A execute function" << std::endl;
    // Get the fields
    atlas::Field prsi = afieldset.field("air_pressure_levels");
    atlas::Field delp = afieldset.field("air_pressure_thickness");

    // Get the units
    std::string prsi_units, delp_units;
    prsi.metadata().get("units", prsi_units);
    delp.metadata().get("units", delp_units);

    // Assert that the units match
    ASSERT_MSG(prsi_units == delp_units, "In Vader::AirPressureThickness_A::executeNL the "
               "units for pressure " + prsi_units +
               "do not match the pressure thickness units" + delp_units);

    util::for_each_column(
        [&](const auto prsi_col,
            auto delp_col) {
            for (int level = 0; level < delp.shape(1); ++level) {
                delp_col(level) = prsi_col(level+1) - prsi_col(level);
            }
        },
        prsi,
        delp);

    oops::Log::trace() << "leaving AirPressureThickness_A execute function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
