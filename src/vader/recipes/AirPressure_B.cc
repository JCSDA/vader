/*
 * (C) Copyright 2026 UCAR.
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
#include "vader/recipes/AirPressure.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPressure_B::Name[] = "AirPressure_B";
const oops::Variables AirPressure_B::
                      Ingredients{std::vector<std::string>{"air_pressure_at_surface"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<AirPressure_B> makerAirPressure_B_(AirPressure_B::Name);

// -------------------------------------------------------------------------------------------------

AirPressure_B::AirPressure_B(const AirPressure_BParameters & params,
                             const VaderConfigVars & configVariables) :
                                    configVariables_{configVariables}
{
    oops::Log::trace() << "AirPressure_B::AirPressure_B Starting" << std::endl;
    oops::Log::trace() << "AirPressure_B::AirPressure_B Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressure_B::name() const {
    return AirPressure_B::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable AirPressure_B::product() const {
    return oops::Variable("air_pressure");
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressure_B::ingredients() const {
    return AirPressure_B::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPressure_B::productLevels(const atlas::FieldSet & afieldset) const {
    int nLevels = configVariables_.getInt("nLevels");
    return nLevels;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPressure_B::productFunctionSpace(const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_at_surface").functionspace();
}

// -------------------------------------------------------------------------------------------------

oops::Variables AirPressure_B::trajectoryVars() const {
    return oops::Variables{};
}

// -------------------------------------------------------------------------------------------------

void AirPressure_B::executeNL(atlas::FieldSet & afieldset) {
    oops::Log::trace() << "AirPressure_B::executeNL Starting" << std::endl;

    // Get the fields
    atlas::Field ps = afieldset.field("air_pressure_at_surface");
    atlas::Field prs = afieldset.field("air_pressure");

    // Get the units
    std::string prs_units, ps_units;
    ps.metadata().get("units", ps_units);
    prs.metadata().get("units", prs_units);

    // Assert that the units match
    ASSERT_MSG(prs_units == ps_units, "In Vader::AirPressureAtInterface_A::executeNL the units "
               "for pressure " + prs_units + "do not match the surface pressure units" + ps_units);

    // Get number of levels
    const int nLevels = configVariables_.getInt("nLevels");

    // Extract ak/bk from client config
    const std::vector<double> ak = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_a_coefficient");
    const std::vector<double> bk = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_b_coefficient");
    ASSERT(ak.size() == nLevels+1);
    ASSERT(bk.size() == nLevels+1);

    // Compute pressure at mid-levels from surface pressure and hybrid coefficients
    util::for_each_column(
        [&](const auto ps_col,
            auto prs_col) {
            for (int level = 0; level < nLevels; ++level) {
                prs_col(level) = 0.5 * (ak[level+1] + ak[level])
                               + 0.5 * (bk[level+1] + bk[level]) * ps_col(0);
            }
        },
        ps,
        prs);

    // Return
    oops::Log::trace() << "AirPressure_B::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirPressure_B::executeTL(atlas::FieldSet & afieldsetTL,
                        const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "AirPressure_B::executeTL Starting" << std::endl;

    // Get the fields
    atlas::Field ps_tl = afieldsetTL.field("air_pressure_at_surface");
    atlas::Field prs_tl = afieldsetTL.field("air_pressure");

    // Get number of levels
    const int nLevels = configVariables_.getInt("nLevels");

    // Extract bk from client config
    const std::vector<double> bk = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_b_coefficient");
    ASSERT(bk.size() == nLevels+1);

    // Compute TL of pressure at mid-levels
    util::for_each_column(
        [&](const auto ps_tl_col,
            auto prs_tl_col) {
            for (int level = 0; level < nLevels; ++level) {
                prs_tl_col(level) = 0.5 * (bk[level+1] + bk[level]) * ps_tl_col(0);
            }
        },
        ps_tl,
        prs_tl);

    oops::Log::trace() << "AirPressure_B::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirPressure_B::executeAD(atlas::FieldSet & afieldsetAD,
                        const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "AirPressure_B::executeAD Starting" << std::endl;

    // Get the fields
    atlas::Field ps_ad = afieldsetAD.field("air_pressure_at_surface");
    atlas::Field prs_ad = afieldsetAD.field("air_pressure");

    // Get number of levels
    const int nLevels = configVariables_.getInt("nLevels");

    // Extract bk from client config
    const std::vector<double> bk = configVariables_.getDoubleVector
                                                ("sigma_pressure_hybrid_coordinate_b_coefficient");
    ASSERT(bk.size() == nLevels+1);

    // Compute AD of pressure at mid-levels into surface pressure
    util::for_each_column(
        [&](auto ps_ad_col,
            auto prs_ad_col) {
            for (int level = 0; level < nLevels; ++level) {
                ps_ad_col(0) += 0.5 * (bk[level+1] + bk[level]) * prs_ad_col(level);
                prs_ad_col(level) = 0.0;
            }
        },
        ps_ad,
        prs_ad);

    oops::Log::trace() << "AirPressure_B::executeAD Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------
}  // namespace vader

// -------------------------------------------------------------------------------------------------
