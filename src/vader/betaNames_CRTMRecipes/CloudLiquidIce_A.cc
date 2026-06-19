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
#include "vader/betaNames_CRTMRecipes/CloudLiquidIce.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char CloudLiquidIce_A::Name[] =
    "CloudLiquidIce_A";
const oops::Variables CloudLiquidIce_A::Ingredients{
    std::vector<std::string>{
      "air_temperature",
      "air_pressure",
      "relative_humidity",
      "air_pressure_at_surface"}};

// Register the maker
static RecipeMaker<CloudLiquidIce_A>
  makerCloudLiquidIce_A_(
      CloudLiquidIce_A::Name);

// -------------------------------------------------------------------------------------------------

CloudLiquidIce_A::CloudLiquidIce_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) {
  oops::Log::trace() << "CloudLiquidIce_A::"
                     << "CloudLiquidIce_A" << std::endl;
}

std::string CloudLiquidIce_A::name() const {
  return CloudLiquidIce_A::Name;
}

oops::Variable CloudLiquidIce_A::product() const {
  return oops::Variable{"cloud_liquid_ice"};
}

oops::Variables CloudLiquidIce_A::ingredients() const {
  return CloudLiquidIce_A::Ingredients;
}

oops::Variables CloudLiquidIce_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{
    "air_temperature",
    "air_pressure",
    "relative_humidity",
    "air_pressure_at_surface"}};
}

