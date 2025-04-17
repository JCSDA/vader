/*
 * (C) Crown Copyright 2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "mo/eval_cloud_liquid_water_mixing_ratio.h"
#include "mo/functions.h"

#include "oops/util/Logger.h"

namespace mo {

// --------------------------------------------------------------------------------------

void eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl(
    atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl()]"
    << " starting ..." << std::endl;

  const std::vector<std::string> fnames {
    "cloud_liquid_water_mixing_ratio_wrt_dry_air",
    "total_water_mixing_ratio_wrt_dry_air",
    "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"};

  functions::eval_q_x_nl(stateFlds, fnames);

  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_tl(
    atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_tl()] "
    << "starting ..." << std::endl;

  const std::vector<std::string> fnames {
    "cloud_liquid_water_mixing_ratio_wrt_dry_air",
    "total_water_mixing_ratio_wrt_dry_air",
    "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"};

  functions::eval_q_x_tl(incFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_tl()] ... exit"
    << std::endl;
}
// --------------------------------------------------------------------------------------

void eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_ad(
    atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_ad()] "
    << "starting ..." << std::endl;

  const std::vector<std::string> fnames {
    "cloud_liquid_water_mixing_ratio_wrt_dry_air",
    "total_water_mixing_ratio_wrt_dry_air",
    "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"};

  functions::eval_q_x_ad(hatFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_ad()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_inv_tl(
    atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_inv_tl()]"
    << " starting ..." << std::endl;

  const std::vector<std::string> fnames {
    "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    "total_water_mixing_ratio_wrt_dry_air",
    "cloud_liquid_water_mixing_ratio_wrt_dry_air"};

  functions::eval_q_x_inv_tl(incFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_inv_tl()]"
    << " ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_inv_ad(
    atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_inv_ad()]"
    << "starting ..." << std::endl;

  const std::vector<std::string> fnames {
    "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    "total_water_mixing_ratio_wrt_dry_air",
    "cloud_liquid_water_mixing_ratio_wrt_dry_air"};

  functions::eval_q_x_inv_ad(hatFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_inv_ad()]"
    << " ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_nl()] starting ..."
    << std::endl;

  const std::vector<std::string> fnames {
    "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    "cloud_liquid_water_mixing_ratio_wrt_dry_air"};

  functions::eval_m_x_nl(stateFlds, fnames);

  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_nl()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_tl()] starting ..."
    << std::endl;

  const std::vector<std::string> fnames {
    "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    "cloud_liquid_water_mixing_ratio_wrt_dry_air"};

  functions::eval_m_x_tl(incFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_tl()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_ad()] starting ..."
    << std::endl;

  const std::vector<std::string> fnames {
    "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
    "cloud_liquid_water_mixing_ratio_wrt_dry_air"};

  functions::eval_m_x_ad(hatFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_cloud_liquid_water_mixing_ratio_wrt_dry_air_ad()] ... exit"
    << std::endl;
}

}  // namespace mo
