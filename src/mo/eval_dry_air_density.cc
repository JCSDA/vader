/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "atlas/field/FieldSet.h"

#include "mo/constants.h"
#include "mo/eval_dry_air_density.h"

#include "oops/util/for_each.h"
#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

using View = atlas::array::LocalView<double, 1>;
using ConstView = atlas::array::LocalView<const double, 1>;

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

  const idx_t numLevels = stateFlds["dry_air_density_levels_minus_one"].shape(1);

  util::for_each_column(
    [=](ConstView hViewCol,
        ConstView hlViewCol,
        ConstView pViewCol,
        ConstView ptViewCol,
        ConstView qViewCol,
        ConstView qcfViewCol,
        ConstView qclViewCol,
        View dryrhoViewCol) {
      double h_minus_hl;
      double h_minus_hm1;
      double hl_minus_hm1;
      double vptdrydens;
      double vptdrydens_intp;
      double vptdrydens_jlm1;

      // vptdrydens is the virtual potential temperature for dry density (theta_vdd)
      // so that the dry air density (rho_dd) can be written as rho_dd = p / (Rd Pi theta_vdd)
      vptdrydens = ptViewCol(0) * (1.0 + constants::c_virtual * qViewCol(0)
                   - qclViewCol(0) - qcfViewCol(0)) / (1.0 - qViewCol(0)
                   - qclViewCol(0) - qcfViewCol(0));
      dryrhoViewCol(0) = pViewCol(0) *
        pow(pViewCol(0) / constants::p_zero, - constants::rd_over_cp) /
        (constants::rd * vptdrydens);

      for (idx_t jl = 1; jl < numLevels; ++jl) {
        h_minus_hm1 = hViewCol(jl) - hViewCol(jl-1);
        h_minus_hl = hViewCol(jl) - hlViewCol(jl);
        hl_minus_hm1 = hlViewCol(jl) - hViewCol(jl-1);
        vptdrydens_jlm1 = vptdrydens;
        vptdrydens = ptViewCol(jl) * (1.0 + constants::c_virtual * qViewCol(jl)
                     - qclViewCol(jl) - qcfViewCol(jl)) / (1.0 - qViewCol(jl)
                     - qclViewCol(jl) - qcfViewCol(jl));
        vptdrydens_intp = (h_minus_hl * vptdrydens_jlm1 + hl_minus_hm1 * vptdrydens) / h_minus_hm1;
        dryrhoViewCol(jl) = pViewCol(jl) *
                         pow(pViewCol(jl) / constants::p_zero, - constants::rd_over_cp) /
                         (constants::rd * vptdrydens_intp);
      }
    },
    stateFlds["height_above_mean_sea_level"],
    stateFlds["height_above_mean_sea_level_levels"],
    stateFlds["air_pressure_levels_minus_one"],
    stateFlds["air_potential_temperature"],
    stateFlds[specific_humidity_mo],
    stateFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"],
    stateFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"],
    stateFlds["dry_air_density_levels_minus_one"]);

  stateFlds["dry_air_density_levels_minus_one"].set_dirty();

  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_nl()] ... exit"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void eval_dry_air_density_from_pressure_levels_minus_one_tl(atlas::FieldSet & incFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_tl()] starting ..."
                     << std::endl;

  const idx_t numLevels = stateFlds["dry_air_density_levels_minus_one"].shape(1);

  util::for_each_column(
    [=](ConstView dryrhoViewCol,
        ConstView hViewCol,
        ConstView hlViewCol,
        ConstView pViewCol,
        ConstView ptViewCol,
        ConstView qViewCol,
        ConstView qcfViewCol,
        ConstView qclViewCol,
        ConstView pIncViewCol,
        ConstView ptIncViewCol,
        ConstView qIncViewCol,
        ConstView qcfIncViewCol,
        ConstView qclIncViewCol,
        View dryrhoIncViewCol) {
      double h_minus_hl;
      double hl_minus_hm1;
      double vptdrydens;
      double vptdrydensInc;
      double vptdrydens_intp_times_h_minus_hm1;
      double vptdrydensInc_intp_times_h_minus_hm1;
      double vptdrydens_jlm1;
      double vptdrydensInc_jlm1;

      vptdrydens = ptViewCol(0) * (1.0 + constants::c_virtual * qViewCol(0)
                   - qclViewCol(0) - qcfViewCol(0)) / (1.0 - qViewCol(0)
                   - qclViewCol(0) - qcfViewCol(0));
      vptdrydensInc = ((1.0 + constants::c_virtual * qViewCol(0)
                        - qclViewCol(0) - qcfViewCol(0)) * ptIncViewCol(0)
                       + ((1.0 + constants::c_virtual) * qIncViewCol(0)
                          * (1.0 - qclViewCol(0) - qcfViewCol(0)) * ptViewCol(0)
                          / (1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0)))
                      + (qclIncViewCol(0) *  qViewCol(0)
                         * (1.0 + constants::c_virtual) * ptViewCol(0))
                         / (1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0))
                      + (qcfIncViewCol(0) *  qViewCol(0)
                         * (1.0 + constants::c_virtual) * ptViewCol(0))
                         / (1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0)))
                      / (1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0));
      dryrhoIncViewCol(0) = dryrhoViewCol(0) * ((1.0 - constants::rd_over_cp)
                                             * pIncViewCol(0) / pViewCol(0)
                                             - vptdrydensInc / vptdrydens);

      for (idx_t jl = 1; jl < numLevels; ++jl) {
        h_minus_hl = hViewCol(jl) - hlViewCol(jl);
        hl_minus_hm1 = hlViewCol(jl) - hViewCol(jl-1);
        vptdrydens_jlm1 = vptdrydens;
        vptdrydens = ptViewCol(jl) * (1.0 + constants::c_virtual * qViewCol(jl)
                       - qclViewCol(jl) - qcfViewCol(jl)) / (1.0 - qViewCol(jl)
                       - qclViewCol(jl) - qcfViewCol(jl));
        vptdrydensInc_jlm1 = vptdrydensInc;
        vptdrydensInc = ((1.0 + constants::c_virtual * qViewCol(jl)
                          - qclViewCol(jl) - qcfViewCol(jl)) * ptIncViewCol(jl)
                         + ((1.0 + constants::c_virtual) * qIncViewCol(jl)
                            * (1.0 - qclViewCol(jl) - qcfViewCol(jl)) * ptViewCol(jl)
                            / (1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl)))
                         + (qclIncViewCol(jl) *  qViewCol(jl)
                            * (1.0 + constants::c_virtual) * ptViewCol(jl))
                            / (1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl))
                         + (qcfIncViewCol(jl) *  qViewCol(jl)
                            * (1.0 + constants::c_virtual) * ptViewCol(jl))
                            / (1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl)))
                        / (1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl));
        vptdrydens_intp_times_h_minus_hm1 = h_minus_hl * vptdrydens_jlm1 +
                                            hl_minus_hm1 * vptdrydens;
        vptdrydensInc_intp_times_h_minus_hm1 = hl_minus_hm1 * vptdrydensInc +
                                               h_minus_hl * vptdrydensInc_jlm1;
        dryrhoIncViewCol(jl) = dryrhoViewCol(jl) * ((1.0 - constants::rd_over_cp)
                               * pIncViewCol(jl) / pViewCol(jl)
                               - vptdrydensInc_intp_times_h_minus_hm1
                               / vptdrydens_intp_times_h_minus_hm1);
      }
    },
    stateFlds["dry_air_density_levels_minus_one"],
    stateFlds["height_above_mean_sea_level"],
    stateFlds["height_above_mean_sea_level_levels"],
    stateFlds["air_pressure_levels_minus_one"],
    stateFlds["air_potential_temperature"],
    stateFlds[specific_humidity_mo],
    stateFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"],
    stateFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"],
    incFlds["air_pressure_levels_minus_one"],
    incFlds["air_potential_temperature"],
    incFlds[specific_humidity_mo],
    incFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"],
    incFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"],
    incFlds["dry_air_density_levels_minus_one"]);

  incFlds["dry_air_density_levels_minus_one"].set_dirty();

  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_tl()] ... exit"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void eval_dry_air_density_from_pressure_levels_minus_one_ad(atlas::FieldSet & hatFlds,
                                            const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_ad()] starting ..."
                     << std::endl;

  const idx_t numLevels = stateFlds["dry_air_density_levels_minus_one"].shape(1);

  util::for_each_column(
    [=](ConstView dryrhoViewCol,
        ConstView hViewCol,
        ConstView hlViewCol,
        ConstView pViewCol,
        ConstView ptViewCol,
        ConstView qViewCol,
        ConstView qcfViewCol,
        ConstView qclViewCol,
        View dryrhoHatViewCol,
        View pHatViewCol,
        View ptHatViewCol,
        View qHatViewCol,
        View qcfHatViewCol,
        View qclHatViewCol) {
      double h_minus_hl;
      double hl_minus_hm1;
      double vptdrydens;
      double vptdrydensHat;
      double vptdrydens_intp_times_h_minus_hm1;
      double vptdrydens_jlm1;
      double vptdrydensHat_jlm1;

      for (idx_t jl = numLevels-1; jl >= 1; --jl) {
        h_minus_hl = hViewCol(jl) - hlViewCol(jl);
        hl_minus_hm1 = hlViewCol(jl) - hViewCol(jl-1);
        pHatViewCol(jl) += dryrhoViewCol(jl) * (1.0 - constants::rd_over_cp) /
                           pViewCol(jl) * dryrhoHatViewCol(jl);
        vptdrydens = ptViewCol(jl) * (1.0 + constants::c_virtual * qViewCol(jl)
                     - qclViewCol(jl) - qcfViewCol(jl)) / (1.0 - qViewCol(jl)
                     - qclViewCol(jl) - qcfViewCol(jl));
        vptdrydens_jlm1 = ptViewCol(jl-1) * (1.0 + constants::c_virtual * qViewCol(jl-1)
                          - qclViewCol(jl-1) - qcfViewCol(jl-1)) / (1.0 - qViewCol(jl-1)
                          - qclViewCol(jl-1) - qcfViewCol(jl-1));
        vptdrydens_intp_times_h_minus_hm1 = h_minus_hl * vptdrydens_jlm1 +
                                            hl_minus_hm1 * vptdrydens;
        vptdrydensHat = - dryrhoViewCol(jl) * dryrhoHatViewCol(jl) * hl_minus_hm1
                        / vptdrydens_intp_times_h_minus_hm1;
        vptdrydensHat_jlm1 = - dryrhoViewCol(jl) * dryrhoHatViewCol(jl) * h_minus_hl /
                             vptdrydens_intp_times_h_minus_hm1;
        ptHatViewCol(jl) += (1.0 + constants::c_virtual * qViewCol(jl)
                             - qclViewCol(jl) - qcfViewCol(jl))
                             / (1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl))
                             * vptdrydensHat;
        qHatViewCol(jl) += (1.0 + constants::c_virtual) * ptViewCol(jl)
                            * (1.0 - qclViewCol(jl) - qcfViewCol(jl))
                            / ((1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl))
                            * (1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl)))
                            * vptdrydensHat;
        qclHatViewCol(jl) += ptViewCol(jl) *  qViewCol(jl) * (1.0 + constants::c_virtual)
                             / ((1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl))
                             * (1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl)))
                             * vptdrydensHat;
        qcfHatViewCol(jl) += ptViewCol(jl) *  qViewCol(jl) * (1.0 + constants::c_virtual)
                             / ((1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl))
                             * (1.0 - qViewCol(jl) - qclViewCol(jl) - qcfViewCol(jl)))
                             * vptdrydensHat;
        ptHatViewCol(jl-1) += (1.0 + constants::c_virtual * qViewCol(jl-1)
                               - qclViewCol(jl-1) - qcfViewCol(jl-1))
                               / (1.0 - qViewCol(jl-1) - qclViewCol(jl-1) - qcfViewCol(jl-1))
                               * vptdrydensHat_jlm1;
        qHatViewCol(jl-1) += (1.0 + constants::c_virtual) * ptViewCol(jl-1)
                              * (1.0 - qclViewCol(jl-1) - qcfViewCol(jl-1))
                              / ((1.0 - qViewCol(jl-1) - qclViewCol(jl-1) - qcfViewCol(jl-1))
                              * (1.0 - qViewCol(jl-1) - qclViewCol(jl-1) - qcfViewCol(jl-1)))
                              * vptdrydensHat_jlm1;
        qclHatViewCol(jl-1) += ptViewCol(jl-1) *  qViewCol(jl-1) * (1.0 + constants::c_virtual)
                               / ((1.0 - qViewCol(jl-1) - qclViewCol(jl-1) - qcfViewCol(jl-1))
                               * (1.0 - qViewCol(jl-1) - qclViewCol(jl-1) - qcfViewCol(jl-1)))
                               * vptdrydensHat_jlm1;
        qcfHatViewCol(jl-1) += ptViewCol(jl-1) *  qViewCol(jl-1) * (1.0 + constants::c_virtual)
                               / ((1.0 - qViewCol(jl-1) - qclViewCol(jl-1) - qcfViewCol(jl-1))
                               * (1.0 - qViewCol(jl-1) - qclViewCol(jl-1) - qcfViewCol(jl-1)))
                               * vptdrydensHat_jlm1;
        dryrhoHatViewCol(jl) = 0.0;
      }
      pHatViewCol(0) += dryrhoViewCol(0) * ((1.0 - constants::rd_over_cp) /
                        pViewCol(0)) * dryrhoHatViewCol(0);
      vptdrydens = ptViewCol(0) *
                   (1.0 + constants::c_virtual * qViewCol(0) - qclViewCol(0) - qcfViewCol(0)) /
                   (1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0));
      vptdrydensHat = - dryrhoViewCol(0) * dryrhoHatViewCol(0) / vptdrydens;
      ptHatViewCol(0) += (1.0 + constants::c_virtual * qViewCol(0)
                          - qclViewCol(0) - qcfViewCol(0))
                          / (1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0)) * vptdrydensHat;
      qHatViewCol(0) += (1.0 + constants::c_virtual) * ptViewCol(0)
                         * (1.0 - qclViewCol(0) - qcfViewCol(0))
                         / ((1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0))
                         * (1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0))) * vptdrydensHat;
      qclHatViewCol(0) += ptViewCol(0) *  qViewCol(0) * (1.0 + constants::c_virtual)
                          / ((1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0))
                          * (1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0))) * vptdrydensHat;
      qcfHatViewCol(0) += ptViewCol(0) *  qViewCol(0) * (1.0 + constants::c_virtual)
                          / ((1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0))
                          * (1.0 - qViewCol(0) - qclViewCol(0) - qcfViewCol(0))) * vptdrydensHat;
      dryrhoHatViewCol(0) = 0.0;
    },
    stateFlds["dry_air_density_levels_minus_one"],
    stateFlds["height_above_mean_sea_level"],
    stateFlds["height_above_mean_sea_level_levels"],
    stateFlds["air_pressure_levels_minus_one"],
    stateFlds["air_potential_temperature"],
    stateFlds[specific_humidity_mo],
    stateFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"],
    stateFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"],
    hatFlds["dry_air_density_levels_minus_one"],
    hatFlds["air_pressure_levels_minus_one"],
    hatFlds["air_potential_temperature"],
    hatFlds[specific_humidity_mo],
    hatFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"],
    hatFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);

  hatFlds["dry_air_density_levels_minus_one"].set_dirty();
  hatFlds["air_pressure_levels_minus_one"].set_dirty();
  hatFlds["air_potential_temperature"].set_dirty();
  hatFlds[specific_humidity_mo].set_dirty();
  hatFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();
  hatFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();

  oops::Log::trace() << "[eval_dry_air_density_from_pressure_levels_minus_one_ad()] ... exit"
                     << std::endl;
}

}  // namespace mo
