/*
 * (C) Crown Copyright 2023-2025 Met Office
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

}  // namespace mo
