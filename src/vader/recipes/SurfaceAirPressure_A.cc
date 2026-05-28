/*
 * (C) Copyright 2021-2026 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/SurfaceAirPressure.h"

namespace vader
{

// Static attribute initialization
const char SurfaceAirPressure_A::Name[] = "SurfaceAirPressure_A";
const oops::Variables SurfaceAirPressure_A::Ingredients{
      std::vector<std::string>{"air_pressure_thickness"}};

// Register the maker
static RecipeMaker<SurfaceAirPressure_A> makerSurfaceAirPressure_A_(SurfaceAirPressure_A::Name);

// -------------------------------------------------------------------------------------------------

SurfaceAirPressure_A::SurfaceAirPressure_A(const SurfaceAirPressure_AParameters & params,
                                        const VaderConfigVars & configVariables) :
                                        configVariables_{configVariables} {
    oops::Log::trace() << "SurfaceAirPressure_A::SurfaceAirPressure_A Starting" << std::endl;
    oops::Log::trace() << "SurfaceAirPressure_A::SurfaceAirPressure_A Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string SurfaceAirPressure_A::name() const {
    return SurfaceAirPressure_A::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable SurfaceAirPressure_A::product() const {
    return oops::Variable{"air_pressure_at_surface"};
}

// -------------------------------------------------------------------------------------------------

oops::Variables SurfaceAirPressure_A::ingredients() const {
    return SurfaceAirPressure_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t SurfaceAirPressure_A::productLevels(const atlas::FieldSet & afieldset) const {
    return 1;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace SurfaceAirPressure_A::productFunctionSpace(const atlas::FieldSet & afieldset)
const {
    return afieldset.field("air_pressure_thickness").functionspace();
}

// -------------------------------------------------------------------------------------------------

void SurfaceAirPressure_A::executeNL(atlas::FieldSet & afieldset) {
    oops::Log::trace() << "SurfaceAirPressure_A::executeNL Starting" << std::endl;

    const double ptop = configVariables_.getDouble("air_pressure_at_top_of_atmosphere_model");

    util::for_each_column(
        [&](const auto delp_col,
            auto ps_col) {
            ps_col(0) = ptop;
            for (int k = 0; k < delp_col.shape(0); ++k) {
                ps_col(0) += delp_col(k);
            }
        },
        afieldset["air_pressure_thickness"],
        afieldset["air_pressure_at_surface"]);

    oops::Log::trace() << "SurfaceAirPressure_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SurfaceAirPressure_A::executeTL(atlas::FieldSet & afieldsetTL,
                               const atlas::FieldSet & /*afieldsetTraj*/) {
    oops::Log::trace() << "SurfaceAirPressure_A::executeTL Starting" << std::endl;

    util::for_each_column(
        [&](const auto delp_tl_col,
            auto ps_tl_col) {
            ps_tl_col(0) = 0.0;
            for (int k = 0; k < delp_tl_col.shape(0); ++k) {
                ps_tl_col(0) += delp_tl_col(k);
            }
        },
        afieldsetTL["air_pressure_thickness"],
        afieldsetTL["air_pressure_at_surface"]);

    oops::Log::trace() << "SurfaceAirPressure_A::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SurfaceAirPressure_A::executeAD(atlas::FieldSet & afieldsetAD,
                               const atlas::FieldSet & /*afieldsetTraj*/) {
    oops::Log::trace() << "SurfaceAirPressure_A::executeAD Starting" << std::endl;

    util::for_each_column(
        [&](auto ps_ad_col,
            auto delp_ad_col) {
            for (int k = 0; k < delp_ad_col.shape(0); ++k) {
                delp_ad_col(k) += ps_ad_col(0);
            }
            ps_ad_col(0) = 0.0;
        },
        afieldsetAD["air_pressure_at_surface"],
        afieldsetAD["air_pressure_thickness"]);

    oops::Log::trace() << "SurfaceAirPressure_A::executeAD Done" << std::endl;
}

}  // namespace vader

// -------------------------------------------------------------------------------------------------
