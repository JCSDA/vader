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
void eval_air_temperature_nl(
  atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
void eval_air_temperature_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
void eval_air_temperature_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

}  // namespace mo

