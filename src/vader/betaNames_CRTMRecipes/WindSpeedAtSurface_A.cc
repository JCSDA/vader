/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/WindSpeedAtSurface.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WindSpeedAtSurface_A::Name[] = "WindSpeedAtSurface_A";
const oops::Variables WindSpeedAtSurface_A::Ingredients{std::vector<std::string>{
                            "eastward_wind_at_surface", "northward_wind_at_surface"}};

// Constants
static constexpr double epsilon = 1.0e-10;  // Tolerance for zero wind checks

// Register the maker
static RecipeMaker<WindSpeedAtSurface_A> makerWindSpeedAtSurface_A_(
                                WindSpeedAtSurface_A::Name);

WindSpeedAtSurface_A::WindSpeedAtSurface_A(const Parameters_ & params,
                   const VaderConfigVars & configVariables) {
  oops::Log::trace() << "WindSpeedAtSurface_A::WindSpeedAtSurface_A(params)"
                     << std::endl;
}

std::string WindSpeedAtSurface_A::name() const {
  return WindSpeedAtSurface_A::Name;
}

oops::Variable WindSpeedAtSurface_A::product() const {
  return oops::Variable{"wind_speed_at_surface"};
}

oops::Variables WindSpeedAtSurface_A::ingredients() const {
  return WindSpeedAtSurface_A::Ingredients;
}

oops::Variables WindSpeedAtSurface_A::trajectoryVars() const {
  return WindSpeedAtSurface_A::Ingredients;
}

size_t WindSpeedAtSurface_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace WindSpeedAtSurface_A::productFunctionSpace(const atlas::FieldSet &
                                          afieldset) const {
  return afieldset.field("eastward_wind_at_surface").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAtSurface_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "WindSpeedAtSurface_A::executeNL starting" << std::endl;

  util::for_each_column(
    [](const auto u_col,
       const auto v_col,
       auto speed_col) {
       speed_col(0) = std::sqrt(u_col(0) * u_col(0) + v_col(0) * v_col(0));
    },
    afieldset["eastward_wind_at_surface"],
    afieldset["northward_wind_at_surface"],
    afieldset["wind_speed_at_surface"]);

  oops::Log::trace() << "WindSpeedAtSurface_A::executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAtSurface_A::executeTL(atlas::FieldSet & afieldsetTL,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindSpeedAtSurface_A::executeTL starting" << std::endl;

  util::for_each_column(
    [&](const auto u_col,
        const auto v_col,
        const auto u_tl_col,
        const auto v_tl_col,
        auto speed_tl_col) {
        double speed = std::sqrt(u_col(0) * u_col(0) + v_col(0) * v_col(0));
        if (speed > epsilon) {
          speed_tl_col(0) = (u_tl_col(0) * u_col(0) + v_tl_col(0) * v_col(0)) / speed;
        } else {
          speed_tl_col(0) = 0.0;
        }
    },
    afieldsetTraj["eastward_wind_at_surface"],
    afieldsetTraj["northward_wind_at_surface"],
    afieldsetTL["eastward_wind_at_surface"],
    afieldsetTL["northward_wind_at_surface"],
    afieldsetTL["wind_speed_at_surface"]);

  oops::Log::trace() << "WindSpeedAtSurface_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAtSurface_A::executeAD(atlas::FieldSet & afieldsetAD,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindSpeedAtSurface_A::executeAD starting" << std::endl;

  util::for_each_column(
    [&](const auto u_col,
        const auto v_col,
        auto u_ad_col,
        auto v_ad_col,
        auto speed_ad_col) {
        double speed = std::sqrt(u_col(0) * u_col(0) + v_col(0) * v_col(0));
        if (speed > epsilon) {
          u_ad_col(0) += (u_col(0) / speed) * speed_ad_col(0);
          v_ad_col(0) += (v_col(0) / speed) * speed_ad_col(0);
        }
        speed_ad_col(0) = 0.0;
    },
    afieldsetTraj["eastward_wind_at_surface"],
    afieldsetTraj["northward_wind_at_surface"],
    afieldsetAD["eastward_wind_at_surface"],
    afieldsetAD["northward_wind_at_surface"],
    afieldsetAD["wind_speed_at_surface"]);

  oops::Log::trace() << "WindSpeedAtSurface_A::executeAD done" << std::endl;
}

}  // namespace vader
