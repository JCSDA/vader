/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

enum class DustBin : int {
  bin1 = 1,
  bin2 = 2
};

enum class DustMode {
  accumulation,
  coarse
};

void eval_dust_bin_mass_concentration_nl(
  atlas::FieldSet & stateFlds, DustBin bin);

}  // namespace mo

