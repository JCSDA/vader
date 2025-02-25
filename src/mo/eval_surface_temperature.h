/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field.h"

namespace mo {

/// USED IN VARTRANSFORMS
/// \details This calculates the surface temperature increment from the atmospheric
///           exner and potential temperature  at the lowest level
void eval_surface_temperature_from_exner_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// \details This calculates the adjoint of the tangent linear of calculation of the
///          surface temperature from the exner and potential temperature.
///          at the lowest level from the adjoint of the surface temperature
void eval_surface_temperature_from_exner_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

}  // namespace mo
