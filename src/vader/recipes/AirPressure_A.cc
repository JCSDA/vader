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

oops::Variables AirPressure_A::trajectoryVars() const {
    return oops::Variables{std::vector<std::string>{"air_pressure_levels"}};
}

// -------------------------------------------------------------------------------------------------

void AirPressure_A::executeNL(atlas::FieldSet & afieldset) {
    oops::Log::trace() << "AirPressure_A::executeNL Starting" << std::endl;

    // Extract value from client config
    const double kappa = configVariables_.getDouble("kappa");  // Need better name
    const bool topDown = configVariables_.getBool("levels_are_top_down");

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
                // pu = higher pressure (bottom of layer), pl = lower pressure (top of layer)
                const double pu = topDown ? airPressureLevels_col(level+1)
                                          : airPressureLevels_col(level);
                const double pl = topDown ? airPressureLevels_col(level)
                                          : airPressureLevels_col(level+1);
                airPressure_col(level) = std::pow(((std::pow(pu, kap1) -
                                                    std::pow(pl, kap1)) /
                                                    (kap1*(pu - pl))), kapr);
            }
        },
        airPressureLevels,
        airPressure);

    oops::Log::trace() << "AirPressure_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirPressure_A::executeTL(atlas::FieldSet & afieldsetTL,
                        const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "AirPressure_A::executeTL Starting" << std::endl;

    // Extract value from client config
    const double kappa = configVariables_.getDouble("kappa");  // Need better name
    const bool topDown = configVariables_.getBool("levels_are_top_down");

    // kappa variations
    const double kap1 = kappa + 1.0;
    const double kapr = 1.0 / kappa;

    // Get fields
    atlas::Field airPressureLevelsTR = afieldsetTraj.field("air_pressure_levels");
    atlas::Field airPressureLevelsTL = afieldsetTL.field("air_pressure_levels");
    atlas::Field airPressureTL = afieldsetTL.field("air_pressure");

    const int v_size = airPressureLevelsTR.shape(1) - 1;

    // Calculate the output variable
    util::for_each_column(
        [&](const auto airPressureLevels_col,
            const auto airPressureLevels_tl_col,
            auto airPressure_tl_col) {
            for (int vv = 0; vv < v_size; ++vv) {
                // pu = higher pressure (bottom of layer), pl = lower pressure (top of layer)
                const int pu_idx = topDown ? vv+1 : vv;
                const int pl_idx = topDown ? vv : vv+1;

                double pu = airPressureLevels_col(pu_idx);
                double pl = airPressureLevels_col(pl_idx);

                double pu_tl = airPressureLevels_tl_col(pu_idx);
                double pl_tl = airPressureLevels_tl_col(pl_idx);

                double A = pow(pu, kap1);
                double B = pow(pl, kap1);

                double dA = kap1 * pow(pu, kap1-1) * pu_tl;
                double dB = kap1 * pow(pl, kap1-1) * pl_tl;

                double denom = kap1 * (pu - pl);
                double S = (A - B) / denom;

                double dS =
                  ((dA - dB) * (pu - pl) - (A - B) * (pu_tl - pl_tl)) /
                  (kap1 * (pu - pl) * (pu - pl));

                airPressure_tl_col(vv) = kapr * pow(S, kapr-1) * dS;
            }
        },
        airPressureLevelsTR,
        airPressureLevelsTL,
        airPressureTL);
    oops::Log::trace() << "AirPressure_A::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void AirPressure_A::executeAD(atlas::FieldSet & afieldsetAD,
                        const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "AirPressure_A::executeAD Starting" << std::endl;

    // Extract value from client config
    const double kappa = configVariables_.getDouble("kappa");  // Need better name
    const bool topDown = configVariables_.getBool("levels_are_top_down");

    // kappa variations
    const double kap1 = kappa + 1.0;
    const double kapr = 1.0 / kappa;

    // Get fields
    atlas::Field airPressureLevelsTR = afieldsetTraj.field("air_pressure_levels");
    atlas::Field airPressureLevelsAD = afieldsetAD.field("air_pressure_levels");
    atlas::Field airPressureAD = afieldsetAD.field("air_pressure");

    const int v_size = airPressureLevelsTR.shape(1) - 1;

    // Calculate the output variable
    util::for_each_column(
        [&](const auto airPressureLevels_col,
            auto airPressureLevels_ad_col,
            auto airPressure_ad_col) {
            for (int vv = v_size-1; vv >= 0; --vv) {
                // pu = higher pressure (bottom of layer), pl = lower pressure (top of layer)
                const int pu_idx = topDown ? vv+1 : vv;
                const int pl_idx = topDown ? vv : vv+1;

                double pu = airPressureLevels_col(pu_idx);
                double pl = airPressureLevels_col(pl_idx);

                double A = pow(pu, kap1);
                double B = pow(pl, kap1);

                double denom = kap1 * (pu - pl);
                double S = (A - B) / denom;

                double y_ad = airPressure_ad_col(vv);
                airPressure_ad_col(vv) = 0.0;
                double S_ad = kapr * pow(S, kapr-1) * y_ad;
                double A_ad = S_ad / denom;
                double B_ad = -S_ad / denom;

                double pu_ad = 0.0;
                double pl_ad = 0.0;

                pu_ad -= S_ad * (A - B) / (kap1 * (pu - pl) * (pu - pl));
                pl_ad += S_ad * (A - B) / (kap1 * (pu - pl) * (pu - pl));

                pu_ad += A_ad * kap1 * pow(pu, kap1-1);
                pl_ad += B_ad * kap1 * pow(pl, kap1-1);

                airPressureLevels_ad_col(pu_idx) += pu_ad;
                airPressureLevels_ad_col(pl_idx) += pl_ad;
            }
        },
        airPressureLevelsTR,
        airPressureLevelsAD,
        airPressureAD);

    oops::Log::trace() << "AirPressure_A::executeAD Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
