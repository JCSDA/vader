/*
 * (C) Copyright 2025 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/Hail.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char Hail_A::Name[] =
    "Hail_A";
const oops::Variables Hail_A::Ingredients{
    std::vector<std::string>{
      "air_temperature", "air_pressure", "relative_humidity", "air_pressure_at_surface"}};

// Register the maker
static RecipeMaker<Hail_A>
  makerHail_A_(
      Hail_A::Name);

// -------------------------------------------------------------------------------------------------

Hail_A::Hail_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) {
  oops::Log::trace() << "Hail_A::"
                     << "Hail_A" << std::endl;
}

std::string Hail_A::name() const {
  return Hail_A::Name;
}

oops::Variable Hail_A::product() const {
  return oops::Variable{"hail"};
}

oops::Variables Hail_A::ingredients() const {
  return Hail_A::Ingredients;
}

oops::Variables Hail_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{
    "air_temperature", "air_pressure", "relative_humidity", "air_pressure_at_surface"}};
}

size_t Hail_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace Hail_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void Hail_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "Hail_A::executeNL starting"
                     << std::endl;

  const int nlevels = afieldset.field("air_temperature").shape(1);
  const double RH_crit_base  = 0.92;
  const double RH_crit_slope = 0.10;  // slope of RH threshold vs P/Ps; empirical
  const double hail_max      = 3e-5;  // peak hail mixing ratio [kg kg-1]; empirical
  const double T_hail_min    = 253.15;
  const double T_hail_max    = 268.15;

  util::for_each_column(
      [&](const auto temp_col,
          const auto pressure_col,
          const auto rh_col,
          const auto ps_col,
          auto hail_col) {
          for (int jl = 0; jl < nlevels; ++jl) {
            const double T = temp_col(jl);
            const double RH = rh_col(jl);
            const double P = pressure_col(jl);
            const double p_ratio = P / ps_col(0);
            const double RH_crit = RH_crit_base + RH_crit_slope * p_ratio;
            // RH_crit > 1 when p_ratio > 0.8; denominator (1-RH_crit) would be negative.
            if (T < T_hail_max && T > T_hail_min && RH_crit < 1.0 && RH > RH_crit) {
              const double RH_excess = std::max(0.0, RH - RH_crit);
              hail_col(jl) = hail_max * RH_excess / (1.0 - RH_crit);
            } else {
              hail_col(jl) = 0.0;
            }
          }
      },
      afieldset.field("air_temperature"),
      afieldset.field("air_pressure"),
      afieldset.field("relative_humidity"),
      afieldset.field("air_pressure_at_surface"),
      afieldset.field("hail"));

  oops::Log::trace() << "Hail_A::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void Hail_A::executeTL(atlas::FieldSet & afieldsetTL,
                        const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "Hail_A::executeTL starting" << std::endl;

  const int nlevels = afieldsetTraj.field("air_temperature").shape(1);
  const double RH_crit_base  = 0.92;
  const double RH_crit_slope = 0.10;  // slope of RH threshold vs P/Ps; empirical
  const double hail_max      = 3e-5;  // peak hail mixing ratio [kg kg-1]; empirical
  const double T_hail_min    = 253.15;
  const double T_hail_max    = 268.15;

  util::for_each_column(
      [&](const auto temp_col,
          const auto pressure_col,
          const auto rh_col,
          const auto ps_col,
          const auto pressure_tl_col,
          const auto rh_tl_col,
          const auto ps_tl_col,
          auto hail_tl_col) {
          for (int jl = 0; jl < nlevels; ++jl) {
            const double T      = temp_col(jl);
            const double RH     = rh_col(jl);
            const double P      = pressure_col(jl);
            const double P_surf = ps_col(0);
            const double RH_crit   = RH_crit_base + RH_crit_slope * P / P_surf;
            const double RH_excess = RH - RH_crit;
            const double denom     = 1.0 - RH_crit;

            if (T < T_hail_max && T > T_hail_min && RH_crit < 1.0
                && RH > RH_crit && RH_excess > 0.0) {
              const double p_ratio_tl  = pressure_tl_col(jl) / P_surf
                                         - P / (P_surf * P_surf) * ps_tl_col(0);
              const double RH_crit_tl  = RH_crit_slope * p_ratio_tl;
              const double RH_excess_tl = rh_tl_col(jl) - RH_crit_tl;
              const double F_tl        = (RH_excess_tl * denom + RH_excess * RH_crit_tl)
                                         / (denom * denom);
              hail_tl_col(jl) = hail_max * F_tl;
            } else {
              hail_tl_col(jl) = 0.0;
            }
          }
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetTraj.field("air_pressure"),
      afieldsetTraj.field("relative_humidity"),
      afieldsetTraj.field("air_pressure_at_surface"),
      afieldsetTL.field("air_pressure"),
      afieldsetTL.field("relative_humidity"),
      afieldsetTL.field("air_pressure_at_surface"),
      afieldsetTL.field("hail"));

  oops::Log::trace() << "Hail_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void Hail_A::executeAD(atlas::FieldSet & afieldsetAD,
                        const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "Hail_A::executeAD starting" << std::endl;

  const int nlevels = afieldsetTraj.field("air_temperature").shape(1);
  const double RH_crit_base  = 0.92;
  const double RH_crit_slope = 0.10;  // slope of RH threshold vs P/Ps; empirical
  const double hail_max      = 3e-5;  // peak hail mixing ratio [kg kg-1]; empirical
  const double T_hail_min    = 253.15;
  const double T_hail_max    = 268.15;

  util::for_each_column(
      [&](const auto temp_col,
          const auto pressure_col,
          const auto rh_col,
          const auto ps_col,
          auto pressure_ad_col,
          auto rh_ad_col,
          auto ps_ad_col,
          auto hail_ad_col) {
          for (int jl = 0; jl < nlevels; ++jl) {
            const double T      = temp_col(jl);
            const double RH     = rh_col(jl);
            const double P      = pressure_col(jl);
            const double P_surf = ps_col(0);
            const double RH_crit   = RH_crit_base + RH_crit_slope * P / P_surf;
            const double RH_excess = RH - RH_crit;
            const double denom     = 1.0 - RH_crit;

            if (T < T_hail_max && T > T_hail_min && RH_crit < 1.0 && RH > RH_crit
                && RH_excess > 0.0) {
              const double F_ad        = hail_max * hail_ad_col(jl);
              const double RH_excess_ad = F_ad / denom;
              const double RH_crit_ad  = F_ad * (RH_excess - denom) / (denom * denom);

              rh_ad_col(jl)       += RH_excess_ad;
              const double p_ratio_ad = RH_crit_slope * RH_crit_ad;
              pressure_ad_col(jl) += p_ratio_ad / P_surf;
              ps_ad_col(0)        += -P / (P_surf * P_surf) * p_ratio_ad;

              hail_ad_col(jl) = 0.0;
            }
          }
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetTraj.field("air_pressure"),
      afieldsetTraj.field("relative_humidity"),
      afieldsetTraj.field("air_pressure_at_surface"),
      afieldsetAD.field("air_pressure"),
      afieldsetAD.field("relative_humidity"),
      afieldsetAD.field("air_pressure_at_surface"),
      afieldsetAD.field("hail"));

  oops::Log::trace() << "Hail_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
