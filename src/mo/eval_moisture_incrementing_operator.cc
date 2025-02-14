/*
 * (C) Crown Copyright 2023-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array.h"
#include "atlas/field.h"

#include "mo/eval_moisture_incrementing_operator.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace {
  const char specific_humidity_mo[] = "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water";
}  // namespace

namespace mo {


// ------------------------------------------------------------------------------------------------
void eval_total_water_nl(atlas::FieldSet & augStateFlds) {
  oops::Log::trace() << "[eval_total_water_tl()] starting ..." << std::endl;

  auto qView = make_view<const double, 2>(augStateFlds[specific_humidity_mo]);
  auto qclView = make_view<const double, 2>
    (augStateFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto qcfView = make_view<const double, 2>
    (augStateFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);

  auto qtView = make_view<double, 2>(augStateFlds["qt"]);
  const idx_t numLevels = augStateFlds["qt"].shape(1);
  const idx_t sizeOwned =
        util::getSizeOwned(augStateFlds["qt"].functionspace());

  for (idx_t jnode = 0; jnode < sizeOwned; jnode++) {
    for (idx_t jlev = 0; jlev < numLevels; jlev++) {
      qtView(jnode, jlev) = qView(jnode, jlev) + qclView(jnode, jlev) + qcfView(jnode, jlev);
    }
  }
  augStateFlds["qt"].set_dirty();

  oops::Log::trace() << "[eval_total_water_tl()] ... done" << std::endl;
}
// ------------------------------------------------------------------------------------------------
void eval_moisture_incrementing_operator_tl(atlas::FieldSet & incFlds,
                                            const atlas::FieldSet & augStateFlds) {
  oops::Log::trace() << "[eval_moisture_incrementing_operator_tl()] starting ..."
                     << std::endl;
  auto qsatView = make_view<const double, 2>(augStateFlds["qsat"]);
  auto dlsvpdTView = make_view<const double, 2>(augStateFlds["dlsvpdT"]);
  auto cleffView = make_view<const double, 2>(augStateFlds["cleff"]);
  auto cfeffView = make_view<const double, 2>(augStateFlds["cfeff"]);
  auto qtIncView = make_view<const double, 2>(incFlds["qt"]);
  auto temperIncView = make_view<const double, 2>(incFlds["air_temperature"]);

  auto qclIncView = make_view<double, 2>
                    (incFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto qcfIncView = make_view<double, 2>
                    (incFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto qIncView = make_view<double, 2>(incFlds[specific_humidity_mo]);
  const idx_t numLevels = incFlds["qt"].shape(1);
  const idx_t sizeOwned =
        util::getSizeOwned(incFlds["qt"].functionspace());

  double maxCldInc;
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      maxCldInc = qtIncView(jn, jl) - qsatView(jn, jl) *
                  dlsvpdTView(jn, jl) * temperIncView(jn, jl);
      qclIncView(jn, jl) = cleffView(jn, jl) * maxCldInc;
      qcfIncView(jn, jl) = cfeffView(jn, jl) * maxCldInc;
      qIncView(jn, jl) = qtIncView(jn, jl) - qclIncView(jn, jl) - qcfIncView(jn, jl);
    }
  }
  incFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();
  incFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();
  incFlds[specific_humidity_mo].set_dirty();

  oops::Log::trace() << "[eval_moisture_incrementing_operator_tl()] ... done"
                     << std::endl;
}

// ------------------------------------------------------------------------------------------------
void eval_moisture_incrementing_operator_ad(atlas::FieldSet & hatFlds,
                             const atlas::FieldSet & augStateFlds) {
  oops::Log::trace() << "[eval_moisture_incrementing_operator_ad()] starting ..."
                     << std::endl;
  auto qsatView = make_view<const double, 2>(augStateFlds["qsat"]);
  auto dlsvpdTView = make_view<const double, 2>(augStateFlds["dlsvpdT"]);
  auto cleffView = make_view<const double, 2>(augStateFlds["cleff"]);
  auto cfeffView = make_view<const double, 2>(augStateFlds["cfeff"]);

  auto temperHatView = make_view<double, 2>(hatFlds["air_temperature"]);
  auto qtHatView = make_view<double, 2>(hatFlds["qt"]);
  auto qHatView = make_view<double, 2>(hatFlds[specific_humidity_mo]);
  auto qclHatView = make_view<double, 2>
                    (hatFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto qcfHatView = make_view<double, 2>
                    (hatFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  const idx_t numLevels = hatFlds["qt"].shape(1);
  const idx_t sizeOwned =
        util::getSizeOwned(hatFlds["qt"].functionspace());

  double qsatdlsvpdT;
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      qsatdlsvpdT = qsatView(jn, jl) * dlsvpdTView(jn, jl);
      temperHatView(jn, jl) += ((cleffView(jn, jl) + cfeffView(jn, jl)) * qHatView(jn, jl)
                                - cleffView(jn, jl) * qclHatView(jn, jl)
                                - cfeffView(jn, jl) * qcfHatView(jn, jl)) * qsatdlsvpdT;
      qtHatView(jn, jl) += cleffView(jn, jl) * qclHatView(jn, jl)
              + cfeffView(jn, jl) * qcfHatView(jn, jl)
              + (1.0 - cleffView(jn, jl) - cfeffView(jn, jl))
              * qHatView(jn, jl);
      qHatView(jn, jl) = 0.0;
      qclHatView(jn, jl) = 0.0;
      qcfHatView(jn, jl) = 0.0;
    }
  }
  hatFlds["air_temperature"].set_dirty();
  hatFlds[specific_humidity_mo].set_dirty();
  hatFlds["qt"].set_dirty();
  hatFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();
  hatFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();

  oops::Log::trace() << "[eval_moisture_incrementing_operator_ad()] ... done"
                     << std::endl;
}

// ------------------------------------------------------------------------------------------------
void eval_total_water_tl(atlas::FieldSet & incFlds,
                         const atlas::FieldSet & augStateFlds) {
  oops::Log::trace() << "[eval_total_water_tl()] starting ..." << std::endl;

  auto qIncView = make_view<const double, 2>(incFlds[specific_humidity_mo]);
  auto qclIncView = make_view<const double, 2>
                    (incFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto qcfIncView = make_view<const double, 2>
                      (incFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);

  auto qtIncView = make_view<double, 2>(incFlds["qt"]);
  const idx_t numLevels = incFlds["qt"].shape(1);
  const idx_t sizeOwned =
        util::getSizeOwned(incFlds["qt"].functionspace());

  for (idx_t jnode = 0; jnode < sizeOwned; jnode++) {
    for (idx_t jlev = 0; jlev < numLevels; jlev++) {
      qtIncView(jnode, jlev) = qIncView(jnode, jlev)
                             + qclIncView(jnode, jlev)
                             + qcfIncView(jnode, jlev);
    }
  }
  incFlds["qt"].set_dirty();

  oops::Log::trace() << "[eval_total_water_tl()] ... done" << std::endl;
}

// ------------------------------------------------------------------------------------------------
void eval_total_water_ad(atlas::FieldSet & hatFlds,
                         const atlas::FieldSet & augStateFlds) {
  oops::Log::trace() << "[eval_total_water_ad()] starting ..." << std::endl;
  auto qIncView = make_view<double, 2>(hatFlds[specific_humidity_mo]);
  auto qclIncView = make_view<double, 2>
                    (hatFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto qcfIncView = make_view<double, 2>
                      (hatFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"]);
  auto qtIncView = make_view<double, 2>(hatFlds["qt"]);
  const idx_t numLevels = hatFlds["qt"].shape(1);
  const idx_t sizeOwned =
        util::getSizeOwned(hatFlds["qt"].functionspace());

  for (idx_t jnode = 0; jnode < sizeOwned; jnode++) {
    for (idx_t jlev = 0; jlev < numLevels; jlev++) {
      qIncView(jnode, jlev) += qtIncView(jnode, jlev);
      qclIncView(jnode, jlev) += qtIncView(jnode, jlev);
      qcfIncView(jnode, jlev) += qtIncView(jnode, jlev);
      qtIncView(jnode, jlev) = 0.0;
    }
  }
  hatFlds[specific_humidity_mo].set_dirty();
  hatFlds["cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();
  hatFlds["cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"].set_dirty();
  hatFlds["qt"].set_dirty();

  oops::Log::trace() << "[eval_total_water_ad()] ... done" << std::endl;
}

// ------------------------------------------------------------------------------------------------
void eval_total_water(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_total_water()] starting ..." << std::endl;
  eval_total_water_tl(stateFlds, stateFlds);
  oops::Log::trace() << "[eval_total_water()] ... done" << std::endl;
}


}  // namespace mo
