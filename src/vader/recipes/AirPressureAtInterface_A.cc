/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field/Field.h"

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirPressureAtInterface.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPressureAtInterface_A::Name[] = "AirPressureAtInterface_A";
const oops::Variables AirPressureAtInterface_A::Ingredients{
      std::vector<std::string>{"air_pressure_at_surface"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<AirPressureAtInterface_A>
       makerAirPressureAtInterface_A_(AirPressureAtInterface_A::Name);

// -------------------------------------------------------------------------------------------------

AirPressureAtInterface_A::AirPressureAtInterface_A(
                                                 const AirPressureAtInterface_AParameters & params,
                                                 const VaderConfigVars & configVariables):
                                                 configVariables_{configVariables}
{
    oops::Log::trace() << "AirPressureAtInterface_A::AirPressureAtInterface_A Starting"
                       << std::endl;
    oops::Log::trace() << "AirPressureAtInterface_A::AirPressureAtInterface_A Done"
                       << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressureAtInterface_A::name() const {
    return AirPressureAtInterface_A::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable AirPressureAtInterface_A::product() const {
    return oops::Variable("air_pressure_levels");
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressureAtInterface_A::ingredients() const {
    return AirPressureAtInterface_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPressureAtInterface_A::productLevels(const atlas::FieldSet & afieldset) const {
    int nLevels = configVariables_.getInt("nLevels");
    return nLevels + 1;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPressureAtInterface_A::productFunctionSpace(const atlas::FieldSet
& afieldset) const {
    return afieldset.field("air_pressure_at_surface").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirPressureAtInterface_A::executeNL(atlas::FieldSet & afieldset) {
    oops::Log::trace() << "AirPressureAtInterface_A::executeNL Starting" << std::endl;

    // Get the fields
    atlas::Field ps = afieldset.field("air_pressure_at_surface");
    atlas::Field prsi = afieldset.field("air_pressure_levels");

    // Get the units
    std::string prsi_units, ps_units;
    ps.metadata().get("units", ps_units);
    prsi.metadata().get("units", prsi_units);

    // Assert that the units match
    ASSERT_MSG(prsi_units.empty() || ps_units.empty() || prsi_units == ps_units,
               "In Vader::AirPressureAtInterface_A::executeNL the units "
               "for pressure " + prsi_units + "do not match the surface pressure units" + ps_units);

    // Get number of levels
    int nLevels = configVariables_.getInt("nLevels");

    // Extract ak/bk from client config
    std::vector<double> ak = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_a_coefficient");
    std::vector<double> bk = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_b_coefficient");
    ASSERT(ak.size() == nLevels+1);
    ASSERT(bk.size() == nLevels+1);

    // Compute pressure thickness from pressure at the levels
    util::for_each_column(
        [&](const auto ps_col,
            auto prsi_col) {
            for (int level = 0; level < nLevels + 1; ++level) {
                prsi_col(level) = ak[level] + bk[level] * ps_col(0);
            }
        },
        ps,
        prsi);

    oops::Log::trace() << "AirPressureAtInterface_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
