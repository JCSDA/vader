/*
 * (C) Crown Copyright 2022-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <Eigen/Core>
#include <string>
#include <vector>

#include "atlas/field.h"
#include "atlas/functionspace.h"

#include "oops/base/Variables.h"
#include "oops/util/Logger.h"

namespace mo {
namespace functions {

//--
// ++ I/O processing ++

/// \details This extracts the scaling coefficients that are applied to Cleff and Cfeff
///          to generate the qcl and qcf increments in the moisture incrementing operator (MIO)
///          The string s can be "qcl_coef" or "qcf_coef"
Eigen::MatrixXd createMIOCoeff(const std::string mioFileName,
                               const std::string s);

extern "C" {
  void umGetLookUp2D_f90(
    const int &,
    const char *,
    const int &,
    const char *,
    const int &,
    const int &,
    double &);
}  // extern "C"

}  // namespace functions
}  // namespace mo
