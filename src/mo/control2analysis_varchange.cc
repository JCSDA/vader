/*
 * (C) Crown Copyright 2022-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array.h"
#include "atlas/field.h"
#include "atlas/functionspace.h"
#include "atlas/util/Config.h"

#include "mo/constants.h"
#include "mo/control2analysis_varchange.h"
#include "mo/functions.h"

#include "oops/util/FunctionSpaceHelpers.h"

using atlas::array::make_view;
using atlas::util::Config;
using atlas::idx_t;

namespace mo {

/// \details Calculate the hydrostatic pressure (on levels)
///           from hydrostatic exner
void evalHydrostaticPressureLevels(atlas::FieldSet & stateFlds) {
  const auto hexnerView = make_view<double, 2>(stateFlds["hydrostatic_exner_levels"]);
  auto hpView = make_view<double, 2>(stateFlds["hydrostatic_pressure_levels"]);
  const idx_t numLevels = stateFlds["hydrostatic_pressure_levels"].shape(1);
  const idx_t sizeOwned =
    util::getSizeOwned(stateFlds["hydrostatic_pressure_levels"].functionspace());
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
       hpView(jn, jl) = constants::p_zero *
         pow(hexnerView(jn, jl), 1.0 / constants::rd_over_cp);
    }
  }
  stateFlds["hydrostatic_pressure_levels"].set_dirty();
}

}  // namespace mo
