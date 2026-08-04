/*
 * (C) Crown Copyright 2023-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <string>
#include <tuple>

#include "atlas/array.h"
#include "atlas/field/for_each.h"
#include "atlas/functionspace.h"

#include "mo/eval_relative_humidity.h"
#include "mo/eval_sat_vapour_pressure.h"

#include "oops/util/for_each.h"
#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;
using atlas::util::Config;

using View = atlas::array::LocalView<double, 1>;
using ConstView = atlas::array::LocalView<const double, 1>;

namespace {
  const char specific_humidity_mo[] = "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water";
  const char relative_humidity_at_2m_percentage[] = "relative_humidity_at_2m_percentage";
  const char relative_humidity_at_2m[] = "relative_humidity_at_2m";
  constexpr double relative_humidity_percent_to_fraction = 0.01;
}  // namespace

namespace mo {

// --------------------------------------------------------------------------------------

void eval_relative_humidity_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_relative_humidity_nl()] starting ..." << std::endl;

  bool cap_super_sat(false);

  if (stateFlds["relative_humidity"].metadata().has("cap_super_sat")) {
    stateFlds["relative_humidity"].metadata().get("cap_super_sat", cap_super_sat);
  }

  // Relative humidity has units from 0 to 1 according to the ESM convention.
  const auto & ghost = stateFlds["relative_humidity"].functionspace().ghost();
  atlas::field::for_each_value_masked(ghost,
                                      stateFlds[specific_humidity_mo],
                                      stateFlds["qsat"],
                                      stateFlds["relative_humidity"],
                                      [&](const double q, const double qsat, double& rh) {
      rh = fmax(q / qsat, 0.0);
      rh = (cap_super_sat && (rh > 1.0)) ? 1.0 : rh;
  });
  stateFlds["relative_humidity"].set_dirty();

  oops::Log::trace() << "[eval_relative_humidity_nl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_relative_humidity_tl(atlas::FieldSet & incFlds,
                               const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_relative_humidity_tl()] starting ..." << std::endl;
  // * This assumes air pressure increments can be neglected (usual approximation)
  // * Only part of this is the tangent linear of eval_relative_humidity_nl.
  // * This neglects supersaturation in eval_relative_humidity_nl


  // State variables qsat and dlsvpdT can be calculated by eval_sat_vapour_pressure_nl
  // and evalSatSpecificHumidity from air_temperature and air_pressure inputs
  const auto & ghost = incFlds["relative_humidity"].functionspace().ghost();
  atlas::field::for_each_value_masked(ghost,
                                      std::make_tuple(incFlds[specific_humidity_mo],
                                                      incFlds["air_temperature"],
                                                      incFlds["relative_humidity"],
                                                      stateFlds[specific_humidity_mo],
                                                      stateFlds["qsat"],
                                                      stateFlds["dlsvpdT"]),
          [](const double qInc, const double tInc, double& rhInc,
             const double q, const double qsat, const double dlsvpdT) {
            rhInc = (qInc - q * dlsvpdT * tInc) / qsat;
          });

  incFlds["relative_humidity"].set_dirty();

  oops::Log::trace() << "[eval_relative_humidity_tl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_relative_humidity_ad(atlas::FieldSet & hatFlds,
                               const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_relative_humidity_ad()] starting ..." << std::endl;

  const auto & ghost = hatFlds["relative_humidity"].functionspace().ghost();
  atlas::field::for_each_value_masked(ghost,
                                      std::make_tuple(hatFlds[specific_humidity_mo],
                                                      hatFlds["air_temperature"],
                                                      hatFlds["relative_humidity"],
                                                      stateFlds[specific_humidity_mo],
                                                      stateFlds["qsat"],
                                                      stateFlds["dlsvpdT"]),
          [](double& qHat, double& tHat, double& rhHat,
             const double q, const double qsat, const double dlsvpdT) {
            qHat += rhHat / qsat;
            tHat -= q * dlsvpdT * rhHat / qsat;
            rhHat = 0.0;
          });

  hatFlds[specific_humidity_mo].set_dirty();
  hatFlds["air_temperature"].set_dirty();
  hatFlds["relative_humidity"].set_dirty();

  oops::Log::trace() << "[eval_relative_humidity_ad()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_relative_humidity_at_2m_percentage_to_fraction_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_relative_humidity_at_2m_percentage_to_fraction_nl()] starting ..."
                     << std::endl;

  util::for_each_column(
    [=] (ConstView rh2mPercentView,
         View rh2mView) {
      rh2mView(0) = rh2mPercentView(0) * relative_humidity_percent_to_fraction;
    },
    stateFlds[relative_humidity_at_2m_percentage],
    stateFlds[relative_humidity_at_2m]);

  stateFlds[relative_humidity_at_2m].set_dirty();

  oops::Log::trace() << "[eval_relative_humidity_at_2m_percentage_to_fraction_nl()] ... exit"
                     << std::endl;
}


// --------------------------------------------------------------------------------------

void eval_relative_humidity_at_2m_percentage_to_fraction_tl(atlas::FieldSet & incFlds) {
  oops::Log::trace() << "[eval_relative_humidity_at_2m_percentage_to_fraction_tl()] starting ..."
                     << std::endl;

  util::for_each_column(
    [=] (ConstView rh2mPercentIncView,
         View rh2mIncView) {
      rh2mIncView(0) = rh2mPercentIncView(0) * relative_humidity_percent_to_fraction;
    },
    incFlds[relative_humidity_at_2m_percentage],
    incFlds[relative_humidity_at_2m]);

  incFlds[relative_humidity_at_2m].set_dirty();

  oops::Log::trace() << "[eval_relative_humidity_at_2m_percentage_to_fraction_tl()] ... exit"
                     << std::endl;
}


// --------------------------------------------------------------------------------------

void eval_relative_humidity_at_2m_percentage_to_fraction_ad(atlas::FieldSet & hatFlds) {
  oops::Log::trace() << "[eval_relative_humidity_at_2m_percentage_to_fraction_ad()] starting ..."
                     << std::endl;

  util::for_each_column(
    [=] (View rh2mPercentHatView,
         View rh2mHatView) {
      rh2mPercentHatView(0) += rh2mHatView(0) * relative_humidity_percent_to_fraction;
      rh2mHatView(0) = 0.0;
    },
    hatFlds[relative_humidity_at_2m_percentage],
    hatFlds[relative_humidity_at_2m]);

  hatFlds[relative_humidity_at_2m_percentage].set_dirty();
  hatFlds[relative_humidity_at_2m].set_dirty();

  oops::Log::trace() << "[eval_relative_humidity_at_2m_percentage_to_fraction_ad()] ... exit"
                     << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_relative_humidity_at_2m_tl(atlas::FieldSet & incFlds) {
  oops::Log::trace() << "[eval_relative_humidity_at_2m_tl()] starting ..." << std::endl;
  const auto rhIncView = make_view<double, 2>(incFlds["relative_humidity"]);
  auto rh2mIncView = make_view<double, 2>(incFlds["relative_humidity_at_2m"]);
  const idx_t sizeOwned =
    util::getSizeOwned(incFlds["relative_humidity_at_2m"].functionspace());

  for (atlas::idx_t jn = 0; jn < sizeOwned; ++jn) {
      rh2mIncView(jn, 0) = rhIncView(jn, 0);
  }
  incFlds["relative_humidity_at_2m"].set_dirty();

  oops::Log::trace()<< "[eval_relative_humidity_at_2m_tl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_relative_humidity_at_2m_ad(atlas::FieldSet & hatFlds) {
  oops::Log::trace() << "[eval_relative_humidity_at_2m_ad()] starting ..." << std::endl;
  auto rhHatView = make_view<double, 2>(hatFlds["relative_humidity"]);
  auto rh2mHatView = make_view<double, 2>(hatFlds["relative_humidity_at_2m"]);
  const idx_t sizeOwned =
    util::getSizeOwned(hatFlds["relative_humidity_at_2m"].functionspace());

  for (atlas::idx_t jn = 0; jn < sizeOwned; ++jn) {
    rhHatView(jn, 0) += rh2mHatView(jn, 0);
    rh2mHatView(jn, 0) = 0.0;
  }
  hatFlds["relative_humidity"].set_dirty();
  hatFlds["relative_humidity_at_2m"].set_dirty();

  oops::Log::trace()<< "[eval_relative_humidity_at_2m_ad()] ... exit" << std::endl;
}


// --------------------------------------------------------------------------------------

void eval_relative_humidity_at_2m_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_relative_humidity_at_2m_nl()] starting ..." << std::endl;

  bool cap_super_sat(false);

  if (stateFlds["relative_humidity_at_2m"].metadata().has("cap_super_sat")) {
    stateFlds["relative_humidity_at_2m"].metadata().get("cap_super_sat", cap_super_sat);
  }

  // Relative humidity has units from 0 to 1 according to the ESM convention.
  const idx_t sizeOwned =
    util::getSizeOwned(stateFlds["relative_humidity_at_2m"].functionspace());

  const auto qView = make_view<double, 2>(stateFlds[specific_humidity_mo]);
  const auto qsatView = make_view<double, 2>(stateFlds["qsat"]);
  auto rh2mView = make_view<double, 2>(stateFlds["relative_humidity_at_2m"]);

  for (atlas::idx_t jn = 0; jn < sizeOwned; ++jn) {
    rh2mView(jn, 0) = fmax(qView(jn, 0) / qsatView(jn, 0), 0.0);
    rh2mView(jn, 0) = (cap_super_sat && (rh2mView(jn, 0) > 1.0)) ? 1.0 : rh2mView(jn, 0);
  }

  stateFlds["relative_humidity_at_2m"].set_dirty();

  oops::Log::trace()<< "[eval_relative_humidity_at_2m_nl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_relative_humidity_at_2m_from_temp_tl(atlas::FieldSet & incFlds,
                                               const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_relative_humidity_at_2m_from_temp_tl()] starting ..." << std::endl;

  // * This assumes air pressure increments can be neglected (usual approximation)
  // * Only part of this is the tangent linear of eval_relative_humidity_nl.
  // * This neglects supersaturation in eval_relative_humidity_nl

  // State variables qsat and dlsvpdT can be calculated by eval_sat_vapour_pressure_nl
  // and evalSatSpecificHumidity from air_temperature and air_pressure inputs
  util::for_each_column(
    [=] (ConstView q,
         ConstView qsat,
         ConstView dlsvpdt,
         ConstView qInc,
         ConstView tInc,
         View rh2mInc) {
      rh2mInc(0) = (qInc(0) - q(0) * dlsvpdt(0) * tInc(0)) / qsat(0);
    },
    stateFlds[specific_humidity_mo],
    stateFlds["qsat"],
    stateFlds["dlsvpdT"],
    incFlds[specific_humidity_mo],
    incFlds["air_temperature_at_2m"],
    incFlds["relative_humidity_at_2m"]);

  incFlds["relative_humidity_at_2m"].set_dirty();

  oops::Log::trace()<< "[eval_relative_humidity_at_2m_from_temp_tl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_relative_humidity_at_2m_from_temp_ad(atlas::FieldSet & hatFlds,
                                               const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_relative_humidity_at_2m_from_temp_ad()] starting ..." << std::endl;

  // State variables qsat and dlsvpdT can be calculated by eval_sat_vapour_pressure_nl
  // and evalSatSpecificHumidity from air_temperature and air_pressure inputs
  util::for_each_column(
    [=] (ConstView q,
         ConstView qsat,
         ConstView dlsvpdt,
         View qHat,
         View tHat,
         View rh2mHat) {
      qHat(0) += rh2mHat(0) / qsat(0);
      tHat(0) -= q(0) * dlsvpdt(0) * rh2mHat(0) / qsat(0);
      rh2mHat(0) = 0.0;
    },
    stateFlds[specific_humidity_mo],
    stateFlds["qsat"],
    stateFlds["dlsvpdT"],
    hatFlds[specific_humidity_mo],
    hatFlds["air_temperature_at_2m"],
    hatFlds["relative_humidity_at_2m"]);

  hatFlds[specific_humidity_mo].set_dirty();
  hatFlds["air_temperature_at_2m"].set_dirty();
  hatFlds["relative_humidity_at_2m"].set_dirty();

  oops::Log::trace()<< "[eval_relative_humidity_at_2m__from_temp_ad()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

}  // namespace mo
