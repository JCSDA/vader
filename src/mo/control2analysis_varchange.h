/*
 * (C) Crown Copyright 2022-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// \details Calculate the hydrostatic pressure (on levels)
///          from hydrostatic exner.
void evalHydrostaticPressureLevels(atlas::FieldSet & stateFlds);

}  // namespace mo
