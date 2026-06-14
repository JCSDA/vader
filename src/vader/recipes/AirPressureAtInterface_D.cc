/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirPressureAtInterface.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPressureAtInterface_D::Name[] = "AirPressureAtInterface_D";
const oops::Variables AirPressureAtInterface_D::Ingredients{
      std::vector<std::string>{"air_pressure_at_surface"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<AirPressureAtInterface_D>
       makerAirPressureAtInterface_D_(AirPressureAtInterface_D::Name);

// -------------------------------------------------------------------------------------------------

AirPressureAtInterface_D::AirPressureAtInterface_D(
                                                 const AirPressureAtInterface_DParameters & params,
                                                 const VaderConfigVars & configVariables):
                                                 configVariables_{configVariables}
{
    oops::Log::trace() << "AirPressureAtInterface_D::AirPressureAtInterface_D Starting"
                       << std::endl;
    oops::Log::trace() << "AirPressureAtInterface_D::AirPressureAtInterface_D Done"
                       << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressureAtInterface_D::name() const {
    return AirPressureAtInterface_D::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable AirPressureAtInterface_D::product() const {
    return oops::Variable{"air_pressure_levels"};
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressureAtInterface_D::ingredients() const {
    return AirPressureAtInterface_D::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPressureAtInterface_D::productLevels(const atlas::FieldSet & afieldset) const {
    int nLevels = configVariables_.getInt("nLevels");
    return nLevels + 1;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPressureAtInterface_D::productFunctionSpace(const atlas::FieldSet
& afieldset) const {
    return afieldset.field("air_pressure_at_surface").functionspace();
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressureAtInterface_D::trajectoryVars() const {
    return oops::Variables{};
}

// -------------------------------------------------------------------------------------------------

void AirPressureAtInterface_D::executeNL(atlas::FieldSet & afieldset) {
    oops::Log::trace() << "AirPressureAtInterface_D::executeNL Starting" << std::endl;

    // Get the fields
    atlas::Field ps = afieldset.field("air_pressure_at_surface");
    atlas::Field prsi = afieldset.field("air_pressure_levels");

    // Get the units
    std::string prsi_units, ps_units;
    ps.metadata().get("units", ps_units);
    prsi.metadata().get("units", prsi_units);

    // Assert that the units match
    ASSERT_MSG(prsi_units == ps_units, "In Vader::AirPressureAtInterface_D::executeNL the units "
               "for pressure " + prsi_units + "do not match the surface pressure units" + ps_units);

    // Get number of levels
    const int nLevels = configVariables_.getInt("nLevels");

    // Extract ak/bk from client config
    const std::vector<double> ak = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_a_coefficient");
    const std::vector<double> bk = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_b_coefficient");
    ASSERT(ak.size() == nLevels+1);
    ASSERT(bk.size() == nLevels+1);

    // Compute interface pressures from surface pressure and hybrid coefficients
    util::for_each_column(
        [&](const auto ps_col,
            auto prsi_col) {
            for (int level = 0; level < nLevels + 1; ++level) {
                prsi_col(level) = ak[level] + bk[level] * ps_col(0);
            }
        },
        ps,
        prsi);

    // Return
    oops::Log::trace() << "AirPressureAtInterface_D::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirPressureAtInterface_D::executeTL(atlas::FieldSet & afieldsetTL,
                                     const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "AirPressureAtInterface_D::executeTL Starting" << std::endl;

    // Get the fields
    atlas::Field ps_tl = afieldsetTL.field("air_pressure_at_surface");
    atlas::Field prsi_tl = afieldsetTL.field("air_pressure_levels");

    // Get number of levels
    const int nLevels = configVariables_.getInt("nLevels");

    // Extract ak/bk from client config
    const std::vector<double> bk = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_b_coefficient");
    ASSERT(bk.size() == nLevels+1);

    // Compute TL of interface pressures
    util::for_each_column(
        [&](const auto ps_tl_col,
            auto prsi_tl_col) {
            for (int level = 0; level < nLevels + 1; ++level) {
                prsi_tl_col(level) = bk[level] * ps_tl_col(0);
            }
        },
        ps_tl,
        prsi_tl);

    oops::Log::trace() << "AirPressureAtInterface_D::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirPressureAtInterface_D::executeAD(atlas::FieldSet & afieldsetAD,
                                     const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "AirPressureAtInterface_D::executeAD Starting" << std::endl;

    // Get the fields
    atlas::Field ps_ad = afieldsetAD.field("air_pressure_at_surface");
    atlas::Field prsi_ad = afieldsetAD.field("air_pressure_levels");

    // Get number of levels
    const int nLevels = configVariables_.getInt("nLevels");

    // Extract ak/bk from client config
    const std::vector<double> bk = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_b_coefficient");
    ASSERT(bk.size() == nLevels+1);

    // Compute AD of interface pressures into surface pressure
    util::for_each_column(
        [&](auto ps_ad_col,
            auto prsi_ad_col) {
            for (int level = 0; level < nLevels + 1; ++level) {
                ps_ad_col(0) += bk[level] * prsi_ad_col(level);
                prsi_ad_col(level) = 0.0;
            }
        },
        ps_ad,
        prsi_ad);

    oops::Log::trace() << "AirPressureAtInterface_D::executeAD Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
