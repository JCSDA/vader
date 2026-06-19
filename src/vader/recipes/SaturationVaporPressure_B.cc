/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "oops/util/Logger.h"
#include "vader/recipes/SaturationVaporPressure.h"

namespace vader {

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

  // Get fields
  auto temperatureView = atlas::array::make_view<const double, 2>(
    afieldset.field("air_temperature"));
  auto svpView = atlas::array::make_view<double, 2>(
    afieldset.field("svp"));

  // Murphy & Koop (2005) coefficients for saturation vapor pressure
  // Reference: Murphy & Koop (2005), Q. J. R. Meteorol. Soc., 131, 1539-1565
  // Formula: ln(es) = A/T + B + C*T + D*T² + E*T³ + F*ln(T)

  // Liquid water coefficients (valid 123K to 332K, accuracy ~0.01%)
  const double A_liq = -6096.9385;
  const double B_liq = 21.2409642;
  const double C_liq = -2.711193e-2;
  const double D_liq = 1.673952e-5;
  const double E_liq = 2.433502e-8;
  const double F_liq = 0.0;

  // Ice coefficients (valid 110K to 273.16K, accuracy ~0.01%)
  const double A_ice = -6024.5282;
  const double B_ice = 29.32707;
  const double C_ice = 1.0613868e-2;
  const double D_ice = -1.3198825e-5;
  const double E_ice = 0.0;
  const double F_ice = -0.49382577;

  // Transition temperature (273.15K = 0°C)
  const double T_transition = 273.15;

  const size_t nx = temperatureView.shape(0);
  const size_t nz = temperatureView.shape(1);

  for (size_t jn = 0; jn < nx; ++jn) {
    for (size_t jl = 0; jl < nz; ++jl) {
      const double T = temperatureView(jn, jl);  // Kelvin

      // Select coefficients based on temperature
      double A, B, C, D, E, F;
      if (T >= T_transition) {
        // Use liquid water formulation above freezing
        A = A_liq; B = B_liq; C = C_liq; D = D_liq; E = E_liq; F = F_liq;
      } else {
        // Use ice formulation below freezing
        A = A_ice; B = B_ice; C = C_ice; D = D_ice; E = E_ice; F = F_ice;
      }

      // Murphy & Koop (2005) formula
      const double lnT = (F != 0.0) ? std::log(T) : 0.0;
      const double lnes = A/T + B + C*T + D*T*T + E*T*T*T + F*lnT;
      const double es = std::exp(lnes);  // Pa

      // Ensure non-negative
      svpView(jn, jl) = std::max(0.0, es);
    }
  }

  oops::Log::trace() << "SaturationVaporPressure_B::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SaturationVaporPressure_B::executeTL(atlas::FieldSet & afieldsetTL,
                                           const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SaturationVaporPressure_B::executeTL Starting" << std::endl;

  // Get trajectory fields
  auto temperatureView = atlas::array::make_view<const double, 2>(
    afieldsetTraj.field("air_temperature"));
  auto svpView = atlas::array::make_view<const double, 2>(
    afieldsetTraj.field("svp"));

  // Get TL fields
  auto temperatureTLView = atlas::array::make_view<const double, 2>(
    afieldsetTL.field("air_temperature"));
  auto svpTLView = atlas::array::make_view<double, 2>(
    afieldsetTL.field("svp"));

  // Murphy & Koop (2005) coefficients
  // Liquid water coefficients
  const double A_liq = -6096.9385;
  const double C_liq = -2.711193e-2;
  const double D_liq = 1.673952e-5;
  const double E_liq = 2.433502e-8;
  const double F_liq = 0.0;

  // Ice coefficients
  const double A_ice = -6024.5282;
  const double C_ice = 1.0613868e-2;
  const double D_ice = -1.3198825e-5;
  const double E_ice = 0.0;
  const double F_ice = -0.49382577;

  // Transition temperature
  const double T_transition = 273.15;

  const size_t nx = temperatureView.shape(0);
  const size_t nz = temperatureView.shape(1);

  for (size_t jn = 0; jn < nx; ++jn) {
    for (size_t jl = 0; jl < nz; ++jl) {
      const double T = temperatureView(jn, jl);
      const double es = svpView(jn, jl);
      const double dT = temperatureTLView(jn, jl);

      // Select coefficients based on temperature
      double A, C, D, E, F;
      if (T >= T_transition) {
        A = A_liq; C = C_liq; D = D_liq; E = E_liq; F = F_liq;
      } else {
        A = A_ice; C = C_ice; D = D_ice; E = E_ice; F = F_ice;
      }

      // Derivative: d(es)/dT = es * d(ln(es))/dT
      // d(ln(es))/dT = -A/T² + C + 2*D*T + 3*E*T² + F/T
      const double dlnes_dT = -A/(T*T) + C + 2.0*D*T + 3.0*E*T*T + F/T;
      const double des_dT = es * dlnes_dT;

      svpTLView(jn, jl) = des_dT * dT;
    }
  }

  oops::Log::trace() << "SaturationVaporPressure_B::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SaturationVaporPressure_B::executeAD(atlas::FieldSet & afieldsetAD,
                                           const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SaturationVaporPressure_B::executeAD Starting" << std::endl;

  // Get trajectory fields
  auto temperatureView = atlas::array::make_view<const double, 2>(
    afieldsetTraj.field("air_temperature"));
  auto svpView = atlas::array::make_view<const double, 2>(
    afieldsetTraj.field("svp"));

  // Get AD fields
  auto temperatureADView = atlas::array::make_view<double, 2>(
    afieldsetAD.field("air_temperature"));
  auto svpADView = atlas::array::make_view<double, 2>(
    afieldsetAD.field("svp"));

  // Murphy & Koop (2005) coefficients
  // Liquid water coefficients
  const double A_liq = -6096.9385;
  const double C_liq = -2.711193e-2;
  const double D_liq = 1.673952e-5;
  const double E_liq = 2.433502e-8;
  const double F_liq = 0.0;

  // Ice coefficients
  const double A_ice = -6024.5282;
  const double C_ice = 1.0613868e-2;
  const double D_ice = -1.3198825e-5;
  const double E_ice = 0.0;
  const double F_ice = -0.49382577;

  // Transition temperature
  const double T_transition = 273.15;

  const size_t nx = temperatureView.shape(0);
  const size_t nz = temperatureView.shape(1);

  for (size_t jn = 0; jn < nx; ++jn) {
    for (size_t jl = 0; jl < nz; ++jl) {
      const double T = temperatureView(jn, jl);
      const double es = svpView(jn, jl);
      const double es_ad = svpADView(jn, jl);

      // Select coefficients based on temperature
      double A, C, D, E, F;
      if (T >= T_transition) {
        A = A_liq; C = C_liq; D = D_liq; E = E_liq; F = F_liq;
      } else {
        A = A_ice; C = C_ice; D = D_ice; E = E_ice; F = F_ice;
      }

      // Adjoint of: des = des_dT * dT
      const double dlnes_dT = -A/(T*T) + C + 2.0*D*T + 3.0*E*T*T + F/T;
      const double des_dT = es * dlnes_dT;

      temperatureADView(jn, jl) += des_dT * es_ad;

      // Zero out svp_ad after processing
      svpADView(jn, jl) = 0.0;
    }
  }

  oops::Log::trace() << "SaturationVaporPressure_B::executeAD Done" << std::endl;
}

}  // namespace vader
