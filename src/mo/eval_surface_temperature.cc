/*
 * (C) Crown Copyright 2023-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>

#include "atlas/array.h"
#include "atlas/parallel/omp/omp.h"

#include "mo/constants.h"
#include "mo/eval_surface_temperature.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {

void eval_surface_temperature_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_surface_temperature_nl()] starting ..." << std::endl;
  const auto temperView = make_view<double, 2>(stateFlds["air_temperature"]);
  auto surfTemperView = make_view<double, 2>(stateFlds["air_temperature_at_2m"]);
  const idx_t sizeOwned =
    util::getSizeOwned(stateFlds["air_temperature_at_2m"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; ++jn) {
    surfTemperView(jn, 0) = temperView(jn, 0);
  }
  stateFlds["air_temperature_at_2m"].set_dirty();

  oops::Log::trace() << "[eval_surface_temperature_nl()] ... done" << std::endl;
}

void eval_surface_temperature_tl(atlas::FieldSet & incFlds) {
  oops::Log::trace() << "[eval_surface_temperature_tl()] starting ..." << std::endl;
  const auto temperIncView = make_view<double, 2>(incFlds["air_temperature"]);
  auto surfTemperIncView = make_view<double, 2>(incFlds["air_temperature_at_2m"]);
  const idx_t sizeOwned =
    util::getSizeOwned(incFlds["air_temperature_at_2m"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; ++jn) {
      surfTemperIncView(jn, 0) = temperIncView(jn, 0);
  }
  incFlds["air_temperature_at_2m"].set_dirty();
  oops::Log::trace() << "[eval_surface_temperature_tl()] ... done" << std::endl;
}

void eval_surface_temperature_ad(atlas::FieldSet & hatFlds) {
  oops::Log::trace() << "[eval_surface_temperature_ad()] starting ..." << std::endl;
  auto temperHatView = make_view<double, 2>(hatFlds["air_temperature"]);
  auto surfTemperHatView = make_view<double, 2>(hatFlds["air_temperature_at_2m"]);
  const idx_t sizeOwned =
    util::getSizeOwned(hatFlds["air_temperature_at_2m"].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; ++jn) {
    temperHatView(jn, 0) += surfTemperHatView(jn, 0);
    surfTemperHatView(jn, 0) = 0.0;
  }
  hatFlds["air_temperature"].set_dirty();
  hatFlds["air_temperature_at_2m"].set_dirty();

  oops::Log::trace() << "[eval_surface_temperature_ad()] ... done" << std::endl;
}

void eval_surface_temperature_from_exner_nl(atlas::FieldSet & stateFlds) {
  const auto thetaView = make_view<const double, 2>(stateFlds["air_potential_temperature"]);
  const auto exnerView = make_view<const double, 2>(stateFlds["dimensionless_exner_function"]);
  auto ds_atemp = make_view<double, 2>(stateFlds["air_temperature_at_2m"]);

  auto fspace = stateFlds["air_temperature_at_2m"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);

  idx_t lvls(stateFlds["air_temperature_at_2m"].shape(1));
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < lvls; ++jl) {
      ds_atemp(jn, jl) = thetaView(jn, jl) * exnerView(jn, jl);
    }
  }
  stateFlds["air_temperature_at_2m"].set_dirty();
}

void eval_surface_temperature_from_exner_tl(atlas::FieldSet & incFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_surface_temperature_from_exner_tl()] ... starting" << std::endl;
  // State Fields
  const auto thetaView = make_view<double, 2>(stateFlds["air_potential_temperature"]);
  const auto exnerLevelsView = make_view<double, 2>(
    stateFlds["dimensionless_exner_function_levels_minus_one"]);
  const auto hView = make_view<double, 2>(stateFlds["height_above_mean_sea_level"]);
  const auto hlView = make_view<double, 2>(stateFlds["height_above_mean_sea_level_levels"]);

  // Increment Fields
  auto tIncView = make_view<double, 2>(incFlds["air_temperature_at_2m"]);
  const auto thetaIncView = make_view<double, 2>(incFlds["air_potential_temperature"]);
  const auto exnerLevelsIncView = make_view<double, 2>(
    incFlds["dimensionless_exner_function_levels_minus_one"]);

  const idx_t sizeOwned = util::getSizeOwned(incFlds["air_temperature_at_2m"].functionspace());

  // Active code
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    tIncView(jn, 0) = (
      ((hView(jn, 0) - hlView(jn, 0))  * exnerLevelsView(jn, 1) +
      (hlView(jn, 1)  - hView(jn, 0)) * exnerLevelsView(jn, 0)) *
      thetaIncView(jn, 0) +
      ((hView(jn, 0) - hlView(jn, 0)) * exnerLevelsIncView(jn, 1) +
      (hlView(jn, 1)  - hView(jn, 0)) * exnerLevelsIncView(jn, 0)) *
      thetaView(jn, 0)) /
      (hlView(jn, 1) - hlView(jn, 0));
  }
  incFlds["air_temperature_at_2m"].set_dirty();
  oops::Log::trace() << "[eval_air_temperature__at_2m()] ... exit" << std::endl;
}

void eval_surface_temperature_from_exner_ad(atlas::FieldSet & hatFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_surface_temperature_from_exner_ad()] ... starting" << std::endl;
  // State Fields
  const auto thetaView = make_view<double, 2>(stateFlds["air_potential_temperature"]);
  const auto exnerLevelsView = make_view<double, 2>(
    stateFlds["dimensionless_exner_function_levels_minus_one"]);
  const auto hView = make_view<double, 2>(stateFlds["height_above_mean_sea_level"]);
  const auto hlView = make_view<double, 2>(stateFlds["height_above_mean_sea_level_levels"]);

  // Increment Fields
  auto tHatView = make_view<double, 2>(hatFlds["air_temperature_at_2m"]);
  auto thetaHatView = make_view<double, 2>(hatFlds["air_potential_temperature"]);
  auto exnerLevelsHatView = make_view<double, 2>(
    hatFlds["dimensionless_exner_function_levels_minus_one"]);

  const idx_t sizeOwned = util::getSizeOwned(hatFlds["air_temperature_at_2m"].functionspace());

  // Active code
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    thetaHatView(jn, 0) += (
      ( (hView(jn, 0) - hlView(jn, 0))  * exnerLevelsView(jn, 1) +
        (hlView(jn, 1)  - hView(jn, 0)) * exnerLevelsView(jn, 0)) *
        tHatView(jn, 0)) /
        (hlView(jn, 1) - hlView(jn, 0));

    exnerLevelsHatView(jn, 1) += (((hView(jn, 0) - hlView(jn, 0)) * tHatView(jn, 0)) *
      thetaView(jn, 0)) / (hlView(jn, 1) - hlView(jn, 0));

    exnerLevelsHatView(jn, 0) += (((hlView(jn, 1)  - hView(jn, 0)) * tHatView(jn, 0)) *
      thetaView(jn, 0)) / (hlView(jn, 1) - hlView(jn, 0));
    tHatView(jn, 0) = 0.0;
  }
  hatFlds["air_temperature_at_2m"].set_dirty();
  hatFlds["air_potential_temperature"].set_dirty();
  hatFlds["dimensionless_exner_function_levels_minus_one"].set_dirty();
  oops::Log::trace() << "[eval_air_temperature__at_2m()] ... exit" << std::endl;
}

}  // namespace mo
