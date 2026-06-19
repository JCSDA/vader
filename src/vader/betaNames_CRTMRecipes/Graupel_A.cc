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
#include "vader/betaNames_CRTMRecipes/Graupel.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char Graupel_A::Name[] =
    "Graupel_A";
const oops::Variables Graupel_A::Ingredients{
    std::vector<std::string>{
      "air_temperature", "air_pressure", "relative_humidity", "air_pressure_at_surface"}};

// Register the maker
static RecipeMaker<Graupel_A>
  makerGraupel_A_(
      Graupel_A::Name);

// -------------------------------------------------------------------------------------------------

Graupel_A::Graupel_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) {
  oops::Log::trace() << "Graupel_A::"
                     << "Graupel_A" << std::endl;
}

std::string Graupel_A::name() const {
  return Graupel_A::Name;
}

oops::Variable Graupel_A::product() const {
  return oops::Variable{"graupel"};
}

oops::Variables Graupel_A::ingredients() const {
  return Graupel_A::Ingredients;
}

oops::Variables Graupel_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{
    "air_temperature", "air_pressure", "relative_humidity", "air_pressure_at_surface"}};
}

size_t Graupel_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace Graupel_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void Graupel_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "Graupel_A::executeNL starting"
                     << std::endl;

  const int nlevels = afieldset.field("air_temperature").shape(1);
  const double RH_crit_base  = 0.88;
  const double RH_crit_slope = 0.10;  // slope of RH threshold vs P/Ps; empirical
  const double graupel_max   = 6e-5;  // peak graupel mixing ratio [kg kg-1]; empirical
  const double T_graupel_min = 258.15;
  const double T_graupel_max = 268.15;
  const double T_optimal     = 263.15;

  util::for_each_column(
      [&](const auto temp_col,
          const auto pressure_col,
          const auto rh_col,
          const auto ps_col,
          auto graupel_col) {
          for (int jl = 0; jl < nlevels; ++jl) {
            const double T = temp_col(jl);
            const double RH = rh_col(jl);
            const double P = pressure_col(jl);
            const double p_ratio = P / ps_col(0);
            const double RH_crit = RH_crit_base + RH_crit_slope * p_ratio;
            if (T < T_graupel_max && T > T_graupel_min && RH > RH_crit) {
              const double RH_excess = std::max(0.0, RH - RH_crit);
              const double temp_factor = 1.0 - std::abs(T - T_optimal) / 10.0;
              graupel_col(jl) = graupel_max * RH_excess / (1.0 - RH_crit) *
                               std::max(0.0, temp_factor);
            } else {
              graupel_col(jl) = 0.0;
            }
          }
      },
      afieldset.field("air_temperature"),
      afieldset.field("air_pressure"),
      afieldset.field("relative_humidity"),
      afieldset.field("air_pressure_at_surface"),
      afieldset.field("graupel"));

  oops::Log::trace() << "Graupel_A::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void Graupel_A::executeTL(atlas::FieldSet & afieldsetTL,
                           const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "Graupel_A::executeTL starting" << std::endl;

  const int nlevels = afieldsetTraj.field("air_temperature").shape(1);
  const double RH_crit_base  = 0.88;
  const double RH_crit_slope = 0.10;  // slope of RH threshold vs P/Ps; empirical
  const double graupel_max   = 6e-5;  // peak graupel mixing ratio [kg kg-1]; empirical
  const double T_graupel_min = 258.15;
  const double T_graupel_max = 268.15;
  const double T_optimal     = 263.15;

  util::for_each_column(
      [&](const auto temp_col,
          const auto pressure_col,
          const auto rh_col,
          const auto ps_col,
          const auto temp_tl_col,
          const auto pressure_tl_col,
          const auto rh_tl_col,
          const auto ps_tl_col,
          auto graupel_tl_col) {
          for (int jl = 0; jl < nlevels; ++jl) {
            const double T      = temp_col(jl);
            const double RH     = rh_col(jl);
            const double P      = pressure_col(jl);
            const double P_surf = ps_col(0);
            const double RH_crit   = RH_crit_base + RH_crit_slope * P / P_surf;
            const double RH_excess = RH - RH_crit;
            const double denom     = 1.0 - RH_crit;

            if (T < T_graupel_max && T > T_graupel_min && RH > RH_crit && RH_excess > 0.0) {
              const double temp_factor = 1.0 - std::abs(T - T_optimal) / 10.0;
              if (temp_factor > 0.0) {
                const double p_ratio_tl  = pressure_tl_col(jl) / P_surf
                                           - P / (P_surf * P_surf) * ps_tl_col(0);
                const double RH_crit_tl  = RH_crit_slope * p_ratio_tl;
                const double RH_excess_tl = rh_tl_col(jl) - RH_crit_tl;
                const double F           = RH_excess / denom;
                const double F_tl        = (RH_excess_tl * denom + RH_excess * RH_crit_tl)
                                           / (denom * denom);
                const double tf_tl = (T <= T_optimal ? 1.0 : -1.0) / 10.0 * temp_tl_col(jl);
                graupel_tl_col(jl) = graupel_max * (F_tl * temp_factor + F * tf_tl);
              } else {
                graupel_tl_col(jl) = 0.0;
              }
            } else {
              graupel_tl_col(jl) = 0.0;
            }
          }
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetTraj.field("air_pressure"),
      afieldsetTraj.field("relative_humidity"),
      afieldsetTraj.field("air_pressure_at_surface"),
      afieldsetTL.field("air_temperature"),
      afieldsetTL.field("air_pressure"),
      afieldsetTL.field("relative_humidity"),
      afieldsetTL.field("air_pressure_at_surface"),
      afieldsetTL.field("graupel"));

  oops::Log::trace() << "Graupel_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void Graupel_A::executeAD(atlas::FieldSet & afieldsetAD,
                           const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "Graupel_A::executeAD starting" << std::endl;

  const int nlevels = afieldsetTraj.field("air_temperature").shape(1);
  const double RH_crit_base  = 0.88;
  const double RH_crit_slope = 0.10;  // slope of RH threshold vs P/Ps; empirical
  const double graupel_max   = 6e-5;  // peak graupel mixing ratio [kg kg-1]; empirical
  const double T_graupel_min = 258.15;
  const double T_graupel_max = 268.15;
  const double T_optimal     = 263.15;

  util::for_each_column(
      [&](const auto temp_col,
          const auto pressure_col,
          const auto rh_col,
          const auto ps_col,
          auto temp_ad_col,
          auto pressure_ad_col,
          auto rh_ad_col,
          auto ps_ad_col,
          auto graupel_ad_col) {
          for (int jl = 0; jl < nlevels; ++jl) {
            const double T      = temp_col(jl);
            const double RH     = rh_col(jl);
            const double P      = pressure_col(jl);
            const double P_surf = ps_col(0);
            const double RH_crit   = RH_crit_base + RH_crit_slope * P / P_surf;
            const double RH_excess = RH - RH_crit;
            const double denom     = 1.0 - RH_crit;

            if (T < T_graupel_max && T > T_graupel_min && RH > RH_crit && RH_excess > 0.0) {
              const double temp_factor = 1.0 - std::abs(T - T_optimal) / 10.0;
              if (temp_factor > 0.0) {
                const double F = RH_excess / denom;
                const double g_ad = graupel_ad_col(jl);

                const double F_ad  = graupel_max * temp_factor * g_ad;
                const double tf_ad = graupel_max * F * g_ad;

                const double RH_excess_ad = F_ad / denom;
                const double RH_crit_ad   = F_ad * (RH_excess - denom) / (denom * denom);

                rh_ad_col(jl)  += RH_excess_ad;
                const double p_ratio_ad = RH_crit_slope * RH_crit_ad;
                pressure_ad_col(jl) += p_ratio_ad / P_surf;
                ps_ad_col(0)        += -P / (P_surf * P_surf) * p_ratio_ad;
                temp_ad_col(jl) += (T <= T_optimal ? 1.0 : -1.0) / 10.0 * tf_ad;

                graupel_ad_col(jl) = 0.0;
              }
            }
          }
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetTraj.field("air_pressure"),
      afieldsetTraj.field("relative_humidity"),
      afieldsetTraj.field("air_pressure_at_surface"),
      afieldsetAD.field("air_temperature"),
      afieldsetAD.field("air_pressure"),
      afieldsetAD.field("relative_humidity"),
      afieldsetAD.field("air_pressure_at_surface"),
      afieldsetAD.field("graupel"));

  oops::Log::trace() << "Graupel_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
