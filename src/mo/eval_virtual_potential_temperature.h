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
/// USED IN RECONFIGURESTATEGAUSS
/// USED IN RECIPE VirtualPotentialTemperature_B
/// \details Calculate the virtual potential temperature
///          from the specific humidity and the potential temperature.
void eval_virtual_potential_temperature_nl(
  atlas::FieldSet & fields);

/// USED IN RECIPE VirtualPotentialTemperature_B
/// \details Calculate the virtual potential temperature
///          from the specific humidity and the potential temperature.
void eval_virtual_potential_temperature_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds);

/// USED IN RECIPE VirtualPotentialTemperature_B
/// \details This calculates the adjoint of the transform to air temperature increments.
void eval_virtual_potential_temperature_ad(
  atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds);

}  // namespace mo
