/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "atlas/array/MakeView.h"
#include "atlas/field.h"
#include "atlas/parallel/omp/omp.h"

#include "mo/eval_ratio.h"
#include "mo/eval_water_vapor_mixing_ratio.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

namespace {
  const char q_moist_condensed_mo[] = "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water";
  const char q2m_moist_condensed_mo[] =
                                "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m";
  const char q_moist_mo[] = "water_vapor_mixing_ratio_wrt_moist_air";
  const char q2m_moist_mo[] = "water_vapor_mixing_ratio_wrt_moist_air_at_2m";
}  // namespace

namespace mo {

using atlas::array::make_view;
using atlas::idx_t;

// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl(
        atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] starting ..."
          << std::endl;

  std::vector<std::string> fnames {"water_vapor_mixing_ratio_wrt_dry_air",
                                   "total_water_mixing_ratio_wrt_dry_air",
                                   q_moist_condensed_mo};

  eval_ratio_to_second(stateFlds, fnames);

  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] ... exit"
          << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_tl(
        atlas::FieldSet & incFlds,
        const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_tl()] starting ..."
          << std::endl;

  std::vector<std::string> fnames {"water_vapor_mixing_ratio_wrt_dry_air",
                                   "total_water_mixing_ratio_wrt_dry_air",
                                   q_moist_condensed_mo};

  eval_ratio_to_second_tl(incFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_tl()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_ad(
        atlas::FieldSet & hatFlds,
        const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_ad()] starting ..."
          << std::endl;

  std::vector<std::string> fnames {"water_vapor_mixing_ratio_wrt_dry_air",
                                   "total_water_mixing_ratio_wrt_dry_air",
                                   q_moist_condensed_mo};

  eval_ratio_to_second_ad(hatFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_ad()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_nl(
        atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_nl()]"
          << " starting ..." << std::endl;

  const auto ds_q = make_view<const double, 2>(stateFlds[q_moist_condensed_mo]);
  auto ds_q2m = make_view<double, 2>(stateFlds[q2m_moist_condensed_mo]);
  const idx_t sizeOwned =
    util::getSizeOwned(
      stateFlds[q2m_moist_condensed_mo].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    ds_q2m(jn, 0) = ds_q(jn, 0);
  }
  stateFlds[q2m_moist_condensed_mo].set_dirty();

  oops::Log::trace()
    << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_nl()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_tl(
        atlas::FieldSet & incFlds,
        const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_tl()]"
          << " starting ..." << std::endl;

  auto q2m_inc = make_view<double, 2>(incFlds[q2m_moist_condensed_mo]);
  auto q_inc = make_view<const double, 2>(incFlds[q_moist_condensed_mo]);
  const idx_t sizeOwned =
    util::getSizeOwned(
      incFlds[q2m_moist_condensed_mo].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    q2m_inc(jn, 0) = q_inc(jn, 0);
  }
  incFlds[q2m_moist_condensed_mo].set_dirty();

  oops::Log::trace()
    << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_tl()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_ad(
        atlas::FieldSet & hatFlds,
        const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_ad()]"
    << " starting ..." << std::endl;

  auto q2m_hat = make_view<double, 2>(hatFlds[q2m_moist_condensed_mo]);
  auto q_hat = make_view<double, 2>(hatFlds[q_moist_condensed_mo]);
  const idx_t sizeOwned =
    util::getSizeOwned(
      hatFlds[q_moist_condensed_mo].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    q_hat(jn, 0) += q2m_hat(jn, 0);
    q2m_hat(jn, 0) = 0.0;
  }
  hatFlds[q2m_moist_condensed_mo].set_dirty();
  hatFlds[q_moist_condensed_mo].set_dirty();

  oops::Log::trace()
    << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_ad()] ... exit"
    << std::endl;
}

}  // namespace mo
