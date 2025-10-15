
/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <string>
#include <vector>

#include "atlas/array.h"
#include "atlas/functionspace.h"

#include "eckit/exception/Exceptions.h"

#include "mo/constants.h"
#include "mo/functions.h"
#include "mo/model2geovals_varchange.h"

#include "oops/util/for_each.h"
#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;
using atlas::util::Config;

using View = atlas::array::LocalView<double, 1>;
using ConstView = atlas::array::LocalView<const double, 1>;

namespace mo {

void evalParamAParamB(atlas::FieldSet & stateFlds)
{
  oops::Log::trace() << "[evalParamAParamB2()] starting ..." << std::endl;

  std::size_t blindex;
  if (!stateFlds["height_above_mean_sea_level"].metadata().has("boundary_layer_index")) {
    oops::Log::error() << "ERROR - data validation failed "
                          "we expect boundary_layer_index value "
                          "in the meta data of the height field" << std::endl;
  }
  stateFlds["height_above_mean_sea_level"].metadata().get("boundary_layer_index", blindex);

  const double exp_pmsh = constants::Lclr * constants::rd / constants::grav;

  util::for_each_column(
    [=] (ConstView height,
         ConstView heightLevels,
         ConstView pressureLevels,
         ConstView specificHumidity,
         View param_a,
         View param_b) {
      // temperature at level above boundary layer
      double t_bl = (-constants::grav / constants::rd) *
             (heightLevels(blindex + 1) - heightLevels(blindex)) /
             log(pressureLevels(blindex + 1) / pressureLevels(blindex));
      t_bl = t_bl / (1.0 + constants::c_virtual * specificHumidity(blindex));

      // temperature at model surface height
      const double t_msh = t_bl + constants::Lclr * (height(blindex) - heightLevels(0));

      param_a(0) = heightLevels(0) + t_msh / constants::Lclr;
      param_b(0) = t_msh / (pow(pressureLevels(0), exp_pmsh) * constants::Lclr);
    },
    stateFlds["height_above_mean_sea_level"],
    stateFlds["height_above_mean_sea_level_levels"],
    stateFlds["air_pressure_levels_minus_one"],
    stateFlds["water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"],
    stateFlds["surf_param_a"],
    stateFlds["surf_param_b"]);

  stateFlds["surf_param_a"].set_dirty();
  stateFlds["surf_param_b"].set_dirty();

  oops::Log::trace() << "[evalParamAParamB()] ... exit" << std::endl;
}

}  // namespace mo
