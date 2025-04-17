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
  const auto mvView =
    make_view<const double, 2>(stateFlds["water_vapor_mixing_ratio_wrt_dry_air"]);
  const auto mciView =
    make_view<const double, 2>(stateFlds["cloud_ice_mixing_ratio_wrt_dry_air"]);
  const auto mclView =
    make_view<const double, 2>(stateFlds["cloud_liquid_water_mixing_ratio_wrt_dry_air"]);
  const auto mrView =
    make_view<const double, 2>(stateFlds["rain_mixing_ratio_wrt_dry_air"]);
  auto mtView = make_view<double, 2>(stateFlds["total_water_mixing_ratio_wrt_dry_air"]);
  const idx_t sizeOwned =
    util::getSizeOwned(stateFlds["total_water_mixing_ratio_wrt_dry_air"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    for (idx_t jl = 0; jl < mtView.shape(1); jl++) {
      mtView(jn, jl) = mvView(jn, jl) + mclView(jn, jl) + mciView(jn, jl) + mrView(jn, jl);
    }
  }
  stateFlds["total_water_mixing_ratio_wrt_dry_air"].set_dirty();

  oops::Log::trace()  << "[eval_total_water_mixing_ratio_wrt_dry_air_nl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_total_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl(
    atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] starting ..."
    << std::endl;
  const auto qvView = make_view<double, 2>(
    stateFlds["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qclView = make_view<double, 2>(
    stateFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qciView = make_view<double, 2>(
    stateFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qrainView = make_view<double, 2>(stateFlds["qrain"]);
  auto qtView = make_view<double, 2>(
    stateFlds["total_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);

  const atlas::idx_t n_levels =
    stateFlds["total_water_mixing_ratio_wrt_moist_air_and_condensed_water"].shape(1);
  const atlas::idx_t sizeOwned = util::getSizeOwned(
    stateFlds["total_water_mixing_ratio_wrt_moist_air_and_condensed_water"].functionspace());
  atlas_omp_parallel_for(atlas::idx_t ih = 0; ih < sizeOwned; ih++) {
    for (atlas::idx_t ilev = 0; ilev < n_levels; ilev++) {
      qtView(ih, ilev) =
        qvView(ih, ilev) + qclView(ih, ilev) + qciView(ih, ilev) + qrainView(ih, ilev);
    }
  }
  stateFlds["total_water_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();

  oops::Log::trace()
    << "[eval_total_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] ... exit"
    << std::endl;
}

}  // namespace mo