size_t CloudLiquidIce_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace CloudLiquidIce_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void CloudLiquidIce_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "CloudLiquidIce_A::executeNL starting"
                     << std::endl;

  const int nlevels = afieldset.field("air_temperature").shape(1);
  const double RH_crit_base  = 0.75;
  const double RH_crit_slope = 0.10;  // slope of RH threshold vs P/Ps; empirical
  const double cli_max       = 1e-4;  // cloud liquid ice amplitude [kg kg-1]; empirical
  const double T_freeze      = 273.15;
  const double T_cold        = 233.15;

  util::for_each_column(
      [&](const auto temp_col,
          const auto pressure_col,
          const auto rh_col,
          const auto ps_col,
          auto ice_col) {
          for (int jl = 0; jl < nlevels; ++jl) {
            const double T = temp_col(jl);
            const double RH = rh_col(jl);
            const double P = pressure_col(jl);
            const double p_ratio = P / ps_col(0);
            const double RH_crit = RH_crit_base + RH_crit_slope * p_ratio;
            if (T < T_freeze && RH > RH_crit) {
              const double RH_excess = std::max(0.0, RH - RH_crit);
              const double temp_factor = std::max(0.0, (T_freeze - T) / (T_freeze - T_cold));
              ice_col(jl) = cli_max * RH_excess / (1.0 - RH_crit) * (1.0 + temp_factor);
            } else {
              ice_col(jl) = 0.0;
            }
          }
      },
      afieldset.field("air_temperature"),
      afieldset.field("air_pressure"),
      afieldset.field("relative_humidity"),
      afieldset.field("air_pressure_at_surface"),
      afieldset.field("cloud_liquid_ice"));

  oops::Log::trace() << "CloudLiquidIce_A::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void CloudLiquidIce_A::executeTL(atlas::FieldSet & afieldsetTL,
                                 const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "CloudLiquidIce_A::executeTL starting"
                     << std::endl;

  const int nlevels = afieldsetTraj.field("air_temperature").shape(1);
  const double RH_crit_base  = 0.75;
  const double RH_crit_slope = 0.10;  // slope of RH threshold vs P/Ps; empirical
  const double cli_max       = 1e-4;  // cloud liquid ice amplitude [kg kg-1]; empirical
  const double T_freeze      = 273.15;
  const double T_cold        = 233.15;

  util::for_each_column(
      [&](const auto temp_col,
          const auto pressure_col,
          const auto rh_col,
          const auto ps_col,
          const auto temp_tl_col,
          const auto pressure_tl_col,
          const auto rh_tl_col,
          const auto ps_tl_col,
          auto ice_tl_col) {
          for (int jl = 0; jl < nlevels; ++jl) {
            const double T  = temp_col(jl);
            const double RH = rh_col(jl);
            const double P  = pressure_col(jl);
            const double P_surf = ps_col(0);
            const double p_ratio  = P / P_surf;
            const double RH_crit  = RH_crit_base + RH_crit_slope * p_ratio;

            if (T < T_freeze && RH > RH_crit) {
              const double p_ratio_tl =
                  pressure_tl_col(jl)/P_surf - P/(P_surf*P_surf) * ps_tl_col(0);
              const double RH_crit_tl = RH_crit_slope * p_ratio_tl;
              const double RH_excess = RH - RH_crit;
              double RH_excess_tl = 0.0;
              if (RH_excess > 0.0) {
                RH_excess_tl = rh_tl_col(jl) - RH_crit_tl;
              }
              const double A = (T_freeze - T)/(T_freeze - T_cold);
              double temp_factor = 0.0;
              double temp_factor_tl = 0.0;
              if (A > 0.0) {
                temp_factor = A;
                temp_factor_tl = -(1.0/(T_freeze - T_cold)) * temp_tl_col(jl);
              }
              const double F = RH_excess / (1.0 - RH_crit);
              const double G = (1.0 + temp_factor);
              const double denom = (1.0 - RH_crit);
              const double F_tl =
                  (RH_excess_tl * denom + RH_excess * RH_crit_tl)
                  / (denom*denom);
              ice_tl_col(jl) = cli_max * (F_tl * G + F * temp_factor_tl);
            } else {
              ice_tl_col(jl) = 0.0;
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
      afieldsetTL.field("cloud_liquid_ice"));

  oops::Log::trace() << "CloudLiquidIce_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void CloudLiquidIce_A::executeAD(atlas::FieldSet & afieldsetAD,
                                 const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "CloudLiquidIce_A::executeAD starting"
                     << std::endl;

  const int nlevels = afieldsetTraj.field("air_temperature").shape(1);
  const double RH_crit_base  = 0.75;
  const double RH_crit_slope = 0.10;  // slope of RH threshold vs P/Ps; empirical
  const double cli_max       = 1e-4;  // cloud liquid ice amplitude [kg kg-1]; empirical
  const double T_freeze      = 273.15;
  const double T_cold        = 233.15;

  util::for_each_column(
      [&](const auto temp_col,
          const auto pressure_col,
          const auto rh_col,
          const auto ps_col,
          auto temp_ad_col,
          auto pressure_ad_col,
          auto rh_ad_col,
          auto ps_ad_col,
          auto ice_ad_col) {
          for (int jl = 0; jl < nlevels; ++jl) {
            const double T  = temp_col(jl);
            const double RH = rh_col(jl);
            const double P  = pressure_col(jl);
            const double P_surf = ps_col(0);
            const double p_ratio  = P / P_surf;
            const double RH_crit  = RH_crit_base + RH_crit_slope * p_ratio;

            if (T < T_freeze && RH > RH_crit) {
              const double RH_excess = RH - RH_crit;
              const double A = (T_freeze - T)/(T_freeze - T_cold);

              if (RH_excess > 0.0 && A > 0.0) {
                const double temp_factor = A;
                const double F = RH_excess / (1.0 - RH_crit);
                const double G = 1.0 + temp_factor;
                const double denom = 1.0 - RH_crit;

                double ice_ad_here = ice_ad_col(jl);

                double F_ad = cli_max * G * ice_ad_here;
                double temp_factor_ad = cli_max * F * ice_ad_here;

                double RH_excess_ad = F_ad / denom;
                double RH_crit_ad = F_ad * (RH_excess - denom) / (denom * denom);

                rh_ad_col(jl) += RH_excess_ad;
                pressure_ad_col(jl) += RH_crit_slope / P_surf * RH_crit_ad;
                ps_ad_col(0) += -RH_crit_slope * P/(P_surf*P_surf) * RH_crit_ad;
                temp_ad_col(jl) += -1.0/(T_freeze - T_cold) * temp_factor_ad;

                ice_ad_col(jl) = 0.0;
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
      afieldsetAD.field("cloud_liquid_ice"));

  oops::Log::trace() << "CloudLiquidIce_A::executeAD done" << std::endl;
}


// -------------------------------------------------------------------------------------------------

}  // namespace vader
