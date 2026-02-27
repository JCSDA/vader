/*
 * (C) Copyright 2024  UCAR.
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

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPressureAtInterface_C::Name[] = "AirPressureAtInterface_C";
const oops::Variables AirPressureAtInterface_C::Ingredients{
      std::vector<std::string>{"air_pressure_at_surface", "air_pressure",
                               "geometric_height_of_layer_interfaces"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<AirPressureAtInterface_C>
       makerAirPressureAtInterface_C_(AirPressureAtInterface_C::Name);

// -------------------------------------------------------------------------------------------------

AirPressureAtInterface_C::AirPressureAtInterface_C(
                          const AirPressureAtInterface_CParameters & params,
                          const VaderConfigVars & configVariables):
                          configVariables_{configVariables}
{
    oops::Log::trace() << "AirPressureAtInterface_C::AirPressureAtInterface_C Starting"
                       << std::endl;
    oops::Log::trace() << "AirPressureAtInterface_C::AirPressureAtInterface_C Done"
                       << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressureAtInterface_C::name() const {
    return AirPressureAtInterface_C::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable AirPressureAtInterface_C::product() const {
    return oops::Variable("air_pressure_levels");
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressureAtInterface_C::ingredients() const {
    return AirPressureAtInterface_C::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPressureAtInterface_C::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("geometric_height_of_layer_interfaces").shape(1);
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPressureAtInterface_C::productFunctionSpace(const atlas::FieldSet
& afieldset) const {
    return afieldset.field("geometric_height_of_layer_interfaces").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirPressureAtInterface_C::executeNL(atlas::FieldSet & afieldset) {
    oops::Log::trace() << "AirPressureAtInterface_C::executeNL Starting" << std::endl;

    // Get the input fields
    atlas::Field ps = afieldset.field("air_pressure_at_surface");
    atlas::Field p = afieldset.field("air_pressure");
    atlas::Field zgrid = afieldset.field("geometric_height_of_layer_interfaces");
    // output:
    atlas::Field prsi = afieldset.field("air_pressure_levels");

    // Get and set the units
    std::string p_units, prsi_units;
    p.metadata().get("units", p_units);
    prsi.metadata().set("units", p_units);

    const int nLevel = zgrid.shape(1) - 1;  // Reduce by 1 since index begins at 0

    util::for_each_column(
        [&](const auto p_col,
            const auto ps_col,
            const auto zgrid_col,
            auto prsi_col) {
            // Set pressure at the bottom to surface pressure
            prsi_col(nLevel) = ps_col(0);
            // Interpolate linearly pressure log(p) to geometrical height levels (zgrid)
            for (int level = nLevel-1; level >= 1; --level) {
                double w1 = (zgrid_col(level) - zgrid_col(level+1))
                            /(zgrid_col(level-1) - zgrid_col(level+1));
                double w2 = 1.0 - w1;
                prsi_col(level) = std::exp(w1*std::log(p_col(level)) + w2*std::log(p_col(level+1)));
            }
            // Extrapolate for the top level
            double z0 = zgrid_col(0);
            double z1 = 0.5*(z0 + zgrid_col(1));
            double z2 = 0.5*(z1 + zgrid_col(2));
            double w1 = (z0-z2)/(z1-z2);
            double w2 = 1.0 - w1;
            prsi_col(0) = std::exp(w1*std::log(p_col(1))
                                   + w2*std::log(p_col(2)) );
        },
        p,
        ps,
        zgrid,
        prsi);

    oops::Log::trace() << "AirPressureAtInterface_C::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
