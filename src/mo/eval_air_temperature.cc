/*
 * (C) Crown Copyright 2023-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "atlas/field.h"
#include "atlas/functionspace.h"

#include "mo/constants.h"
#include "mo/eval_air_temperature.h"

#include "oops/util/for_each.h"
#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

using View = atlas::array::LocalView<double, 1>;
using ConstView = atlas::array::LocalView<const double, 1>;

namespace mo {

/// \details Calculates air temperature from potential temperature and exner.
void eval_air_temperature_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_temperature_nl()] starting ..." << std::endl;

  idx_t lvls(stateFlds["air_temperature"].shape(1));
  util::for_each_column(
    [=] (ConstView ds_theta, ConstView ds_exner, View ds_atemp) {
      for (atlas::idx_t jl = 0; jl < lvls; ++jl) {
        ds_atemp(jl) = ds_theta(jl) * ds_exner(jl);
      }
    },
    stateFlds["air_potential_temperature"],
    stateFlds["dimensionless_exner_function"],
    stateFlds["air_temperature"]);

  stateFlds["air_temperature"].set_dirty();
  oops::Log::trace() << "[eval_air_temperature_nl()] ... exit" << std::endl;
}

void eval_air_temperature_tl(atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_temperature_tl()] starting ..." << std::endl;

  idx_t lvls(incFlds["air_temperature"].shape(1));
  idx_t lvlsm1 = lvls - 1;

  util::for_each_column(
    [=] (ConstView theta, ConstView exnerLevels, ConstView h, ConstView hl,
         ConstView thetaInc, ConstView exnerLevelsInc, View tInc) {
      // Passive code: Value above model top is assumed to be in hydrostatic balance.
      double exnerTopVal = exnerLevels(lvlsm1) -
        (constants::grav * (hl(lvls) - hl(lvlsm1))) /
        (constants::cp * theta(lvlsm1));

        // Active code;
      for (idx_t jl= 0; jl < lvls - 1; ++jl) {
        tInc(jl) = (
          ( (h(jl) - hl(jl)) * exnerLevels(jl + 1) +
            (hl(jl+1)  - h(jl)) * exnerLevels(jl) ) *
            thetaInc(jl) +
          ( (h(jl) - hl(jl)) * exnerLevelsInc(jl + 1) +
            (hl(jl+1)  - h(jl)) * exnerLevelsInc(jl) ) *
          theta(jl) ) /
          (hl(jl+1) - hl(jl));
      }

      double exnerTopIncVal = exnerLevelsInc(lvlsm1) +
        thetaInc(lvlsm1) * (exnerLevels(lvlsm1) - exnerTopVal) /
        theta(lvlsm1);

      tInc(lvlsm1) = (
        ( (h(lvlsm1) - hl(lvlsm1)) * exnerTopVal +
          (hl(lvls)  - h(lvlsm1)) * exnerLevels(lvlsm1) ) *
          thetaInc(lvlsm1) +
        ( (h(lvlsm1) - hl(lvlsm1)) * exnerTopIncVal +
          (hl(lvls)  - h(lvlsm1)) * exnerLevelsInc(lvlsm1) ) *
          theta(lvlsm1)) /
        (hl(lvls) - hl(lvlsm1));
    },
    stateFlds["air_potential_temperature"],
    stateFlds["dimensionless_exner_function_levels_minus_one"],
    stateFlds["height_above_mean_sea_level"],
    stateFlds["height_above_mean_sea_level_levels"],
    incFlds["air_potential_temperature"],
    incFlds["dimensionless_exner_function_levels_minus_one"],
    incFlds["air_temperature"]);

  incFlds["air_temperature"].set_dirty();
  oops::Log::trace() << "[eval_air_temperature_tl()] ... exit" << std::endl;
}

void eval_air_temperature_ad(atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_temperature_ad()] starting ..." << std::endl;

  idx_t lvls(hatFlds["air_temperature"].shape(1));
  idx_t lvlsm1 = lvls - 1;

  util::for_each_column(
    [=] (ConstView theta, ConstView exnerLevels, ConstView h, ConstView hl,
         View thetaHat, View exnerLevelsHat, View tHat) {
      // Passive code: Value above model top is assumed to be in hydrostatic balance.
      double exnerTopVal = exnerLevels(lvlsm1) -
        (constants::grav * (hl(lvls) - hl(lvlsm1))) /
        (constants::cp * theta(lvlsm1));

      // Active code
      thetaHat(lvlsm1) += ( (h(lvlsm1) - hl(lvlsm1)) * exnerTopVal +
        (hl(lvls)  - h(lvlsm1)) * exnerLevels(lvlsm1) ) *
        tHat(lvlsm1) /
        (hl(lvls) - hl(lvlsm1));

      double exnerTopHatVal = (h(lvlsm1) - hl(lvlsm1)) *
        tHat(lvlsm1) * theta(lvlsm1) /
        (hl(lvls) - hl(lvlsm1));

      exnerLevelsHat(lvlsm1) += (hl(lvls)  - h(lvlsm1)) *
        tHat(lvlsm1) * theta(lvlsm1) /
        (hl(lvls) - hl(lvlsm1));

      tHat(lvlsm1) = 0.0;

      exnerLevelsHat(lvlsm1) += exnerTopHatVal;
      thetaHat(lvlsm1) += exnerTopHatVal * (exnerLevels(lvlsm1) - exnerTopVal) /
          theta(lvlsm1);

      for (idx_t jl = lvls - 2; jl >= 0; --jl) {
        thetaHat(jl) += (
          (h(jl) - hl(jl)) * exnerLevels(jl + 1) +
          (hl(jl + 1) - h(jl)) * exnerLevels(jl) ) *
          tHat(jl) /
          (hl(jl + 1) - hl(jl));

        exnerLevelsHat(jl + 1) += (h(jl) - hl(jl)) *
          tHat(jl) * theta(jl) /
          (hl(jl + 1) - hl(jl));

        exnerLevelsHat(jl) += (hl(jl + 1)  - h(jl)) *
          tHat(jl) * theta(jl) /
          (hl(jl + 1) - hl(jl));

        tHat(jl) = 0.0;
      }
    },
    stateFlds["air_potential_temperature"],
    stateFlds["dimensionless_exner_function_levels_minus_one"],
    stateFlds["height_above_mean_sea_level"],
    stateFlds["height_above_mean_sea_level_levels"],
    hatFlds["air_potential_temperature"],
    hatFlds["dimensionless_exner_function_levels_minus_one"],
    hatFlds["air_temperature"]);

  hatFlds["dimensionless_exner_function_levels_minus_one"].set_dirty();
  hatFlds["air_potential_temperature"].set_dirty();
  hatFlds["air_temperature"].set_dirty();
  oops::Log::trace() << "[eval_air_temperature_ad()] ... exit" << std::endl;
}

}  // namespace mo
