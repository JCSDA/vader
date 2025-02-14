/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field.h"

namespace mo {

/// \details This calculates the surface pressure increment from the atmospheric
///          dimensionless exner pressure increment at the lowest level
void eval_surface_pressure_from_exner_tl(atlas::FieldSet & incFlds,
                                         const atlas::FieldSet & stateFlds);

/// \details This calculates the adjoint of the calculation of the surface pressure
///          from the dimensionless exner pressure
void eval_surface_pressure_from_exner_ad(atlas::FieldSet & HatFlds,
                                         const atlas::FieldSet & stateFlds);

/// \details This calculates the surface pressure increment from the atmospheric
///          pressure increment at the lowest level (TO BE RETIRED)
void eval_surface_pressure_tl(atlas::FieldSet & incFlds);

/// \details This calculates the adjoint of the atmospheric pressure
///          at the lowest level from the adjoint of the surface pressure
///          (TO BE RETIRED)
void eval_surface_pressure_ad(atlas::FieldSet & HatFlds);

}  // namespace mo
