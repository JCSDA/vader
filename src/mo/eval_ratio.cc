/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "atlas/array/MakeView.h"
#include "atlas/field.h"
#include "atlas/parallel/omp/omp.h"
#include "atlas/util/Config.h"

#include "eckit/exception/Exceptions.h"

#include "mo/eval_ratio.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;
using atlas::util::Config;

namespace mo {

void eval_ratio_to_second(atlas::FieldSet & fields,
                          const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_ratio_to_second()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars[0] = m_x = [ water_vapor_mixing_ratio_wrt_dry_air        |
  //                   cloud_ice_mixing_ratio_wrt_dry_air          |
  //                   cloud_liquid_water_mixing_ratio_wrt_dry_air |
  //                   rain_mixing_ratio_wrt_dry_air ]
  const auto ds_m_x  = make_view<const double, 2>(fields[vars[0]]);
  const auto ds_m_t  = make_view<const double, 2>(fields[vars[1]]);
  auto ds_tfield  = make_view<double, 2>(fields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(fields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    for (idx_t jl = 0; jl < ds_tfield.shape(1); jl++) {
      ds_tfield(jn, jl) = ds_m_x(jn, jl) / ds_m_t(jn, jl);
    }
  }
  fields[vars[2]].set_dirty();
  oops::Log::trace() << "[eval_ratio_to_second()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_ratio_to_second_tl(atlas::FieldSet & incFields,
                             const atlas::FieldSet & fields,
                             const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_ratio_to_second_tl()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars[0] = m_x = [ water_vapor_mixing_ratio_wrt_dry_air        |
  //                   cloud_ice_mixing_ratio_wrt_dry_air          |
  //                   cloud_liquid_water_mixing_ratio_wrt_dry_air |
  //                   rain_mixing_ratio_wrt_dry_air ]
  const auto inc_m_x  = make_view<const double, 2>(incFields[vars[0]]);
  const auto inc_m_t  = make_view<const double, 2>(incFields[vars[1]]);
  const auto m_x  = make_view<const double, 2>(fields[vars[0]]);
  const auto m_t  = make_view<const double, 2>(fields[vars[1]]);
  auto inc_ratio  = make_view<double, 2>(incFields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(incFields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    for (idx_t jl = 0; jl < inc_ratio.shape(1); jl++) {
      inc_ratio(jn, jl) = inc_m_x(jn, jl) / m_t(jn, jl)
                 - m_x(jn, jl) / (m_t(jn, jl) * m_t(jn, jl)) * inc_m_t(jn, jl);
    }
  }
  incFields[vars[2]].set_dirty();
  oops::Log::trace() << "[eval_ratio_to_second_tl()] ... exit" << std::endl;
}

// ----------------------------------------------------------------------------

void eval_ratio_to_second_ad(atlas::FieldSet & hatFields,
                             const atlas::FieldSet & fields,
                             const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_ratio_to_second_ad()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars[0] = m_x = [ water_vapor_mixing_ratio_wrt_dry_air        |
  //                   cloud_ice_mixing_ratio_wrt_dry_air          |
  //                   cloud_liquid_water_mixing_ratio_wrt_dry_air |
  //                   rain_mixing_ratio_wrt_dry_air ]
  auto hat_m_x  = make_view<double, 2>(hatFields[vars[0]]);
  auto hat_m_t  = make_view<double, 2>(hatFields[vars[1]]);
  const auto m_x  = make_view<const double, 2>(fields[vars[0]]);
  const auto m_t  = make_view<const double, 2>(fields[vars[1]]);
  auto hat_ratio  = make_view<double, 2>(hatFields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(hatFields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    for (idx_t jl = 0; jl < hat_ratio.shape(1); jl++) {
      hat_m_x(jn, jl) += hat_ratio(jn, jl) / m_t(jn, jl);
      hat_m_t(jn, jl) -= hat_ratio(jn, jl) * m_x(jn, jl)
                                / (m_t(jn, jl) * m_t(jn, jl));
      hat_ratio(jn, jl) = 0.0;
    }
  }
  hatFields[vars[0]].set_dirty();
  hatFields[vars[1]].set_dirty();
  hatFields[vars[2]].set_dirty();

  oops::Log::trace() << "[eval_ratio_to_second_ad()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

}  // namespace mo
