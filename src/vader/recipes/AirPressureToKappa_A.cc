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
#include "vader/recipes/AirPressureToKappa.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPressureToKappa_A::Name[] = "AirPressureToKappa_A";
const oops::Variables AirPressureToKappa_A::Ingredients{
      std::vector<std::string>{"air_pressure_levels", "ln_air_pressure_at_interface"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<AirPressureToKappa_A> makerAirPressureToKappa_A_(AirPressureToKappa_A::Name);

// -------------------------------------------------------------------------------------------------

AirPressureToKappa_A::AirPressureToKappa_A(const AirPressureToKappa_AParameters & params,
                                        const VaderConfigVars & configVariables)
: configVariables_{configVariables} {
    oops::Log::trace() << "AirPressureToKappa_A::AirPressureToKappa_A Starting" << std::endl;
    oops::Log::trace() << "AirPressureToKappa_A::AirPressureToKappa_A Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressureToKappa_A::name() const {
    return AirPressureToKappa_A::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable AirPressureToKappa_A::product() const {
    return oops::Variable{"air_pressure_to_kappa"};
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressureToKappa_A::ingredients() const {
    return AirPressureToKappa_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPressureToKappa_A::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_levels").shape(1) - 1;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPressureToKappa_A::productFunctionSpace(
  const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_levels").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirPressureToKappa_A::executeNL(atlas::FieldSet & afieldset) {
    oops::Log::trace() << "AirPressureToKappa_A::executeNL Starting" << std::endl;

    const double kappa = configVariables_.getDouble("kappa");  // Need better name

    // Get fields
    atlas::Field airPressureLevels = afieldset.field("air_pressure_levels");
    atlas::Field lnAirPressureAtInterface = afieldset.field("ln_air_pressure_at_interface");
    atlas::Field airPressureToKappa = afieldset.field("air_pressure_to_kappa");

    util::for_each_column(
        [&](const auto airPressureLevels_col,
            const auto lnAirPressureAtInterface_col,
            auto airPressureToKappa_col) {
            for (int level = 0; level < airPressureLevels.shape(1) - 1; ++level) {
                double pk1 = std::exp(kappa * lnAirPressureAtInterface_col(level));
                double pk2 = std::exp(kappa * lnAirPressureAtInterface_col(level+1));
                airPressureToKappa_col(level) = (pk2 - pk1) /
                  (kappa * (lnAirPressureAtInterface_col(level+1) -
                   lnAirPressureAtInterface_col(level)));
            }
        },
        airPressureLevels,
        lnAirPressureAtInterface,
        airPressureToKappa);

    oops::Log::trace() << "AirPressureToKappa_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
