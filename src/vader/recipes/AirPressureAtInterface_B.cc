 /*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirPressureAtInterface.h"

namespace vader
{

// Static attribute initialization
const char AirPressureAtInterface_B::Name[] = "AirPressureAtInterface_B";
const oops::Variables AirPressureAtInterface_B::Ingredients{
      std::vector<std::string>{"air_pressure_thickness"}};

// Register the maker
static RecipeMaker<AirPressureAtInterface_B>
       makerAirPressureAtInterface_B_(AirPressureAtInterface_B::Name);

// -------------------------------------------------------------------------------------------------

AirPressureAtInterface_B::AirPressureAtInterface_B(
                                                 const AirPressureAtInterface_BParameters & params,
                                        const VaderConfigVars & configVariables) :
                                        configVariables_{configVariables}
{
    oops::Log::trace() << "AirPressureAtInterface_B::AirPressureAtInterface_B(params) Starting"
                       << std::endl;
    oops::Log::trace() << "AirPressureAtInterface_B::AirPressureAtInterface_B(params) Done"
                       << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressureAtInterface_B::name() const {
    return AirPressureAtInterface_B::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable AirPressureAtInterface_B::product() const {
    return oops::Variable("air_pressure_levels");
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressureAtInterface_B::ingredients() const {
    return AirPressureAtInterface_B::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPressureAtInterface_B::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_thickness").shape(1)+1;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPressureAtInterface_B::productFunctionSpace
                                                        (const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_thickness").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirPressureAtInterface_B::executeNL(atlas::FieldSet & afieldset) {
    //
    oops::Log::trace() << "AirPressureAtInterface_B::executeNL Starting" << std::endl;

    const double ptop = configVariables_.getDouble("air_pressure_at_top_of_atmosphere_model");
    const int nLevels = configVariables_.getInt("nLevels");
    const bool topDown = configVariables_.getBool("levels_are_top_down");

    // Get the fields
    atlas::Field delp = afieldset.field("air_pressure_thickness");
    atlas::Field prsi = afieldset.field("air_pressure_levels");

    // Get the units
    std::string delp_units, prsi_units;
    delp.metadata().get("units", delp_units);
    prsi.metadata().get("units", prsi_units);

    // Assert that the units match
    ASSERT_MSG(prsi_units.empty() || delp_units.empty() || prsi_units == delp_units,
               "In Vader::AirPressureAtInterface_B::executeNL the units "
               "for pressure at the levels" + prsi_units + "do not match the pressure thickness "
               "units " + delp_units);

    if (topDown) {
        util::for_each_column(
            [&](const auto delp_col,
                auto prsi_col) {
                // Level 0 = model top;
                prsi_col(0) = ptop;
                for (int level = 1; level <= nLevels; ++level) {
                    prsi_col(level) = prsi_col(level-1) + delp_col(level-1);
                }
            },
            delp,
            prsi);
    } else {
        util::for_each_column(
            [&](const auto delp_col,
                auto prsi_col) {
                // Level 0 = surface;
                prsi_col(nLevels) = ptop;
                for (int level = nLevels-1; level >= 0; --level) {
                    prsi_col(level) = prsi_col(level+1) + delp_col(level);
                }
            },
            delp,
            prsi);
    }

    oops::Log::trace() << "AirPressureAtInterface_B::executeNL Done" << std::endl;
}

}  // namespace vader

// -------------------------------------------------------------------------------------------------
