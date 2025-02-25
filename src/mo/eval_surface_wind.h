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
/// \details This calculates the wind increment at 10 m from the atmospheric
///          wind increment at the lowest level
void eval_surface_wind_tl(atlas::FieldSet & incFlds);

/// USED IN VARTRANSFORMS
/// \details This calculates the adjoint of the wind increment at the lowest level
///          from the adjoint of the atmospheric wind at 10 m
void eval_surface_wind_ad(atlas::FieldSet & hatFlds);

}  // namespace mo
