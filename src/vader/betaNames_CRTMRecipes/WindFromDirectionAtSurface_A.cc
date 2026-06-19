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
#include "vader/betaNames_CRTMRecipes/WindFromDirectionAtSurface.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WindFromDirectionAtSurface_A::Name[] = "WindFromDirectionAtSurface_A";
const oops::Variables WindFromDirectionAtSurface_A::Ingredients{std::vector<std::string>{
                            "eastward_wind_at_surface",
                            "northward_wind_at_surface"}};

// Constants
static constexpr double rad_to_deg = 180.0 / M_PI;
static constexpr double epsilon = 1.0e-10;  // Tolerance for zero wind checks

// Register the maker
static RecipeMaker<WindFromDirectionAtSurface_A> makerWindFromDirectionAtSurface_A_(
                                WindFromDirectionAtSurface_A::Name);

WindFromDirectionAtSurface_A::WindFromDirectionAtSurface_A(const Parameters_ & params,
                   const VaderConfigVars & configVariables) {
  oops::Log::trace() << "WindFromDirectionAtSurface_A::WindFromDirectionAtSurface_A(params)"
                     << std::endl;
}

std::string WindFromDirectionAtSurface_A::name() const {
  return WindFromDirectionAtSurface_A::Name;
}

oops::Variable WindFromDirectionAtSurface_A::product() const {
  return oops::Variable{"wind_from_direction_at_surface"};
}

oops::Variables WindFromDirectionAtSurface_A::ingredients() const {
  return WindFromDirectionAtSurface_A::Ingredients;
}

oops::Variables WindFromDirectionAtSurface_A::trajectoryVars() const {
  return WindFromDirectionAtSurface_A::Ingredients;
}

size_t WindFromDirectionAtSurface_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace WindFromDirectionAtSurface_A::productFunctionSpace(const atlas::FieldSet &
                                          afieldset) const {
  return afieldset.field("eastward_wind_at_surface").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WindFromDirectionAtSurface_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "WindFromDirectionAtSurface_A::executeNL starting" << std::endl;

  util::for_each_value(
    [&](const double uu,
        const double vv,
        double& dir) {
        // Wind direction: atan2(-uu, -vv) gives direction wind is coming FROM
        // Convert to degrees and ensure 0-360 range
        dir = std::atan2(-uu, -vv) * rad_to_deg;
        if (dir < 0.0) {
          dir += 360.0;
        }
    },
    afieldset["eastward_wind_at_surface"],
    afieldset["northward_wind_at_surface"],
    afieldset["wind_from_direction_at_surface"]);

  oops::Log::trace() << "WindFromDirectionAtSurface_A::executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindFromDirectionAtSurface_A::executeTL(atlas::FieldSet & afieldsetTL,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindFromDirectionAtSurface_A::executeTL starting" << std::endl;

  util::for_each_value(
    [&](const double uu,
        const double vv,
        const double uu_tl,
        const double vv_tl,
        double& dir_tl) {
        double speed_sq = uu * uu + vv * vv;
        if (speed_sq > epsilon) {
          // TL derivatives of direction = atan2(-u, -v) * 180/pi with respect to u and v:
          // Using chain rule: d(direction)/du = d(atan2(-uu,-vv))/du * 180/pi
          //                   d(direction)/dv = d(atan2(-uu,-vv))/dv * 180/pi
          // Derivatives: d(atan2(-uu,-vv))/du = vv/speed_sq
          //              d(atan2(-uu,-vv))/dv = -uu/speed_sq
          dir_tl = (vv * uu_tl / speed_sq - uu * vv_tl / speed_sq) * rad_to_deg;
        } else {
          dir_tl = 0.0;
        }
    },
    afieldsetTraj["eastward_wind_at_surface"],
    afieldsetTraj["northward_wind_at_surface"],
    afieldsetTL["eastward_wind_at_surface"],
    afieldsetTL["northward_wind_at_surface"],
    afieldsetTL["wind_from_direction_at_surface"]);

  oops::Log::trace() << "WindFromDirectionAtSurface_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindFromDirectionAtSurface_A::executeAD(atlas::FieldSet & afieldsetAD,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindFromDirectionAtSurface_A::executeAD starting" << std::endl;

  util::for_each_value(
    [&](const double uu,
        const double vv,
        double& uu_ad,
        double& vv_ad,
        double& dir_ad) {
        double speed_sq = uu * uu + vv * vv;
        if (speed_sq > epsilon) {
          // AD: Apply adjoint of the TL operation
          double du_contrib = vv / speed_sq;
          double dv_contrib = -uu / speed_sq;
          uu_ad += du_contrib * dir_ad * rad_to_deg;
          vv_ad += dv_contrib * dir_ad * rad_to_deg;
        }
        dir_ad = 0.0;
    },
    afieldsetTraj["eastward_wind_at_surface"],
    afieldsetTraj["northward_wind_at_surface"],
    afieldsetAD["eastward_wind_at_surface"],
    afieldsetAD["northward_wind_at_surface"],
    afieldsetAD["wind_from_direction_at_surface"]);

  oops::Log::trace() << "WindFromDirectionAtSurface_A::executeAD done" << std::endl;
}

}  // namespace vader
