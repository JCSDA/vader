/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// USED IN RECONFIGURESTATEGAUSS
/// \details This is the inverse of the MIO, that determines the total water control
///          variable from specific humidity, cloud ice water and cloud liquid water
///          increments
void eval_total_water_nl(atlas::FieldSet & augStateFlds);

/// USED IN RECIPE TotalWater_A.cc
/// \details This is the inverse of the MIO, that determines the total water control
///          variable from specific humidity, cloud ice water and cloud liquid water
///          increments
void eval_total_water_tl(atlas::FieldSet & incFlds,
                         const atlas::FieldSet & augStateFlds);

/// USED IN RECIPE TotalWater_A.cc
/// \details This is the adjoint of eval_total_water_tl
void eval_total_water_ad(atlas::FieldSet & hatFlds,
                         const atlas::FieldSet & augStateFlds);

}  // namespace mo
