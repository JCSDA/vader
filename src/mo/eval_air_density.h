/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// USED IN RECIPE AirDensityLevelsMinusOne_A
/// USED IN RECONFIGURESTATEGAUSS
/// \details Calculate the air density
void eval_air_density_from_pressure_levels_minus_one_nl(
  atlas::FieldSet & stateFlds);

}  // namespace mo
