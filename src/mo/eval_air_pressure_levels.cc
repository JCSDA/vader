/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "atlas/field/FieldSet.h"

#include "eckit/exception/Exceptions.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

#include "mo/constants.h"
#include "mo/eval_air_pressure_levels.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {

/// \details Vertical extrapolation of air pressure above model top
void eval_air_pressure_levels_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_pressure_levels_nl()] starting ..." << std::endl;

  auto ds_elmo = make_view<const double, 2>(
       stateFlds["dimensionless_exner_function_levels_minus_one"]);
  auto ds_plmo = make_view<const double, 2>(stateFlds["air_pressure_levels_minus_one"]);
  auto ds_t = make_view<const double, 2>(stateFlds["air_potential_temperature"]);
  auto ds_hl = make_view<const double, 2>(stateFlds["height_above_mean_sea_level_levels"]);
  auto ds_pl = make_view<double, 2>(stateFlds["air_pressure_levels"]);

  idx_t levels(stateFlds["air_pressure_levels"].shape(1));
  const idx_t sizeOwned = util::getSizeOwned(stateFlds["air_pressure_levels"].functionspace());

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < levels - 1; ++jl) {
      ds_pl(jn, jl) = ds_plmo(jn, jl);
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

    ds_pl(jn, levels-1) =  constants::p_zero * pow(
      ds_elmo(jn, levels-2) - (constants::grav * (ds_hl(jn, levels-1) - ds_hl(jn, levels-2))) /
      (constants::cp * ds_t(jn, levels-2)), (1.0 / constants::rd_over_cp));

    ds_pl(jn, levels-1) = ds_pl(jn, levels-1) > 0.0 ? ds_pl(jn, levels-1) : constants::deps;
  }

  stateFlds["air_pressure_levels"].set_dirty();
  oops::Log::trace() << "[eval_air_pressure_levels_nl()] ... exit" << std::endl;
}

void eval_air_pressure_levels_from_exner_tl(atlas::FieldSet & incFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_pressure_levels_from_exner_tl()] starting ..." << std::endl;

  // State fields
  const auto thetaView = make_view<double, 2>(stateFlds["air_potential_temperature"]);
  const auto pView = make_view<double, 2>(stateFlds["air_pressure_levels"]);
  const auto exnerView = make_view<double, 2>(
    stateFlds["dimensionless_exner_function_levels_minus_one"]);
  const auto hlView = make_view<double, 2>(stateFlds["height_above_mean_sea_level_levels"]);

  // Increment fields
  auto pIncView = make_view<double, 2>(incFlds["air_pressure_levels"]);
  const auto thetaIncView = make_view<double, 2>(incFlds["air_potential_temperature"]);
  const auto exnerIncView = make_view<double, 2>(
    incFlds["dimensionless_exner_function_levels_minus_one"]);

  double exnerTopVal;
  double exnerTopIncVal;
  atlas::idx_t lvls = incFlds["dimensionless_exner_function_levels_minus_one"].shape(1);

  const idx_t lvlsm1 = lvls - 1;
  const atlas::idx_t sizeOwned =
    util::getSizeOwned(incFlds["dimensionless_exner_function_levels_minus_one"].functionspace());

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    exnerTopVal = exnerView(jn, lvlsm1) -
      (constants::grav * (hlView(jn, lvls) - hlView(jn, lvlsm1))) /
      (constants::cp * thetaView(jn, lvlsm1));

    for (idx_t jl = 0; jl < lvls - 1; ++jl) {
      pIncView(jn, jl) = exnerIncView(jn, jl) *  pView(jn, jl) /
        (constants::rd_over_cp * exnerView(jn, jl));
    }

    exnerTopIncVal = exnerIncView(jn, lvlsm1) +
      thetaIncView(jn, lvlsm1) * (exnerView(jn, lvlsm1) - exnerTopVal) /
      thetaView(jn, lvlsm1);

    pIncView(jn, lvls) = exnerTopIncVal *  pView(jn, lvls) /
      (constants::rd_over_cp * exnerTopVal);
  }

  incFlds["air_pressure_levels"].set_dirty();

  oops::Log::trace() << "[eval_air_pressure_levels_from_exner_tl()] end ..." << std::endl;
}

void eval_air_pressure_levels_from_exner_ad(atlas::FieldSet & hatFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_pressure_levels_from_exner_ad()] starting ..." << std::endl;

  // State fields
  const auto thetaView = make_view<double, 2>(stateFlds["air_potential_temperature"]);
  const auto pView = make_view<double, 2>(stateFlds["air_pressure_levels"]);
  const auto exnerView = make_view<double, 2>(
    stateFlds["dimensionless_exner_function_levels_minus_one"]);
  const auto hlView = make_view<double, 2>(stateFlds["height_above_mean_sea_level_levels"]);

  // Increment fields
  auto pHatView = make_view<double, 2>(hatFlds["air_pressure_levels"]);
  auto thetaHatView = make_view<double, 2>(hatFlds["air_potential_temperature"]);
  auto exnerHatView = make_view<double, 2>(
    hatFlds["dimensionless_exner_function_levels_minus_one"]);

  const idx_t sizeOwned =
    util::getSizeOwned(hatFlds["dimensionless_exner_function_levels_minus_one"].functionspace());
  const idx_t lvls = hatFlds["dimensionless_exner_function_levels_minus_one"].shape(1);
  const idx_t lvlsm1 = lvls -1;
  double alpha_jl(0.0);
  double exnerTopVal;
  double exnerTopHatVal;

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    exnerTopVal = exnerView(jn, lvlsm1) -
      (constants::grav * (hlView(jn, lvls) - hlView(jn, lvlsm1))) /
      (constants::cp * thetaView(jn, lvlsm1));

    exnerTopHatVal = pHatView(jn, lvls) *  pView(jn, lvls) /
      (constants::rd_over_cp * exnerTopVal);
    pHatView(jn, lvls) = 0.0;

    exnerHatView(jn, lvlsm1) += exnerTopHatVal;
    thetaHatView(jn, lvlsm1) += exnerTopHatVal * (exnerView(jn, lvlsm1) - exnerTopVal) /
      thetaView(jn, lvlsm1);
    exnerTopHatVal = 0.0;

    for (idx_t jl = 0; jl < lvls - 1; ++jl) {
      exnerHatView(jn, jl) +=  pHatView(jn, jl) *  pView(jn, jl) /
        (constants::rd_over_cp * exnerView(jn, jl));
    }
  }

  hatFlds["air_potential_temperature"].set_dirty();
  hatFlds["air_pressure_levels"].set_dirty();
  hatFlds["dimensionless_exner_function_levels_minus_one"].set_dirty();

  oops::Log::trace() << "[eval_air_pressure_levels_from_exner_ad()] end ..." << std::endl;
}

}  // namespace mo
