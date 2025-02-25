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

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {

void eval_air_pressure_from_exner_tl(atlas::FieldSet & incFlds,
                                     const atlas::FieldSet & stateFlds) {
  // State fields
  const auto thetaView = make_view<double, 2>(stateFlds["air_potential_temperature"]);
  const auto pView = make_view<double, 2>(stateFlds["air_pressure_levels"]);
  const auto exnerView = make_view<double, 2>(
    stateFlds["dimensionless_exner_function_levels_minus_one"]);
  const auto hlView = make_view<double, 2>(stateFlds["height_above_mean_sea_level_levels"]);
  const auto hView = make_view<double, 2>(stateFlds["height_above_mean_sea_level"]);

  // Increment fields
  auto pbarIncView = make_view<double, 2>(incFlds["air_pressure"]);
  const auto thetaIncView = make_view<const double, 2>(incFlds["air_potential_temperature"]);
  const auto exnerIncView = make_view<const double, 2>(
    incFlds["dimensionless_exner_function_levels_minus_one"]);


  const idx_t sizeOwned = util::getSizeOwned(incFlds["air_pressure"].functionspace());
  const idx_t lvls = incFlds["air_pressure"].shape(1);
  const idx_t lvlsm1 = lvls -1;
  double alpha_jl(0.0);
  double exnerTopVal;
  double exnerTopIncVal;

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < lvlsm1; ++jl) {
      alpha_jl = (hView(jn, jl) - hlView(jn, jl)) / (hlView(jn, jl+1) - hlView(jn, jl));

      pbarIncView(jn, jl) = (1.0 - alpha_jl) *
        exnerIncView(jn, jl) *  pView(jn, jl) / (constants::rd_over_cp * exnerView(jn, jl)) +
        alpha_jl *
        exnerIncView(jn, jl+1) *  pView(jn, jl+1) / (constants::rd_over_cp * exnerView(jn, jl+1));
    }
    exnerTopVal = exnerView(jn, lvlsm1) -
      (constants::grav * (hlView(jn, lvls) - hlView(jn, lvlsm1))) /
      (constants::cp * thetaView(jn, lvlsm1));
    exnerTopIncVal = exnerIncView(jn, lvlsm1) +
      thetaIncView(jn, lvlsm1) * (exnerView(jn, lvlsm1) - exnerTopVal) /
      thetaView(jn, lvlsm1);

    alpha_jl = (hView(jn, lvlsm1) - hlView(jn, lvlsm1)) /
      (hlView(jn, lvls) - hlView(jn, lvlsm1));

    pbarIncView(jn, lvlsm1) = (1.0 - alpha_jl) *
      exnerIncView(jn, lvlsm1) *  pView(jn, lvlsm1) /
      (constants::rd_over_cp * exnerView(jn, lvlsm1)) +
      alpha_jl *
      exnerTopIncVal *  pView(jn, lvls) / (constants::rd_over_cp * exnerTopVal);
  }

  incFlds["air_pressure"].set_dirty();
}

void eval_air_pressure_from_exner_ad(atlas::FieldSet & hatFlds,
                                     const atlas::FieldSet & stateFlds) {
  // State fields
  const auto thetaView = make_view<double, 2>(stateFlds["air_potential_temperature"]);
  const auto pView = make_view<double, 2>(stateFlds["air_pressure_levels"]);
  const auto exnerView = make_view<double, 2>(
    stateFlds["dimensionless_exner_function_levels_minus_one"]);
  const auto hlView = make_view<double, 2>(stateFlds["height_above_mean_sea_level_levels"]);
  const auto hView = make_view<double, 2>(stateFlds["height_above_mean_sea_level"]);

  // Increment fields
  auto pbarHatView = make_view<double, 2>(hatFlds["air_pressure"]);
  auto thetaHatView = make_view<double, 2>(hatFlds["air_potential_temperature"]);
  auto exnerHatView = make_view<double, 2>(
    hatFlds["dimensionless_exner_function_levels_minus_one"]);

  const idx_t sizeOwned = util::getSizeOwned(hatFlds["air_pressure"].functionspace());
  const idx_t lvls = hatFlds["air_pressure"].shape(1);
  const idx_t lvlsm1 = lvls -1;
  double alpha_jl(0.0);
  double exnerTopVal;
  double exnerTopHatVal;

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    // Passive code: Value above model top is assumed to be in hydrostatic balance.
    exnerTopVal = exnerView(jn, lvlsm1) -
      (constants::grav * (hlView(jn, lvls) - hlView(jn, lvlsm1))) /
      (constants::cp * thetaView(jn, lvlsm1));

    alpha_jl = (hView(jn, lvlsm1) - hlView(jn, lvlsm1)) /
      (hlView(jn, lvls) - hlView(jn, lvlsm1));

    exnerHatView(jn, lvlsm1) += (1.0 - alpha_jl) *
      pbarHatView(jn, lvlsm1) * pView(jn, lvlsm1) / (constants::rd_over_cp * exnerView(jn, lvlsm1));
    exnerTopHatVal += alpha_jl *
      pbarHatView(jn, lvlsm1) * pView(jn, lvls) / (constants::rd_over_cp * exnerTopVal);
    pbarHatView(jn, lvlsm1) = 0;

    exnerHatView(jn, lvlsm1) += exnerTopHatVal;
    thetaHatView(jn, lvlsm1) += exnerTopHatVal * (exnerView(jn, lvlsm1) - exnerTopVal) /
      thetaView(jn, lvlsm1);
    exnerTopHatVal = 0.0;

    for (idx_t jl = lvls - 2; jl >= 0; --jl) {
      alpha_jl = (hView(jn, jl) - hlView(jn, jl)) / (hlView(jn, jl+1) - hlView(jn, jl));
      exnerHatView(jn, jl) += (1.0 - alpha_jl) *
        pbarHatView(jn, jl) * pView(jn, jl) / (constants::rd_over_cp * exnerView(jn, jl));
      exnerHatView(jn, jl+1) += alpha_jl *
        pbarHatView(jn, jl) * pView(jn, jl+1) / (constants::rd_over_cp * exnerView(jn, jl+1));
      pbarHatView(jn, jl) = 0.0;
    }
  }

  hatFlds["air_potential_temperature"].set_dirty();
  hatFlds["air_pressure"].set_dirty();
  hatFlds["dimensionless_exner_function_levels_minus_one"].set_dirty();
}

}  // namespace mo
