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
                                        const VaderConfigVars & configVariables) :
                                        configVariables_(configVariables) {
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
    ASSERT_MSG(prsi_units.empty() || delp_units.empty() || prsi_units == delp_units,
               "In Vader::AirPressureThickness_A::executeNL the "
               "units for pressure " + prsi_units +
               "do not match the pressure thickness units" + delp_units);

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const double sign = topDown ? 1.0 : -1.0;
    const int nLevels = configVariables_.getInt("nLevels");

    util::for_each_column(
        [&](const auto prsi_col,
            auto delp_col) {
            for (int level = 0; level < nLevels; ++level) {
                delp_col(level) = sign * (prsi_col(level+1) - prsi_col(level));
            }
        },
        prsi,
        delp);

    oops::Log::trace() << "leaving AirPressureThickness_A executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirPressureThickness_A::executeTL(atlas::FieldSet & afieldsetTL,
                                       const atlas::FieldSet & /*afieldsetTraj*/) {
    oops::Log::trace() << "AirPressureThickness_A::executeTL Starting" << std::endl;

    atlas::Field prsi_tl = afieldsetTL.field("air_pressure_levels");
    atlas::Field delp_tl = afieldsetTL.field("air_pressure_thickness");

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const double sign = topDown ? 1.0 : -1.0;
    const int nLevels = configVariables_.getInt("nLevels");

    util::for_each_column(
        [&](const auto prsi_tl_col,
            auto delp_tl_col) {
            for (int level = 0; level < nLevels; ++level) {
                delp_tl_col(level) = sign * (prsi_tl_col(level+1) - prsi_tl_col(level));
            }
        },
        prsi_tl,
        delp_tl);

    oops::Log::trace() << "AirPressureThickness_A::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirPressureThickness_A::executeAD(atlas::FieldSet & afieldsetAD,
                                       const atlas::FieldSet & /*afieldsetTraj*/) {
    oops::Log::trace() << "AirPressureThickness_A::executeAD Starting" << std::endl;

    atlas::Field prsi_ad = afieldsetAD.field("air_pressure_levels");
    atlas::Field delp_ad = afieldsetAD.field("air_pressure_thickness");

    const bool topDown = configVariables_.getBool("levels_are_top_down");
    const double sign = topDown ? 1.0 : -1.0;
    const int nLevels = configVariables_.getInt("nLevels");

    util::for_each_column(
        [&](auto delp_ad_col,
            auto prsi_ad_col) {
            for (int level = 0; level < nLevels; ++level) {
                prsi_ad_col(level+1) += sign * delp_ad_col(level);
                prsi_ad_col(level) -= sign * delp_ad_col(level);
                delp_ad_col(level) = 0.0;
            }
        },
        delp_ad,
        prsi_ad);

    oops::Log::trace() << "AirPressureThickness_A::executeAD Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
