/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "atlas/field/FieldSet.h"

#include "mo/constants.h"
#include "mo/eval_air_pressure.h"

#include "oops/util/for_each.h"
#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

using View = atlas::array::LocalView<double, 1>;
using ConstView = atlas::array::LocalView<const double, 1>;

namespace mo {

void eval_air_pressure_from_exner_tl(atlas::FieldSet & incFlds,
                                     const atlas::FieldSet & stateFlds) {
  const idx_t lvls = incFlds["air_pressure"].shape(1);
  const idx_t lvlsm1 = lvls -1;

  util::for_each_column(
    [=] (ConstView theta,
         ConstView p,
         ConstView exner,
         ConstView hl,
         ConstView h,
         ConstView thetaInc,
         ConstView exnerInc,
         View pbarInc) {
      double alpha_jl = 0.0;
      for (idx_t jl = 0; jl < lvlsm1; ++jl) {
        alpha_jl = (h(jl) - hl(jl)) / (hl(jl+1) - hl(jl));
        pbarInc(jl) = (1.0 - alpha_jl) *
          exnerInc(jl) * p(jl) / (constants::rd_over_cp * exner(jl)) +
          alpha_jl *
          exnerInc(jl+1) * p(jl+1) / (constants::rd_over_cp * exner(jl+1));
      }

      const double exnerTopVal = exner(lvlsm1) -
        (constants::grav * (hl(lvls) - hl(lvlsm1))) / (constants::cp * theta(lvlsm1));
      const double exnerTopIncVal = exnerInc(lvlsm1) +
        thetaInc(lvlsm1) * (exner(lvlsm1) - exnerTopVal) / theta(lvlsm1);

      alpha_jl = (h(lvlsm1) - hl(lvlsm1)) / (hl(lvls) - hl(lvlsm1));

      pbarInc(lvlsm1) = (1.0 - alpha_jl) *
        exnerInc(lvlsm1) * p(lvlsm1) / (constants::rd_over_cp * exner(lvlsm1)) +
        alpha_jl *
        exnerTopIncVal * p(lvls) / (constants::rd_over_cp * exnerTopVal);
    },
    stateFlds["air_potential_temperature"],
    stateFlds["air_pressure_levels"],
    stateFlds["dimensionless_exner_function_levels_minus_one"],
    stateFlds["height_above_mean_sea_level_levels"],
    stateFlds["height_above_mean_sea_level"],
    incFlds["air_potential_temperature"],
    incFlds["dimensionless_exner_function_levels_minus_one"],
    incFlds["air_pressure"]);

  incFlds["air_pressure"].set_dirty();
}

void eval_air_pressure_from_exner_ad(atlas::FieldSet & hatFlds,
                                     const atlas::FieldSet & stateFlds) {
  const idx_t lvls = hatFlds["air_pressure"].shape(1);
  const idx_t lvlsm1 = lvls -1;

  util::for_each_column(
    [=] (ConstView theta,
         ConstView p,
         ConstView exner,
         ConstView hl,
         ConstView h,
         View pbarHat,
         View thetaHat,
         View exnerHat) {
      // Passive code: Value above model top is assumed to be in hydrostatic balance.
      const double exnerTopVal = exner(lvlsm1) -
        (constants::grav * (hl(lvls) - hl(lvlsm1))) / (constants::cp * theta(lvlsm1));

      double alpha_jl = (h(lvlsm1) - hl(lvlsm1)) / (hl(lvls) - hl(lvlsm1));

      exnerHat(lvlsm1) += (1.0 - alpha_jl) *
        pbarHat(lvlsm1) * p(lvlsm1) / (constants::rd_over_cp * exner(lvlsm1));
      const double exnerTopHatVal = alpha_jl *
        pbarHat(lvlsm1) * p(lvls) / (constants::rd_over_cp * exnerTopVal);
      pbarHat(lvlsm1) = 0;

      exnerHat(lvlsm1) += exnerTopHatVal;
      thetaHat(lvlsm1) += exnerTopHatVal * (exner(lvlsm1) - exnerTopVal) / theta(lvlsm1);

      for (idx_t jl = lvls - 2; jl >= 0; --jl) {
        alpha_jl = (h(jl) - hl(jl)) / (hl(jl+1) - hl(jl));
        exnerHat(jl) += (1.0 - alpha_jl) *
          pbarHat(jl) * p(jl) / (constants::rd_over_cp * exner(jl));
        exnerHat(jl+1) += alpha_jl *
          pbarHat(jl) * p(jl+1) / (constants::rd_over_cp * exner(jl+1));
        pbarHat(jl) = 0.0;
      }
    },
    stateFlds["air_potential_temperature"],
    stateFlds["air_pressure_levels"],
    stateFlds["dimensionless_exner_function_levels_minus_one"],
    stateFlds["height_above_mean_sea_level_levels"],
    stateFlds["height_above_mean_sea_level"],
    hatFlds["air_pressure"],
    hatFlds["air_potential_temperature"],
    hatFlds["dimensionless_exner_function_levels_minus_one"]);

  hatFlds["air_potential_temperature"].set_dirty();
  hatFlds["air_pressure"].set_dirty();
  hatFlds["dimensionless_exner_function_levels_minus_one"].set_dirty();
}

}  // namespace mo
