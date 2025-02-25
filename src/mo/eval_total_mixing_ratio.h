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
/// USED IN RECIPE TotalMixingRatio_A
/// USED IN SABER BLOCK GaussUVToGp
/// \brief function to evaluate the total mixing ratio:
///   m_t = M_t/M_dry
/// where ...
///   M_t  =  mass of dry air, moist air and condensed water
///   M_dry = mass of dry air
///   m_t  =  moist air and condensed water mixing ratio wrt dry air
void eval_total_mixing_ratio_nl(
  atlas::FieldSet & stateFlds);

/// USED IN RECIPE TotalMixingRatio_A.cc
void eval_total_mixing_ratio_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

/// USED IN RECIPE TotalMixingRatio_A.cc
void eval_total_mixing_ratio_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

}  // namespace mo

