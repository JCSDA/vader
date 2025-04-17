/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "mo/eval_rain_mixing_ratio.h"
#include "mo/functions.h"

#include "oops/util/Logger.h"

namespace mo {

// --------------------------------------------------------------------------------------

void eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_nl(
    atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] starting ..."
    << std::endl;

  const std::vector<std::string> fnames {
    "rain_mixing_ratio_wrt_dry_air",
    "total_water_mixing_ratio_wrt_dry_air",
    "qrain"};

  functions::eval_q_x_nl(stateFlds, fnames);

  oops::Log::trace()
    << "[eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_tl(
    atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_tl()] starting ..."
    << std::endl;

  const std::vector<std::string> fnames {
    "rain_mixing_ratio_wrt_dry_air",
    "total_water_mixing_ratio_wrt_dry_air",
    "qrain"};

  functions::eval_q_x_tl(incFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_tl()] ... exit"
    << std::endl;
}
// --------------------------------------------------------------------------------------

void eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_ad(
    atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
    << "[eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_ad()] starting ..."
    << std::endl;

  const std::vector<std::string> fnames {
    "rain_mixing_ratio_wrt_dry_air",
    "total_water_mixing_ratio_wrt_dry_air",
    "qrain"};

  functions::eval_q_x_ad(hatFlds, stateFlds, fnames);

  oops::Log::trace()
    << "[eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_ad()] ... exit"
    << std::endl;
}

// --------------------------------------------------------------------------------------

}  // namespace mo
