/*
 * (C) Crown Copyright 2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "atlas/array/MakeView.h"
#include "atlas/field.h"
#include "atlas/parallel/omp/omp.h"

#include "mo/eval_total_water_mixing_ratio.h"

#include "oops/util/for_each.h"
#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

namespace mo {

using atlas::array::make_view;
using atlas::idx_t;

// --------------------------------------------------------------------------------------

void eval_total_water_mixing_ratio_wrt_dry_air_nl(
    atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_total_water_mixing_ratio_wrt_dry_air_nl()] starting ..." << std::endl;

  util::for_each_value(
    [](const double& mvView, const double& mclView,
       const double& mciView, const double& mrView,
       double& mtView) {
      mtView = mvView + mclView + mciView + mrView;
    },
    stateFlds["water_vapor_mixing_ratio_wrt_dry_air"],
    stateFlds["cloud_liquid_water_mixing_ratio_wrt_dry_air"],
    stateFlds["cloud_ice_mixing_ratio_wrt_dry_air"],
    stateFlds["rain_mixing_ratio_wrt_dry_air"],
    stateFlds["total_water_mixing_ratio_wrt_dry_air"]);

  stateFlds["total_water_mixing_ratio_wrt_dry_air"].set_dirty();

  oops::Log::trace()  << "[eval_total_water_mixing_ratio_wrt_dry_air_nl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_total_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl(
    atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] starting ..."
    << std::endl;

  util::for_each_value(
    [](const double& qvView, const double& qclView,
       const double& qciView, const double& qrainView,
       double& qtView) {
      qtView = qvView + qclView + qciView + qrainView;
    },
    stateFlds["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"],
    stateFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"],
    stateFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"],
    stateFlds["qrain"],
    stateFlds["total_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);

  stateFlds["total_water_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();

  oops::Log::trace()
    << "[eval_total_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] ... exit"
    << std::endl;
}

}  // namespace mo
