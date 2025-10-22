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
#include "mo/eval_surface_temperature.h"

#include "oops/util/for_each.h"
#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

using View = atlas::array::LocalView<double, 1>;
using ConstView = atlas::array::LocalView<const double, 1>;

namespace mo {

void eval_surface_temperature_from_exner_tl(atlas::FieldSet & incFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_surface_temperature_from_exner_tl()] ... starting" << std::endl;

  util::for_each_column(
    [=] (ConstView theta,
         ConstView exnerLevels,
         ConstView h,
         ConstView hl,
         ConstView thetaInc,
         ConstView exnerLevelsInc,
         View tInc) {
      tInc(0) = (
        ((h(0) - hl(0)) * exnerLevels(1) + (hl(1) - h(0)) * exnerLevels(0)) * thetaInc(0) +
        ((h(0) - hl(0)) * exnerLevelsInc(1) + (hl(1) - h(0)) * exnerLevelsInc(0)) * theta(0)) /
        (hl(1) - hl(0));
    },
    stateFlds["air_potential_temperature"],
    stateFlds["dimensionless_exner_function_levels_minus_one"],
    stateFlds["height_above_mean_sea_level"],
    stateFlds["height_above_mean_sea_level_levels"],
    incFlds["air_potential_temperature"],
    incFlds["dimensionless_exner_function_levels_minus_one"],
    incFlds["air_temperature_at_2m"]);

  incFlds["air_temperature_at_2m"].set_dirty();

  oops::Log::trace() << "[eval_air_temperature__at_2m()] ... exit" << std::endl;
}

void eval_surface_temperature_from_exner_ad(atlas::FieldSet & hatFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_surface_temperature_from_exner_ad()] ... starting" << std::endl;

  util::for_each_column(
    [=] (ConstView theta,
         ConstView exnerLevels,
         ConstView h,
         ConstView hl,
         View tHat,
         View thetaHat,
         View exnerLevelsHat) {
      thetaHat(0) += ((h(0) - hl(0)) * exnerLevels(1) + (hl(1) - h(0)) * exnerLevels(0)) *
          tHat(0) / (hl(1) - hl(0));

      exnerLevelsHat(1) += (((h(0) - hl(0)) * tHat(0)) * theta(0)) / (hl(1) - hl(0));
      exnerLevelsHat(0) += (((hl(1) - h(0)) * tHat(0)) * theta(0)) / (hl(1) - hl(0));
      tHat(0) = 0.0;
    },
    stateFlds["air_potential_temperature"],
    stateFlds["dimensionless_exner_function_levels_minus_one"],
    stateFlds["height_above_mean_sea_level"],
    stateFlds["height_above_mean_sea_level_levels"],
    hatFlds["air_temperature_at_2m"],
    hatFlds["air_potential_temperature"],
    hatFlds["dimensionless_exner_function_levels_minus_one"]);

  hatFlds["air_temperature_at_2m"].set_dirty();
  hatFlds["air_potential_temperature"].set_dirty();
  hatFlds["dimensionless_exner_function_levels_minus_one"].set_dirty();

  oops::Log::trace() << "[eval_air_temperature__at_2m()] ... exit" << std::endl;
}

}  // namespace mo
