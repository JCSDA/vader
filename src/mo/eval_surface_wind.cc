/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>

#include "atlas/array.h"
#include "atlas/parallel/omp/omp.h"

#include "mo/eval_surface_wind.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {

void eval_surface_wind_tl(atlas::FieldSet & incFlds) {
  oops::Log::trace() << "[eval_surface_wind_tl()] starting ..." << std::endl;
  const auto uIncView = make_view<double, 2>(incFlds["eastward_wind"]);
  const auto vIncView = make_view<double, 2>(incFlds["northward_wind"]);
  auto u10mIncView = make_view<double, 2>(incFlds["eastward_wind_at_10m"]);
  auto v10mIncView = make_view<double, 2>(incFlds["northward_wind_at_10m"]);
  const idx_t sizeOwned =
    util::getSizeOwned(incFlds["eastward_wind_at_10m"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; ++jn) {
      u10mIncView(jn, 0) = uIncView(jn, 0);
      v10mIncView(jn, 0) = vIncView(jn, 0);
  }
  incFlds["eastward_wind_at_10m"].set_dirty();
  incFlds["northward_wind_at_10m"].set_dirty();

  oops::Log::trace() << "[eval_surface_wind_tl()] ... done" << std::endl;
}

void eval_surface_wind_ad(atlas::FieldSet & hatFlds) {
  oops::Log::trace() << "[eval_surface_wind_ad()] starting ..." << std::endl;
  auto uHatView = make_view<double, 2>(hatFlds["eastward_wind"]);
  auto vHatView = make_view<double, 2>(hatFlds["northward_wind"]);
  auto u10mHatView = make_view<double, 2>(hatFlds["eastward_wind_at_10m"]);
  auto v10mHatView = make_view<double, 2>(hatFlds["northward_wind_at_10m"]);
  const idx_t sizeOwned =
    util::getSizeOwned(hatFlds["eastward_wind_at_10m"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; ++jn) {
      uHatView(jn, 0) += u10mHatView(jn, 0);
      vHatView(jn, 0) += v10mHatView(jn, 0);
      u10mHatView(jn, 0) = 0.0;
      v10mHatView(jn, 0) = 0.0;
  }
  hatFlds["eastward_wind"].set_dirty();
  hatFlds["northward_wind"].set_dirty();
  hatFlds["eastward_wind_at_10m"].set_dirty();
  hatFlds["northward_wind_at_10m"].set_dirty();

  oops::Log::trace() << "[eval_surface_wind_ad()] ... done" << std::endl;
}

}  // namespace mo
