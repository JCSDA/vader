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
/// USED IN RECIPE WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A
/// USED IN SABER BLOCK GaussUVToGP
/// \brief function to evaluate the water vapor mixing ratio wrt
///        moist air and condensed water (one possible definition
///        of specific humidity):
///   q = water_vapor_mixing_ratio_wrt_dry_air/total_water_mixing_ratio_wrt_dry_air
void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl(
  atlas::FieldSet & stateFlds);

/// USED IN RECIPE WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A
void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

/// USED IN RECIPE WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A
void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// USED IN RECONFIGURESTATEGAUSS
/// USED IN RECIPE WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A
/// \brief function to evaluate the water vapor mixing ratio wrt moist air
///        and condensed water at 2m:
///   q2m = rh * qsat
/// where ...
///   q2m  = water vapor mixing ratio wrt moist air and condensed water at 2m above surface
///   rh   = relative humidity at 2m above surface
///   qsat = water vapor mixing ratio wrt moist air and condensed water assuming saturation
void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_nl(
  atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// USED IN RECIPE WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A
void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_tl(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// USED IN RECIPE WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A
void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);
}  // namespace mo
