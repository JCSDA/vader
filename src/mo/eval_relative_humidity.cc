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

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;
using atlas::util::Config;

namespace {
  const char specific_humidity_mo[] = "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water";
}  // namespace

namespace mo {

// --------------------------------------------------------------------------------------

void eval_relative_humidity_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_relative_humidity_nl()] starting ..." << std::endl;

  bool cap_super_sat(false);

  if (stateFlds["relative_humidity"].metadata().has("cap_super_sat")) {
    stateFlds["relative_humidity"].metadata().get("cap_super_sat", cap_super_sat);
  }

  // Warning! Relative humidity in saber, vader and ufo has units percents (from 0 to 100),
  // while it should have units 1 (from 0 to 1) according to the CCPP convention.
  const auto & ghost = stateFlds["relative_humidity"].functionspace().ghost();
  atlas::field::for_each_value_masked(ghost,
                                      stateFlds[specific_humidity_mo],
                                      stateFlds["qsat"],
                                      stateFlds["relative_humidity"],
                                      [&](const double q, const double qsat, double& rh) {
      rh = fmax(q / qsat * 100.0, 0.0);
      rh = (cap_super_sat && (rh > 100)) ? 100.0 : rh;
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
            rhInc = 100.0 * (qInc - q * dlsvpdT * tInc) / qsat;
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
            qHat += 100 * rhHat / qsat;
            tHat -= 100 * q * dlsvpdT * rhHat / qsat;
            rhHat = 0.0;
          });

  hatFlds[specific_humidity_mo].set_dirty();
  hatFlds["air_temperature"].set_dirty();
  hatFlds["relative_humidity"].set_dirty();

  oops::Log::trace() << "[eval_relative_humidity_ad()] ... exit" << std::endl;
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

  // Warning! Relative humidity in saber, vader and ufo has units percents (from 0 to 100),
  // while it should have units 1 (from 0 to 1) according to the CCPP convention.
  const idx_t sizeOwned =
    util::getSizeOwned(stateFlds["relative_humidity_at_2m"].functionspace());

  const auto qView = make_view<double, 2>(stateFlds[specific_humidity_mo]);
  const auto qsatView = make_view<double, 2>(stateFlds["qsat"]);
  auto rh2mView = make_view<double, 2>(stateFlds["relative_humidity_at_2m"]);

  for (atlas::idx_t jn = 0; jn < sizeOwned; ++jn) {
    rh2mView(jn, 0) = fmax(qView(jn, 0) / qsatView(jn, 0) * 100.0, 0.0);
    rh2mView(jn, 0) = (cap_super_sat && (rh2mView(jn, 0) > 100)) ? 100.0 : rh2mView(jn, 0);
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
  const auto qView = make_view<double, 2>(stateFlds[specific_humidity_mo]);
  const auto qsatView = make_view<double, 2>(stateFlds["qsat"]);
  const auto dlsvpdtView = make_view<double, 2>(stateFlds["dlsvpdT"]);
  const auto qIncView = make_view<double, 2>(incFlds[specific_humidity_mo]);
  const auto tIncView = make_view<double, 2>(incFlds["air_temperature_at_2m"]);
  auto rh2mIncView = make_view<double, 2>(incFlds["relative_humidity_at_2m"]);
  const idx_t sizeOwned =
    util::getSizeOwned(incFlds["relative_humidity_at_2m"].functionspace());

  for (atlas::idx_t jn = 0; jn < sizeOwned; ++jn) {
      rh2mIncView(jn, 0) = 100.0 * (qIncView(jn, 0) - qView(jn, 0) *
                                    dlsvpdtView(jn, 0) * tIncView(jn, 0)) / qsatView(jn, 0);
  }
  incFlds["relative_humidity_at_2m"].set_dirty();

  oops::Log::trace()<< "[eval_relative_humidity_at_2m_from_temp_tl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_relative_humidity_at_2m_from_temp_ad(atlas::FieldSet & hatFlds,
                                               const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_relative_humidity_at_2m_from_temp_ad()] starting ..." << std::endl;

  // State variables qsat and dlsvpdT can be calculated by eval_sat_vapour_pressure_nl
  // and evalSatSpecificHumidity from air_temperature and air_pressure inputs
  const auto qView = make_view<double, 2>(stateFlds[specific_humidity_mo]);
  const auto qsatView = make_view<double, 2>(stateFlds["qsat"]);
  const auto dlsvpdtView = make_view<double, 2>(stateFlds["dlsvpdT"]);
  auto qHatView = make_view<double, 2>(hatFlds[specific_humidity_mo]);
  auto tHatView = make_view<double, 2>(hatFlds["air_temperature_at_2m"]);
  auto rh2mHatView = make_view<double, 2>(hatFlds["relative_humidity_at_2m"]);
  const idx_t sizeOwned =
    util::getSizeOwned(hatFlds["relative_humidity_at_2m"].functionspace());

  for (atlas::idx_t jn = 0; jn < sizeOwned; ++jn) {
    qHatView(jn, 0) += 100 * rh2mHatView(jn, 0) / qsatView(jn, 0);
    tHatView(jn, 0) -= 100 * qView(jn, 0) * dlsvpdtView(jn, 0) *
      rh2mHatView(jn, 0) / qsatView(jn, 0);
    rh2mHatView(jn, 0) = 0.0;
  }

  hatFlds[specific_humidity_mo].set_dirty();
  hatFlds["air_temperature_at_2m"].set_dirty();
  hatFlds["relative_humidity_at_2m"].set_dirty();

  oops::Log::trace()<< "[eval_relative_humidity_at_2m__from_temp_ad()] ... exit" << std::endl;
}


// --------------------------------------------------------------------------------------

}  // namespace mo
