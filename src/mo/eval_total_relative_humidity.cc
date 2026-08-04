/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>

#include "atlas/array.h"
#include "atlas/parallel/omp/omp.h"

#include "mo/eval_total_relative_humidity.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;

namespace mo {

void eval_total_relative_humidity_nl(atlas::FieldSet & fields) {
  oops::Log::trace() << "[eval_total_relative_humidity_nl()] starting ..." << std::endl;

  const auto qView = make_view<const double, 2>(
                              fields["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qclView = make_view<const double, 2>
                 (fields["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qciView = make_view<const double, 2>
                 (fields["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qrainView = make_view<const double, 2>(fields["qrain"]);
  const auto qsatView = make_view<const double, 2>(fields["qsat"]);
  auto rhtView = make_view<double, 2>(fields["rht"]);

  const atlas::idx_t n_levels(fields["rht"].shape(1));
  const atlas::idx_t sizeOwned =
        util::getSizeOwned(fields["rht"].functionspace());
  atlas_omp_parallel_for(atlas::idx_t ih = 0; ih < sizeOwned; ih++) {
    for (atlas::idx_t ilev = 0; ilev < n_levels; ilev++) {
      rhtView(ih, ilev) = (qView(ih, ilev) + qclView(ih, ilev) + qciView(ih, ilev)
                          + qrainView(ih, ilev)) / qsatView(ih, ilev);

      if (rhtView(ih, ilev) < 0.0) rhtView(ih, ilev) = 0.0;
    }
  }
  fields["rht"].set_dirty();
  oops::Log::trace() << "[eval_total_relative_humidity_nl()] ... exit" << std::endl;
}

}  // namespace mo
