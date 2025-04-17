/*
 * (C) Crown Copyright 2022-2025 Met Office
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

/// helper functions for water-based calculations.
void eval_q_x_nl(
  atlas::FieldSet & fields, const std::vector<std::string> & vars);

/// Helper functions for calculating specific quantities
void eval_q_x_tl(atlas::FieldSet & incFields,
  const atlas::FieldSet & fields, const std::vector<std::string> & vars);

/// Helper functions for calculating specific quantities
void eval_q_x_ad(atlas::FieldSet & hatFields,
  const atlas::FieldSet & fields, const std::vector<std::string> & vars);

/// Helper functions for calculating mixing ratio wrt dry air quantities
void eval_m_x_nl(
  atlas::FieldSet & fields, const std::vector<std::string> & vars);

/// Helper functions for calculating mixing ratio wrt dry air quantities
void eval_m_x_tl(atlas::FieldSet & incFields,
  const atlas::FieldSet & fields, const std::vector<std::string> & vars);

/// Helper functions for calculating mixing ratio wrt dry air quantities
void eval_m_x_ad(atlas::FieldSet & hatFields,
  const atlas::FieldSet & fields, const std::vector<std::string> & vars);

/// Helper functions for calculating mixing ratio wrt dry air quantities
void eval_q_x_inv_tl(atlas::FieldSet & incFields,
  const atlas::FieldSet & fields, const std::vector<std::string> & vars);

/// Helper functions for calculating mixing ratio wrt dry air quantities
void eval_q_x_inv_ad(atlas::FieldSet & hatFields,
  const atlas::FieldSet & fields, const std::vector<std::string> & vars);

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
