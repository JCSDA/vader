/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "atlas/field/FieldSet.h"

#include "mo/constants.h"
#include "mo/eval_air_density.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {

// -------------------------------------------------------------------------------------------------

/// \details Calculate the air density from virtual potential temperature
///          and air pressure (using air_pressure_levels_minus_one)
void eval_air_density_from_pressure_levels_minus_one_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_air_density_from_pressure_levels_minus_one_nl()] starting ..."
                     << std::endl;
  const auto hlView = make_view<const double, 2>(stateFlds["height_above_mean_sea_level_levels"]);
  const auto hView = make_view<const double, 2>(stateFlds["height_above_mean_sea_level"]);
  const auto vptView = make_view<const double, 2>(stateFlds["virtual_potential_temperature"]);
  const auto pView = make_view<const double, 2>(stateFlds["air_pressure_levels_minus_one"]);
  auto rhoView = make_view<double, 2>(stateFlds["air_density_levels_minus_one"]);

  const idx_t numLevels = stateFlds["air_density_levels_minus_one"].shape(1);
  const idx_t sizeOwned =
    util::getSizeOwned(stateFlds["air_density_levels_minus_one"].functionspace());
  double h_minus_hm1;
  double h_minus_hl;
  double hl_minus_hm1;

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    rhoView(jn, 0) = pView(jn, 0) *
      pow(pView(jn, 0) / constants::p_zero, - constants::rd_over_cp) /
      (constants::rd * vptView(jn, 0));

    for (idx_t jl = 1; jl < numLevels; ++jl) {
      h_minus_hm1 = hView(jn, jl) - hView(jn, jl-1);
      h_minus_hl = hView(jn, jl) - hlView(jn, jl);
      hl_minus_hm1 = hlView(jn, jl) - hView(jn, jl-1);
      rhoView(jn, jl) = pView(jn, jl) *
        pow(pView(jn, jl) / constants::p_zero, - constants::rd_over_cp)
        * h_minus_hm1 / (constants::rd * (
        h_minus_hl * vptView(jn, jl-1) + hl_minus_hm1 * vptView(jn, jl)));
    }
  }
  stateFlds["air_density_levels_minus_one"].set_dirty();

  oops::Log::trace() << "[eval_air_density_from_pressure_levels_minus_one_nl()] ... exit"
                     << std::endl;
}

}  // namespace mo
