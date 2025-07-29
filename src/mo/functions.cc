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
#include "oops/util/for_each.h"
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

  // variables ...
  //  vars[0] = mixing ratio quantity wrt dry air name
  //  vars[1] = "total water mixing ratio wrt dry air"
  //  vars[2] = specific quantity name

  util::for_each_value(
    [](const double& mx, const double& mt, double& qx) {
      qx = mx / (1.0 + mt);
    },
    fields[vars[0]], fields[vars[1]], fields[vars[2]]);

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

  // variables ...
  //  vars[0] = mixing ratio quantity wrt dry air name
  //  vars[1] = "total water mixing ratio wrt dry air"
  //  vars[2] = specific quantity name

  util::for_each_value(
    [](const double& mxInc, const double& mx, const double& mt,
       double& qxInc) {
      const double totalMixingRatio = 1.0 + mt;
      qxInc = mxInc *
        (totalMixingRatio - mx) / (totalMixingRatio * totalMixingRatio);
    },
    incFields[vars[0]], fields[vars[0]], fields[vars[1]], incFields[vars[2]]);

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

  // variables ...
  //  vars[0] = mixing ratio quantity wrt dry air name
  //  vars[1] = "total water mixing ratio wrt dry air"
  //  vars[2] = specific quantity name

  util::for_each_value(
    [](const double& mx, const double& mt,
       double& mxHat, double& qxHat) {
      const double totalMixingRatio = 1.0 + mt;
      mxHat += qxHat *
        (totalMixingRatio - mx) / (totalMixingRatio * totalMixingRatio);
      qxHat = 0.0;
    },
    fields[vars[0]], fields[vars[1]], hatFields[vars[0]], hatFields[vars[2]]);

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

  // variables ...
  //  vars[0] = "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"
  //  vars[1] = "total_water_mixing_ratio_wrt_moist_air_and_condensed_water"
  //  vars[2] = "water_vapor_mixing_ratio_wrt_dry_air"

  util::for_each_value(
    [](const double& qx, const double& qt, double& mx) {
      mx = qx / (1.0 - qt);
    },
    fields[vars[0]], fields[vars[1]], fields[vars[2]]);

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

  // variables ...
  //  vars[0] = specific quantity name
  //  vars[1] = "total_water_mixing_ratio_wrt_moist_air_and_condensed_water"
  //  vars[2] = "water_vapor_mixing_ratio_wrt_dry_air"

  util::for_each_value(
    [](const double& qxInc, const double& qx, const double& qt,
       double& mxInc) {
      const double totalMixingRatio = 1.0 - qt;
      mxInc = qxInc *
        (totalMixingRatio + qx) / (totalMixingRatio * totalMixingRatio);
    },
    incFields[vars[0]], fields[vars[0]], fields[vars[1]], incFields[vars[2]]);

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

  // variables ...
  //  vars[0] = specific quantity name
  //  vars[1] = "total_water_mixing_ratio_wrt_moist_air_and_condensed_water"
  //  vars[2] = "water_vapor_mixing_ratio_wrt_dry_air"

  util::for_each_value(
    [](const double& qx, const double& qt,
       double& qxHat, double& mxHat) {
      const double totalMixingRatio = 1.0 - qt;
      qxHat += mxHat *
        (totalMixingRatio + qx) / (totalMixingRatio * totalMixingRatio);
      mxHat = 0.0;
    },
    fields[vars[0]], fields[vars[1]], hatFields[vars[0]], hatFields[vars[2]]);

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

  // variables ...
  //  vars[0] = specific quantity name
  //  vars[1] = "total water mixing ratio wrt dry air"
  //  vars[2] = mixing ratio quantity wrt dry air name

  util::for_each_value(
    [](const double& qxInc, const double& mx, const double& mt,
       double& mxInc) {
      const double totalMixingRatio = 1.0 + mt;
      mxInc = qxInc *
        (totalMixingRatio * totalMixingRatio) / (totalMixingRatio - mx);
    },
    incFields[vars[0]], fields[vars[2]], fields[vars[1]], incFields[vars[2]]);

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

  // variables ...
  //  vars[0] = specific quantity name
  //  vars[1] = "total water mixing ratio wrt dry air"
  //  vars[2] = mixing ratio quantity wrt dry air name

  util::for_each_value(
    [](const double& mx, const double& mt,
       double& qxHat, double& mxHat) {
      const double totalMixingRatio = 1.0 + mt;
      qxHat += mxHat *
        (totalMixingRatio * totalMixingRatio) / (totalMixingRatio - mx);
      mxHat = 0.0;
    },
    fields[vars[2]], fields[vars[1]], hatFields[vars[0]], hatFields[vars[2]]);

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
