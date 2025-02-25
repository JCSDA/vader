/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// USED IN VARTRANSFORMS
/// USED IN RECONFIGURESTATEGAUSS
/// USED IN RECIPE AirPressureExtendedUpByOne_A
/// \details Vertical extrapolation of air pressure above model top
void eval_air_pressure_levels_nl(atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// \details Creating air pressure levels from
///          dimensionless_exner_function_levels_minus_one
void eval_air_pressure_levels_from_exner_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// \details Adjoint of creating air pressure levels from
///          dimensionless_exner_function_levels_minus_one
void eval_air_pressure_levels_from_exner_ad(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

}  // namespace mo
