/*
 * (C) Crown Copyright 2022-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <Eigen/Core>
#include <string>
#include <vector>

#include "atlas/array.h"
#include "atlas/field.h"
#include "atlas/parallel/omp/omp.h"

#include "eckit/exception/Exceptions.h"

#include "mo/constants.h"
#include "mo/functions.h"

#include "oops/base/Variables.h"
#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {
namespace functions {

void eval_q_x_nl(atlas::FieldSet & fields, const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_q_x_nl()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars = [ mixing ratio quantity wrt dry air name |
  //          "total water mixing ratio wrt dry air" |
  //          specific quantity name ]
  const auto mxView = make_view<double, 2>(fields[vars[0]]);
  const auto mtView = make_view<double, 2>(fields[vars[1]]);
  auto qxView = make_view<double, 2>(fields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(fields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    for (idx_t jl = 0; jl < qxView.shape(1); jl++) {
      qxView(jn, jl) = mxView(jn, jl) / (1.0 + mtView(jn, jl));
    }
  }
  fields[vars[2]].set_dirty();
  oops::Log::trace() << "[eval_q_x_nl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_q_x_tl(atlas::FieldSet & incFields, const atlas::FieldSet & fields,
    const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_q_x_tl()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars = [ mixing ratio quantity wrt dry air name |
  //          "total water mixing ratio wrt dry air" |
  //          specific quantity name ]
  const auto mxIncView = make_view<double, 2>(incFields[vars[0]]);
  const auto mxView = make_view<double, 2>(fields[vars[0]]);
  const auto mtView = make_view<double, 2>(fields[vars[1]]);
  auto qxIncView = make_view<double, 2>(incFields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(incFields[vars[2]].functionspace());

  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    double totalMixingRatio;
    for (idx_t jl = 0; jl < qxIncView.shape(1); jl++) {
      totalMixingRatio = 1.0 + mtView(jn, jl);
      qxIncView(jn, jl) = mxIncView(jn, jl) *
        (totalMixingRatio - mxView(jn, jl)) / (totalMixingRatio * totalMixingRatio);
    }
  }
  incFields[vars[2]].set_dirty();
  oops::Log::trace() << "[eval_q_x_tl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_q_x_ad(atlas::FieldSet & hatFields, const atlas::FieldSet & fields,
    const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_q_x_ad()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars = [ mixing ratio quantity wrt dry air name |
  //          "total water mixing ratio wrt dry air" |
  //          specific quantity name ]
  auto mxhatView = make_view<double, 2>(hatFields[vars[0]]);
  const auto mxView = make_view<double, 2>(fields[vars[0]]);
  const auto mtView = make_view<double, 2>(fields[vars[1]]);
  auto qxhatView = make_view<double, 2>(hatFields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(hatFields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    double totalMixingRatio;
    for (idx_t jl = 0; jl < mtView.shape(1); jl++) {
      totalMixingRatio = 1.0 + mtView(jn, jl);
      mxhatView(jn, jl) += qxhatView(jn, jl) *
        (totalMixingRatio - mxView(jn, jl)) / (totalMixingRatio * totalMixingRatio);
      qxhatView(jn, jl) = 0.0;
    }
  }
  hatFields[vars[0]].set_dirty();
  hatFields[vars[2]].set_dirty();

  oops::Log::trace() << "[eval_q_x_ad()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_m_x_nl(atlas::FieldSet & fields, const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_m_x_nl()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars = [ "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water",
  //          "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
  //          "water_vapor_mixing_ratio_wrt_dry_air" ]
  const auto qxView = make_view<double, 2>(fields[vars[0]]);
  const auto qtView = make_view<double, 2>(fields[vars[1]]);
  auto mxView = make_view<double, 2>(fields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(fields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    for (idx_t jl = 0; jl < qxView.shape(1); jl++) {
      mxView(jn, jl) = qxView(jn, jl) / (1.0 - qtView(jn, jl));
    }
  }
  fields[vars[2]].set_dirty();
  oops::Log::trace() << "[eval_m_x_nl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_m_x_tl(atlas::FieldSet & incFields, const atlas::FieldSet & fields,
    const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_m_x_tl()] vars starting ..." << vars << std::endl;

  oops::Log::trace() << "[eval_m_x_tl()] incFields ..." << incFields.field_names() << std::endl;

  oops::Log::trace() << "[eval_m_x_tl()] fields ..." << fields.field_names() << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars = [ specific quantity name,
  //          "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
  //          "water_vapor_mixing_ratio_wrt_dry_air" ]
  const auto qxIncView = make_view<double, 2>(incFields[vars[0]]);
  const auto qxView = make_view<double, 2>(fields[vars[0]]);
  const auto qtView = make_view<double, 2>(fields[vars[1]]);
  auto mxIncView = make_view<double, 2>(incFields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(incFields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    double totalMixingRatio;
    for (idx_t jl = 0; jl < qtView.shape(1); jl++) {
      totalMixingRatio = 1.0 - qtView(jn, jl);
      mxIncView(jn, jl) = qxIncView(jn, jl) *
        (totalMixingRatio + qxView(jn, jl)) / (totalMixingRatio * totalMixingRatio);
    }
  }
  incFields[vars[2]].set_dirty();
  oops::Log::trace() << "[eval_m_x_tl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_m_x_ad(atlas::FieldSet & hatFields, const atlas::FieldSet & fields,
    const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_m_x_ad()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars = [ specific quantity name,
  //          "total_water_mixing_ratio_wrt_moist_air_and_condensed_water",
  //          "water_vapor_mixing_ratio_wrt_dry_air" ]
  auto qxHatView = make_view<double, 2>(hatFields[vars[0]]);
  const auto qxView = make_view<double, 2>(fields[vars[0]]);
  const auto qtView = make_view<double, 2>(fields[vars[1]]);
  auto mxHatView = make_view<double, 2>(hatFields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(hatFields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    double totalMixingRatio;
    for (idx_t jl = 0; jl < qtView.shape(1); jl++) {
      totalMixingRatio = 1.0 - qtView(jn, jl);
      qxHatView(jn, jl) += mxHatView(jn, jl) *
        (totalMixingRatio + qxView(jn, jl)) / (totalMixingRatio * totalMixingRatio);
      mxHatView(jn, jl) = 0.0;
    }
  }
  hatFields[vars[0]].set_dirty();
  hatFields[vars[2]].set_dirty();
  oops::Log::trace() << "[eval_m_x_ad()] ... exit" << std::endl;
}


// --------------------------------------------------------------------------------------

void eval_q_x_inv_tl(atlas::FieldSet & incFields, const atlas::FieldSet & fields,
    const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_q_x_inv_tl()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars = [ specific quantity name |
  //          "total water mixing ratio wrt dry air" |
  //          mixing ratio quantity wrt dry air name ]


  const auto qxIncView = make_view<double, 2>(incFields[vars[0]]);
  const auto mxView = make_view<double, 2>(fields[vars[2]]);
  const auto mtView = make_view<double, 2>(fields[vars[1]]);
  auto mxIncView = make_view<double, 2>(incFields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(incFields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    double totalMixingRatio;
    for (idx_t jl = 0; jl < mtView.shape(1); jl++) {
      totalMixingRatio = 1.0 + mtView(jn, jl);
      mxIncView(jn, jl) = qxIncView(jn, jl) *
        (totalMixingRatio * totalMixingRatio) / (totalMixingRatio - mxView(jn, jl));
    }
  }
  incFields[vars[2]].set_dirty();
  oops::Log::trace() << "[eval_q_x_inv_tl()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

void eval_q_x_inv_ad(atlas::FieldSet & hatFields, const atlas::FieldSet & fields,
    const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[eval_q_x_inv_ad()] starting ..." << std::endl;

  if (vars.size() != 3) {
    throw eckit::Exception("Expected 3 variable names in vars", Here());
  }

  // vars = [ specific quantity name |
  //          "total water mixing ratio wrt dry air" |
  //          mixing ratio quantity wrt dry air name
  auto qxHatView = make_view<double, 2>(hatFields[vars[0]]);
  const auto mxView = make_view<double, 2>(fields[vars[2]]);
  const auto mtView = make_view<double, 2>(fields[vars[1]]);
  auto mxHatView = make_view<double, 2>(hatFields[vars[2]]);

  const idx_t sizeOwned = util::getSizeOwned(hatFields[vars[2]].functionspace());
  atlas_omp_parallel_for(idx_t jn = 0; jn < sizeOwned; jn++) {
    double totalMixingRatio;
    for (idx_t jl = 0; jl < mtView.shape(1); jl++) {
      totalMixingRatio = 1.0 + mtView(jn, jl);
      qxHatView(jn, jl) += mxHatView(jn, jl) *
        (totalMixingRatio * totalMixingRatio) / (totalMixingRatio - mxView(jn, jl));
      mxHatView(jn, jl) = 0.0;
    }
  }
  hatFields[vars[0]].set_dirty();
  hatFields[vars[2]].set_dirty();

  oops::Log::trace() << "[eval_q_x_inv_ad()] ... exit" << std::endl;
}

// --------------------------------------------------------------------------------------

Eigen::MatrixXd createMIOCoeff(const std::string mioFileName,
                               const std::string s) {
  oops::Log::trace() << "[createMIOCoeff] starting ..." << std::endl;
  Eigen::MatrixXd mioCoeff(static_cast<std::size_t>(constants::mioLevs),
                           static_cast<std::size_t>(constants::mioBins));

  std::vector<double> valuesvec(constants::mioLookUpLength, 0);

  umGetLookUp2D_f90(static_cast<int>(mioFileName.size()),
                    mioFileName.c_str(),
                    static_cast<int>(s.size()),
                    s.c_str(),
                    static_cast<int>(constants::mioBins),
                    static_cast<int>(constants::mioLevs),
                    valuesvec[0]);

  for (int j = 0; j < static_cast<int>(constants::mioLevs); ++j) {
    for (int i = 0; i < static_cast<int>(constants::mioBins); ++i) {
      // Fortran returns column major order, but C++ needs row major
      mioCoeff(j, i) = valuesvec[i * constants::mioLevs+j];
    }
  }
  oops::Log::trace() << "[createMIOCoeff] ... exit" << std::endl;
  return mioCoeff;
}

}  // namespace functions
}  // namespace mo
