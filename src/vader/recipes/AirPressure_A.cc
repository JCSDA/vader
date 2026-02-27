/*
 * (C) Copyright 2023  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirPressure.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPressure_A::Name[] = "AirPressure_A";
const oops::Variables AirPressure_A::Ingredients{std::vector<std::string>{"air_pressure_levels"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<AirPressure_A> makerAirPressure_A_(AirPressure_A::Name);

// -------------------------------------------------------------------------------------------------

AirPressure_A::AirPressure_A(const AirPressure_AParameters & params,
                             const VaderConfigVars & configVariables) :
                                    configVariables_{configVariables}
{
    oops::Log::trace() << "AirPressure_A::AirPressure_A Starting" << std::endl;
    oops::Log::trace() << "AirPressure_A::AirPressure_A Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressure_A::name() const {
    return AirPressure_A::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable AirPressure_A::product() const {
    return oops::Variable("air_pressure");
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressure_A::ingredients() const {
    return AirPressure_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPressure_A::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_levels").shape(1) - 1;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPressure_A::productFunctionSpace(const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_levels").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirPressure_A::executeNL(atlas::FieldSet & afieldset) {
    oops::Log::trace() << "AirPressure_A::executeNL Starting" << std::endl;

    // Extract value from client config
    const double kappa = configVariables_.getDouble("kappa");  // Need better name

    // kappa variations
    const double kap1 = kappa + 1.0;
    const double kapr = 1.0 / kappa;

    // Get fields
    atlas::Field airPressureLevels = afieldset.field("air_pressure_levels");
    atlas::Field airPressure = afieldset.field("air_pressure");

    // Number of levels in the output field air_pressure
    int nlevels = airPressureLevels.shape(1) - 1;

    util::for_each_column(
        [&](const auto airPressureLevels_col,
            auto airPressure_col) {
            for (int level = 0; level < nlevels; ++level) {
                airPressure_col(level) = std::pow(((std::pow(airPressureLevels_col(level+1), kap1) -
                                                    std::pow(airPressureLevels_col(level), kap1)) /
                                                    (kap1*(airPressureLevels_col(level+1) -
                                                    airPressureLevels_col(level)))), kapr);
            }
        },
        airPressureLevels,
        airPressure);

    oops::Log::trace() << "AirPressure_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
