/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// WILL BE A REPLACEMENT FOR eval_total_mixing_ratio_nl
void eval_total_water_mixing_ratio_wrt_dry_air_nl(
  atlas::FieldSet & stateFlds);

void eval_total_water_mixing_ratio_wrt_moist_air_and_condensed_water_nl(
  atlas::FieldSet & stateFlds);
}  // namespace mo

