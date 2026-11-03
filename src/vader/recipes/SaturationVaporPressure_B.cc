/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/field/Field.h"
#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/SaturationVaporPressure.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Sonntag (1990) / Murphy & Koop (2005) coefficients for
// ln(es) = A/T + B + C*T + D*T² + E*T³ + F*ln(T)
// Reference: Murphy & Koop (2005), Q. J. R. Meteorol. Soc., 131, 1539-1565
namespace {
  // Liquid water (valid 123–332 K, ~0.01% accuracy)
  constexpr double A_liq = -6096.9385;
  constexpr double B_liq =  21.2409642;
  constexpr double C_liq = -2.711193e-2;
  constexpr double D_liq =  1.673952e-5;
  constexpr double E_liq =  0.0;         // no T^3 term for liquid water
  constexpr double F_liq =  2.433502;    // ln(T) coefficient

  // Ice (valid 110–273.16 K, ~0.01% accuracy)
  constexpr double A_ice = -6024.5282;
  constexpr double B_ice =  29.32707;
  constexpr double C_ice =  1.0613868e-2;
  constexpr double D_ice = -1.3198825e-5;
  constexpr double E_ice =  0.0;
  constexpr double F_ice = -0.49382577;

  constexpr double T_transition = 273.15;  // K
}  // namespace

// Static attribute initialization
const char SaturationVaporPressure_B::Name[] = "SaturationVaporPressure_B";
const oops::Variables SaturationVaporPressure_B::Ingredients{
  std::vector<std::string>{"air_temperature"}};

// Register the maker
static RecipeMaker<SaturationVaporPressure_B>
  makerSaturationVaporPressure_B_(SaturationVaporPressure_B::Name);

SaturationVaporPressure_B::SaturationVaporPressure_B(const Parameters_ & params,
  const VaderConfigVars & configVariables) :
  configVariables_{configVariables} {
  oops::Log::trace() << "SaturationVaporPressure_B::SaturationVaporPressure_B" << std::endl;
}

std::string SaturationVaporPressure_B::name() const {
  return SaturationVaporPressure_B::Name;
}

oops::Variable SaturationVaporPressure_B::product() const {
  return oops::Variable{"svp"};
}

oops::Variables SaturationVaporPressure_B::ingredients() const {
  return Ingredients;
}

oops::Variables SaturationVaporPressure_B::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"air_temperature", "svp"}};
}

size_t SaturationVaporPressure_B::productLevels(const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace SaturationVaporPressure_B::productFunctionSpace(
  const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void SaturationVaporPressure_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SaturationVaporPressure_B::executeNL Starting" << std::endl;

  util::for_each_value(
    [](const double T,
       double & es) {
       const bool liquid = (T >= T_transition);
       const double A = liquid ? A_liq : A_ice;
       const double B = liquid ? B_liq : B_ice;
       const double C = liquid ? C_liq : C_ice;
       const double D = liquid ? D_liq : D_ice;
       const double F = liquid ? F_liq : F_ice;
       const double lnes = A/T + B + C*T + D*T*T + F*std::log(T);
       es = std::max(0.0, std::exp(lnes));
    },
    afieldset["air_temperature"],
    afieldset["svp"]);

  oops::Log::trace() << "SaturationVaporPressure_B::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SaturationVaporPressure_B::executeTL(atlas::FieldSet & afieldsetTL,
                                           const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SaturationVaporPressure_B::executeTL Starting" << std::endl;

  // d(ln(es))/dT = -A/T² + C + 2*D*T + F/T
  util::for_each_value(
    [](const double T,
       const double es,
       const double dT,
       double & des) {
       const bool liquid = (T >= T_transition);
       const double A = liquid ? A_liq : A_ice;
       const double C = liquid ? C_liq : C_ice;
       const double D = liquid ? D_liq : D_ice;
       const double F = liquid ? F_liq : F_ice;
       const double dlnes_dT = -A/(T*T) + C + 2.0*D*T + F/T;
       des = es * dlnes_dT * dT;
    },
    afieldsetTraj["air_temperature"],
    afieldsetTraj["svp"],
    afieldsetTL["air_temperature"],
    afieldsetTL["svp"]);

  oops::Log::trace() << "SaturationVaporPressure_B::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SaturationVaporPressure_B::executeAD(atlas::FieldSet & afieldsetAD,
                                           const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SaturationVaporPressure_B::executeAD Starting" << std::endl;

  util::for_each_value(
    [](const double T,
       const double es,
       double & T_ad,
       double & es_ad) {
       const bool liquid = (T >= T_transition);
       const double A = liquid ? A_liq : A_ice;
       const double C = liquid ? C_liq : C_ice;
       const double D = liquid ? D_liq : D_ice;
       const double F = liquid ? F_liq : F_ice;
       const double dlnes_dT = -A/(T*T) + C + 2.0*D*T + F/T;
       T_ad += es * dlnes_dT * es_ad;
       es_ad = 0.0;
    },
    afieldsetTraj["air_temperature"],
    afieldsetTraj["svp"],
    afieldsetAD["air_temperature"],
    afieldsetAD["svp"]);

  oops::Log::trace() << "SaturationVaporPressure_B::executeAD Done" << std::endl;
}

}  // namespace vader
