/*
 * (C) Crown Copyright 2022-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>
#include <vector>

#include "atlas/field/Field.h"
#include "atlas/field/FieldSet.h"

namespace mo {

/// USED IN RECONFIGURESTATEGAUSS
/// USED IN RECIPE HydrostaticPressureLevels_A
/// \brief function to evaluate saturation specific humidity (qsat)
/// Needs air pressure [Pa] and svp [Pa] Atlas fields and returns the qsat Atlas field
void evalSatSpecificHumidity(atlas::FieldSet & stateFlds);

}  // namespace mo
