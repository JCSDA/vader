/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "atlas/field/FieldSet.h"

#include "eckit/exception/Exceptions.h"

#include "oops/util/for_each.h"
#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

#include "mo/constants.h"
#include "mo/eval_air_pressure_levels.h"

using atlas::array::make_view;
using atlas::idx_t;

using View = atlas::array::LocalView<double, 1>;
using ConstView = atlas::array::LocalView<const double, 1>;

namespace mo {

/// \details Vertical extrapolation of air pressure above model top
void eval_air_pressure_levels_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_pressure_levels_nl()] starting ..." << std::endl;

  const idx_t levels(stateFlds["air_pressure_levels"].shape(1));

  util::for_each_column(
    [=] (ConstView ds_elmo, ConstView ds_plmo, ConstView ds_t, ConstView ds_hl, View ds_pl) {
      for (idx_t jl = 0; jl < levels - 1; ++jl) {
        ds_pl(jl) = ds_plmo(jl);
      }

      // Note that I am calculating the exner pressure above the top first and then
      // converting it to pressure
      // Note that strictly speaking we should be using virtual potential temperature here
      // but given that there should be no moisture at the top of the model, we should be
      // able to ignore that contribution.
      //
      // exner^k+1 = exner^k - g (height^k+1 - height^k) / (cp theta_v)
      //
      // pressure^k+1 = reference_pressure * (exner^k+1)**((1.0 / constants::rd_over_cp)
      //
      // where k is the model level index on half levels just below model top.

      ds_pl(levels-1) = constants::p_zero * pow(
        ds_elmo(levels-2) - (constants::grav * (ds_hl(levels-1) - ds_hl(levels-2))) /
        (constants::cp * ds_t(levels-2)), (1.0 / constants::rd_over_cp));

      ds_pl(levels-1) = ds_pl(levels-1) > 0.0 ? ds_pl(levels-1) : constants::deps;
    },
    stateFlds["dimensionless_exner_function_levels_minus_one"],
    stateFlds["air_pressure_levels_minus_one"],
    stateFlds["air_potential_temperature"],
    stateFlds["height_above_mean_sea_level_levels"],
    stateFlds["air_pressure_levels"]);

  stateFlds["air_pressure_levels"].set_dirty();

  oops::Log::trace() << "[eval_air_pressure_levels_nl()] ... exit" << std::endl;
}

void eval_air_pressure_levels_from_exner_tl(atlas::FieldSet & incFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_pressure_levels_from_exner_tl()] starting ..." << std::endl;

  const idx_t lvls = incFlds["dimensionless_exner_function_levels_minus_one"].shape(1);
  const idx_t lvlsm1 = lvls - 1;

  util::for_each_column(
    [=] (ConstView theta,
         ConstView p,
         ConstView exner,
         ConstView hl,
         ConstView thetaInc,
         ConstView exnerInc,
         View pInc) {
      const double exnerTopVal = exner(lvlsm1) -
        (constants::grav * (hl(lvls) - hl(lvlsm1))) / (constants::cp * theta(lvlsm1));

      for (idx_t jl = 0; jl < lvls - 1; ++jl) {
        pInc(jl) = exnerInc(jl) * p(jl) / (constants::rd_over_cp * exner(jl));
      }

      const double exnerTopIncVal = exnerInc(lvlsm1) +
        thetaInc(lvlsm1) * (exner(lvlsm1) - exnerTopVal) / theta(lvlsm1);

      pInc(lvls) = exnerTopIncVal * p(lvls) / (constants::rd_over_cp * exnerTopVal);
    },
    stateFlds["air_potential_temperature"],
    stateFlds["air_pressure_levels"],
    stateFlds["dimensionless_exner_function_levels_minus_one"],
    stateFlds["height_above_mean_sea_level_levels"],
    incFlds["air_potential_temperature"],
    incFlds["dimensionless_exner_function_levels_minus_one"],
    incFlds["air_pressure_levels"]);

  incFlds["air_pressure_levels"].set_dirty();

  oops::Log::trace() << "[eval_air_pressure_levels_from_exner_tl()] end ..." << std::endl;
}

void eval_air_pressure_levels_from_exner_ad(atlas::FieldSet & hatFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_pressure_levels_from_exner_ad()] starting ..." << std::endl;

  const idx_t lvls = hatFlds["dimensionless_exner_function_levels_minus_one"].shape(1);
  const idx_t lvlsm1 = lvls -1;

  util::for_each_column(
    [=] (ConstView theta,
         ConstView p,
         ConstView exner,
         ConstView hl,
         View pHat,
         View thetaHat,
         View exnerHat) {
      const double exnerTopVal = exner(lvlsm1) -
        (constants::grav * (hl(lvls) - hl(lvlsm1))) / (constants::cp * theta(lvlsm1));

      const double exnerTopHatVal = pHat(lvls) * p(lvls) / (constants::rd_over_cp * exnerTopVal);
      pHat(lvls) = 0.0;

      exnerHat(lvlsm1) += exnerTopHatVal;
      thetaHat(lvlsm1) += exnerTopHatVal * (exner(lvlsm1) - exnerTopVal) / theta(lvlsm1);

      for (idx_t jl = 0; jl < lvlsm1; ++jl) {
        exnerHat(jl) += pHat(jl) * p(jl) / (constants::rd_over_cp * exner(jl));
      }
    },
    stateFlds["air_potential_temperature"],
    stateFlds["air_pressure_levels"],
    stateFlds["dimensionless_exner_function_levels_minus_one"],
    stateFlds["height_above_mean_sea_level_levels"],
    hatFlds["air_pressure_levels"],
    hatFlds["air_potential_temperature"],
    hatFlds["dimensionless_exner_function_levels_minus_one"]);

  hatFlds["air_potential_temperature"].set_dirty();
  hatFlds["air_pressure_levels"].set_dirty();
  hatFlds["dimensionless_exner_function_levels_minus_one"].set_dirty();

  oops::Log::trace() << "[eval_air_pressure_levels_from_exner_ad()] end ..." << std::endl;
}

}  // namespace mo
