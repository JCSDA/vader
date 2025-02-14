/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {
/// \details Calculate the hydrostatic pressure (on levels)
///          from hydrostatic exner.
void eval_air_pressure_nl(atlas::FieldSet & stateFlds);

void eval_air_pressure_tl(atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

void eval_air_pressure_ad(atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

void eval_air_pressure_from_exner_tl(atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

void eval_air_pressure_from_exner_ad(atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

}  // namespace mo
