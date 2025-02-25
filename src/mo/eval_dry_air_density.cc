/*
 * (C) Crown Copyright 2023-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "atlas/field/FieldSet.h"

#include "mo/constants.h"
#include "mo/eval_dry_air_density.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace {
  const char specific_humidity_mo[] = "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water";
}  // namespace

namespace mo {

// -------------------------------------------------------------------------------------------------

/// \details Calculate the dry air density from potential temperature,
///          specific humidity, cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water,
///          cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water
///          and air pressure (using air_pressure_levels_minus_one)
void eval_dry_air_density_from_pressure_levels_minus_one_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_nl()] starting ..."
                     << std::endl;
  const auto hlView = make_view<const double, 2>(stateFlds["height_above_mean_sea_level_levels"]);
  const auto hView = make_view<const double, 2>(stateFlds["height_above_mean_sea_level"]);
  const auto ptView = make_view<const double, 2>(stateFlds["air_potential_temperature"]);
  const auto qView = make_view<const double, 2>(stateFlds[specific_humidity_mo]);
  const auto qclView = make_view<const double, 2>(
                    stateFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qcfView = make_view<const double, 2>(
                         stateFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto pView = make_view<const double, 2>(stateFlds["air_pressure_levels_minus_one"]);
  auto dryrhoView = make_view<double, 2>(stateFlds["dry_air_density_levels_minus_one"]);

  const idx_t numLevels = stateFlds["dry_air_density_levels_minus_one"].shape(1);
  const idx_t sizeOwned =
    util::getSizeOwned(stateFlds["dry_air_density_levels_minus_one"].functionspace());
  double h_minus_hm1;
  double h_minus_hl;
  double hl_minus_hm1;
  double vptdrydens;
  double vptdrydens_jlm1;
  double vptdrydens_intp;

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    // vptdrydens is the virtual potential temperature for dry density (theta_vdd)
    // so that the dry air density (rho_dd) can be written as rho_dd = p / (Rd Pi theta_vdd)
    vptdrydens = ptView(jn, 0) * (1.0 + constants::c_virtual * qView(jn, 0)
                 - qclView(jn, 0) - qcfView(jn, 0)) / (1.0 - qView(jn, 0)
                 - qclView(jn, 0) - qcfView(jn, 0));
    dryrhoView(jn, 0) = pView(jn, 0) *
      pow(pView(jn, 0) / constants::p_zero, - constants::rd_over_cp) /
      (constants::rd * vptdrydens);

    for (idx_t jl = 1; jl < numLevels; ++jl) {
      h_minus_hm1 = hView(jn, jl) - hView(jn, jl-1);
      h_minus_hl = hView(jn, jl) - hlView(jn, jl);
      hl_minus_hm1 = hlView(jn, jl) - hView(jn, jl-1);
      vptdrydens_jlm1 = vptdrydens;
      vptdrydens = ptView(jn, jl) * (1.0 + constants::c_virtual * qView(jn, jl)
                   - qclView(jn, jl) - qcfView(jn, jl)) / (1.0 - qView(jn, jl)
                   - qclView(jn, jl) - qcfView(jn, jl));
      vptdrydens_intp = (h_minus_hl * vptdrydens_jlm1 + hl_minus_hm1 * vptdrydens) / h_minus_hm1;
      dryrhoView(jn, jl) = pView(jn, jl) *
                           pow(pView(jn, jl) / constants::p_zero, - constants::rd_over_cp) /
                           (constants::rd * vptdrydens_intp);
    }
  }
  stateFlds["dry_air_density_levels_minus_one"].set_dirty();

  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_nl()] ... exit"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void eval_dry_air_density_from_pressure_levels_minus_one_tl(atlas::FieldSet & incFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_tl()] starting ..."
                     << std::endl;
  const auto hlView = make_view<const double, 2>(stateFlds["height_above_mean_sea_level_levels"]);
  const auto hView = make_view<const double, 2>(stateFlds["height_above_mean_sea_level"]);
  const auto ptView = make_view<const double, 2>(stateFlds["air_potential_temperature"]);
  const auto qView = make_view<const double, 2>(stateFlds[specific_humidity_mo]);
  const auto qclView = make_view<const double, 2>(
                    stateFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qcfView = make_view<const double, 2>(
                         stateFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto pView = make_view<const double, 2>(stateFlds["air_pressure_levels_minus_one"]);
  const auto dryrhoView = make_view<const double, 2>(stateFlds["dry_air_density_levels_minus_one"]);

  const auto pIncView = make_view<const double, 2>(incFlds["air_pressure_levels_minus_one"]);
  const auto ptIncView = make_view<const double, 2>(incFlds["air_potential_temperature"]);
  const auto qIncView = make_view<const double, 2>(incFlds[specific_humidity_mo]);
  const auto qclIncView = make_view<const double, 2>(
                      incFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qcfIncView = make_view<const double, 2>(
                            incFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto dryrhoIncView = make_view<double, 2>(incFlds["dry_air_density_levels_minus_one"]);
  const idx_t numLevels = incFlds["dry_air_density_levels_minus_one"].shape(1);
  const idx_t sizeOwned =
    util::getSizeOwned(incFlds["dry_air_density_levels_minus_one"].functionspace());

  double h_minus_hl;
  double hl_minus_hm1;
  double vptdrydens;
  double vptdrydensInc;
  double vptdrydens_jlm1;
  double vptdrydensInc_jlm1;
  double vptdrydens_intp_times_h_minus_hm1;
  double vptdrydensInc_intp_times_h_minus_hm1;

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    vptdrydens = ptView(jn, 0) * (1.0 + constants::c_virtual * qView(jn, 0)
                 - qclView(jn, 0) - qcfView(jn, 0)) / (1.0 - qView(jn, 0)
                 - qclView(jn, 0) - qcfView(jn, 0));
    vptdrydensInc = ((1.0 + constants::c_virtual * qView(jn, 0)
                      - qclView(jn, 0) - qcfView(jn, 0)) * ptIncView(jn, 0)
                     + ((1.0 + constants::c_virtual) * qIncView(jn, 0)
                        * (1.0 - qclView(jn, 0) - qcfView(jn, 0)) * ptView(jn, 0)
                        / (1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0)))
                     + (qclIncView(jn, 0) *  qView(jn, 0)
                       * (1.0 + constants::c_virtual) * ptView(jn, 0))
                       / (1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0))
                     + (qcfIncView(jn, 0) *  qView(jn, 0)
                       * (1.0 + constants::c_virtual) * ptView(jn, 0))
                       / (1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0)))
                    / (1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0));
    dryrhoIncView(jn, 0) = dryrhoView(jn, 0) * ((1.0 - constants::rd_over_cp)
                           * pIncView(jn, 0) / pView(jn, 0)
                           - vptdrydensInc / vptdrydens);

    for (idx_t jl = 1; jl < numLevels; ++jl) {
      h_minus_hl = hView(jn, jl) - hlView(jn, jl);
      hl_minus_hm1 = hlView(jn, jl) - hView(jn, jl-1);
      vptdrydens_jlm1 = vptdrydens;
      vptdrydens = ptView(jn, jl) * (1.0 + constants::c_virtual * qView(jn, jl)
                   - qclView(jn, jl) - qcfView(jn, jl)) / (1.0 - qView(jn, jl)
                   - qclView(jn, jl) - qcfView(jn, jl));
      vptdrydensInc_jlm1 = vptdrydensInc;
      vptdrydensInc = ((1.0 + constants::c_virtual * qView(jn, jl)
                        - qclView(jn, jl) - qcfView(jn, jl)) * ptIncView(jn, jl)
                       + ((1.0 + constants::c_virtual) * qIncView(jn, jl)
                          * (1.0 - qclView(jn, jl) - qcfView(jn, jl)) * ptView(jn, jl)
                          / (1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl)))
                       + (qclIncView(jn, jl) *  qView(jn, jl)
                          * (1.0 + constants::c_virtual) * ptView(jn, jl))
                          / (1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl))
                       + (qcfIncView(jn, jl) *  qView(jn, jl)
                          * (1.0 + constants::c_virtual) * ptView(jn, jl))
                          / (1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl)))
                      / (1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl));
      vptdrydens_intp_times_h_minus_hm1 = h_minus_hl * vptdrydens_jlm1 +
                                           hl_minus_hm1 * vptdrydens;
      vptdrydensInc_intp_times_h_minus_hm1 = hl_minus_hm1 * vptdrydensInc +
                                              h_minus_hl * vptdrydensInc_jlm1;
      dryrhoIncView(jn, jl) = dryrhoView(jn, jl) * ( (1.0 - constants::rd_over_cp)
                              * pIncView(jn, jl) / pView(jn, jl)
                              - vptdrydensInc_intp_times_h_minus_hm1
                              / vptdrydens_intp_times_h_minus_hm1);
    }
  }
  incFlds["dry_air_density_levels_minus_one"].set_dirty();
  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_tl()] ... exit"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void eval_dry_air_density_from_pressure_levels_minus_one_ad(atlas::FieldSet & hatFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_ad()] starting ..."
                     << std::endl;
  const auto hlView = make_view<const double, 2>(stateFlds["height_above_mean_sea_level_levels"]);
  const auto hView = make_view<const double, 2>(stateFlds["height_above_mean_sea_level"]);
  const auto ptView = make_view<const double, 2>(stateFlds["air_potential_temperature"]);
  const auto qView = make_view<const double, 2>(stateFlds[specific_humidity_mo]);
  const auto qclView = make_view<const double, 2>(
                 stateFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto qcfView = make_view<const double, 2>(
                       stateFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const auto pView = make_view<const double, 2>(stateFlds["air_pressure_levels_minus_one"]);
  const auto dryrhoView = make_view<const double, 2>(stateFlds["dry_air_density_levels_minus_one"]);

  auto pHatView = make_view<double, 2>(hatFlds["air_pressure_levels_minus_one"]);
  auto ptHatView = make_view<double, 2>(hatFlds["air_potential_temperature"]);
  auto qHatView = make_view<double, 2>(hatFlds[specific_humidity_mo]);
  auto qclHatView = make_view<double, 2>(
                    hatFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto qcfHatView = make_view<double, 2>
                              (hatFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto dryrhoHatView = make_view<double, 2>(hatFlds["dry_air_density_levels_minus_one"]);

  double h_minus_hl;
  double hl_minus_hm1;
  double vptdrydens;
  double vptdrydens_intp_times_h_minus_hm1;
  double vptdrydensHat;
  double vptdrydens_jlm1;
  double vptdrydensHat_jlm1;

  const idx_t numLevels = hatFlds["dry_air_density_levels_minus_one"].shape(1);
  const idx_t sizeOwned =
    util::getSizeOwned(hatFlds["dry_air_density_levels_minus_one"].functionspace());

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = numLevels-1; jl >= 1; --jl) {
      h_minus_hl = hView(jn, jl) - hlView(jn, jl);
      hl_minus_hm1 = hlView(jn, jl) - hView(jn, jl-1);
      pHatView(jn, jl) += dryrhoView(jn, jl) * (1.0 - constants::rd_over_cp) /
                          pView(jn, jl) * dryrhoHatView(jn, jl);
      vptdrydens = ptView(jn, jl) * (1.0 + constants::c_virtual * qView(jn, jl)
                   - qclView(jn, jl) - qcfView(jn, jl)) / (1.0 - qView(jn, jl)
                   - qclView(jn, jl) - qcfView(jn, jl));
      vptdrydens_jlm1 = ptView(jn, jl-1) * (1.0 + constants::c_virtual * qView(jn, jl-1)
                        - qclView(jn, jl-1) - qcfView(jn, jl-1)) / (1.0 - qView(jn, jl-1)
                        - qclView(jn, jl-1) - qcfView(jn, jl-1));
      vptdrydens_intp_times_h_minus_hm1 = h_minus_hl * vptdrydens_jlm1 +
                                           hl_minus_hm1 * vptdrydens;
      vptdrydensHat = - dryrhoView(jn, jl) * dryrhoHatView(jn, jl) * hl_minus_hm1
                      / vptdrydens_intp_times_h_minus_hm1;
      vptdrydensHat_jlm1 = - dryrhoView(jn, jl) * dryrhoHatView(jn, jl) * h_minus_hl /
                           vptdrydens_intp_times_h_minus_hm1;
      ptHatView(jn, jl) += (1.0 + constants::c_virtual * qView(jn, jl)
                            - qclView(jn, jl) - qcfView(jn, jl))
                           / (1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl))
                           * vptdrydensHat;
      qHatView(jn, jl) += (1.0 + constants::c_virtual) * ptView(jn, jl)
                           * (1.0 - qclView(jn, jl) - qcfView(jn, jl))
                           / ((1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl))
                               * (1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl)))
                           * vptdrydensHat;
      qclHatView(jn, jl) += ptView(jn, jl) *  qView(jn, jl) * (1.0 + constants::c_virtual)
                            / ((1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl))
                                * (1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl)))
                            * vptdrydensHat;
      qcfHatView(jn, jl) += ptView(jn, jl) *  qView(jn, jl) * (1.0 + constants::c_virtual)
                            / ((1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl))
                                * (1.0 - qView(jn, jl) - qclView(jn, jl) - qcfView(jn, jl)))
                            * vptdrydensHat;
      ptHatView(jn, jl-1) += (1.0 + constants::c_virtual * qView(jn, jl-1)
                              - qclView(jn, jl-1) - qcfView(jn, jl-1))
                             / (1.0 - qView(jn, jl-1) - qclView(jn, jl-1) - qcfView(jn, jl-1))
                             * vptdrydensHat_jlm1;
      qHatView(jn, jl-1) += (1.0 + constants::c_virtual) * ptView(jn, jl-1)
                            * (1.0 - qclView(jn, jl-1) - qcfView(jn, jl-1))
                            / ((1.0 - qView(jn, jl-1) - qclView(jn, jl-1) - qcfView(jn, jl-1))
                                * (1.0 - qView(jn, jl-1) - qclView(jn, jl-1) - qcfView(jn, jl-1)))
                            * vptdrydensHat_jlm1;
      qclHatView(jn, jl-1) += ptView(jn, jl-1) *  qView(jn, jl-1) * (1.0 + constants::c_virtual)
                              / ((1.0 - qView(jn, jl-1) - qclView(jn, jl-1) - qcfView(jn, jl-1))
                                  * (1.0 - qView(jn, jl-1) - qclView(jn, jl-1)
                                     - qcfView(jn, jl-1)))
                              * vptdrydensHat_jlm1;
      qcfHatView(jn, jl-1) += ptView(jn, jl-1) *  qView(jn, jl-1) * (1.0 + constants::c_virtual)
                              / ((1.0 - qView(jn, jl-1) - qclView(jn, jl-1) - qcfView(jn, jl-1))
                                  * (1.0 - qView(jn, jl-1) - qclView(jn, jl-1)
                                     - qcfView(jn, jl-1)))
                              * vptdrydensHat_jlm1;
      dryrhoHatView(jn, jl) = 0.0;
    }
    pHatView(jn, 0) += dryrhoView(jn, 0) * ((1.0 - constants::rd_over_cp) /
                       pView(jn, 0)) * dryrhoHatView(jn, 0);
    vptdrydens = ptView(jn, 0) *
                 (1.0 + constants::c_virtual * qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0)) /
                 (1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0));
    vptdrydensHat = - dryrhoView(jn, 0) * dryrhoHatView(jn, 0) / vptdrydens;
    ptHatView(jn, 0) += (1.0 + constants::c_virtual * qView(jn, 0)
                         - qclView(jn, 0) - qcfView(jn, 0))
                        / (1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0)) * vptdrydensHat;
    qHatView(jn, 0) += (1.0 + constants::c_virtual) * ptView(jn, 0)
                       * (1.0 - qclView(jn, 0) - qcfView(jn, 0))
                       / ((1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0))
                       * (1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0))) * vptdrydensHat;
    qclHatView(jn, 0) += ptView(jn, 0) *  qView(jn, 0) * (1.0 + constants::c_virtual)
                         / ((1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0))
                         * (1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0))) * vptdrydensHat;
    qcfHatView(jn, 0) += ptView(jn, 0) *  qView(jn, 0) * (1.0 + constants::c_virtual)
                         / ((1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0))
                         * (1.0 - qView(jn, 0) - qclView(jn, 0) - qcfView(jn, 0))) * vptdrydensHat;
    dryrhoHatView(jn, 0) = 0.0;
  }
  hatFlds["air_pressure_levels_minus_one"].set_dirty();
  hatFlds["air_potential_temperature"].set_dirty();
  hatFlds[specific_humidity_mo].set_dirty();
  hatFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();
  hatFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();
  hatFlds["dry_air_density_levels_minus_one"].set_dirty();

  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_ad()] ... exit"
                     << std::endl;
}

}  // namespace mo
