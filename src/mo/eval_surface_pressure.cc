/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>

#include "atlas/array.h"
#include "atlas/parallel/omp/omp.h"

#include "mo/constants.h"
#include "mo/eval_surface_pressure.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {

void eval_surface_pressure_from_exner_tl(atlas::FieldSet & incFlds,
                                         const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_surface_pressure_from_exner_tl()] starting ..." << std::endl;
  // State Fields
  const auto pView = make_view<double, 2>(stateFlds["air_pressure_levels_minus_one"]);
  const auto exnerView = make_view<double, 2>(
    stateFlds["dimensionless_exner_function_levels_minus_one"]);

  // Increment Fields
  auto pstarIncView = make_view<double, 2>(incFlds["air_pressure_at_surface"]);
  const auto exnerIncView = make_view<double, 2>(
    incFlds["dimensionless_exner_function_levels_minus_one"]);

  const idx_t sizeOwned =
    util::getSizeOwned(incFlds["air_pressure_at_surface"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; ++jn) {
    pstarIncView(jn, 0) = exnerIncView(jn, 0) *  pView(jn, 0) /
      (constants::rd_over_cp * exnerView(jn, 0));
  }
  incFlds["air_pressure_at_surface"].set_dirty();
  oops::Log::trace() << "[eval_surface_pressure_from_exner_tl()] ... done" << std::endl;
}

void eval_surface_pressure_from_exner_ad(atlas::FieldSet & hatFlds,
                                         const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_surface_pressure_from_exner_ad()] starting ..." << std::endl;
  // State Fields
  const auto pView = make_view<double, 2>(stateFlds["air_pressure_levels_minus_one"]);
  const auto exnerView = make_view<double, 2>(
    stateFlds["dimensionless_exner_function_levels_minus_one"]);

  // Increment Fields
  auto pstarHatView = make_view<double, 2>(hatFlds["air_pressure_at_surface"]);
  auto exnerHatView = make_view<double, 2>(
    hatFlds["dimensionless_exner_function_levels_minus_one"]);

  const idx_t sizeOwned =
    util::getSizeOwned(hatFlds["air_pressure_at_surface"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; ++jn) {
    exnerHatView(jn, 0) += pstarHatView(jn, 0) * pView(jn, 0) /
      (constants::rd_over_cp * exnerView(jn, 0));
    pstarHatView(jn, 0) = 0.0;
  }
  hatFlds["air_pressure_at_surface"].set_dirty();
  hatFlds["dimensionless_exner_function_levels_minus_one"].set_dirty();
  oops::Log::trace() << "[eval_surface_pressure_from_exner_ad()] ... done" << std::endl;
}

void eval_surface_pressure_tl(atlas::FieldSet & incFlds) {
  oops::Log::trace() << "[eval_surface_pressure_tl()] starting ..." << std::endl;
  const auto pIncView = make_view<double, 2>(incFlds["air_pressure_levels"]);
  auto pstarIncView = make_view<double, 2>(incFlds["air_pressure_at_surface"]);
  const idx_t sizeOwned =
    util::getSizeOwned(incFlds["air_pressure_at_surface"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; ++jn) {
      pstarIncView(jn, 0) = pIncView(jn, 0);
  }
  incFlds["air_pressure_at_surface"].set_dirty();
  oops::Log::trace() << "[eval_surface_pressure_tl()] ... done" << std::endl;
}

void eval_surface_pressure_ad(atlas::FieldSet & hatFlds) {
  oops::Log::trace() << "[eval_surface_pressure_ad()] starting ..." << std::endl;
  auto pHatView = make_view<double, 2>(hatFlds["air_pressure_levels"]);
  auto pstarHatView = make_view<double, 2>(hatFlds["air_pressure_at_surface"]);
  const idx_t sizeOwned =
    util::getSizeOwned(hatFlds["air_pressure_at_surface"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; ++jn) {
    pHatView(jn, 0) += pstarHatView(jn, 0);
    pstarHatView(jn, 0) = 0.0;
  }
  hatFlds["air_pressure_levels"].set_dirty();
  hatFlds["air_pressure_at_surface"].set_dirty();
  oops::Log::trace() << "[eval_surface_pressure_ad()] ... done" << std::endl;
}

}  // namespace mo
