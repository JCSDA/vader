/*
 * (C) Crown Copyright 2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <string>

#include "atlas/array/MakeView.h"
#include "atlas/field.h"
#include "atlas/field/FieldSet.h"
#include "atlas/functionspace.h"

#include "mo/constants.h"
#include "mo/eval_dust_2bin_mass_concentration.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {
/// \brief Evaluates CLASSIC dust mass fractions in two bins from UKCA model
//  bi-modal dust mass and number fractions
/// \param[in,out] stateFlds FieldSet containing the input and output fields.
/// \note The input fields are:
///   - `mass_fraction_of_dust_coarse_aerosol_particles_in_air`
///   - `mass_fraction_of_dust_accumulation_aerosol_particles_in_air`
///   - `number_fraction_of_coarse_aerosol_particles_in_air`
///   - `number_fraction_of_accumulation_aerosol_particles_in_air`
/// \note The output fields are:
///   - `mass_fraction_of_dust001_in_air`
///   - `mass_fraction_of_dust002_in_air`
/// \details Calculates 2-bin dust mass fractions from bi-modal
/// dust mass and number fractions
double getDiameter(double num, double mass, DustMode mode) {
  // function to calculate median diameter from number (n) and mass (m) fractions
  // median_diameter = alpha * (m/n)^(1/3) where alpha is a function of the modal width

  const double alpha_a = std::pow((6.0 *mo::constants::k_B) / (M_PI * mo::constants::rd *
                         mo::constants::glomap_dust_density), 1.0 / 3.0)*
                         std::exp(1.5*std::pow(std::log(mo::constants::sigma_acc), 2.0));
  const double alpha_c = std::pow((6.0 *mo::constants::k_B) / (M_PI * mo::constants::rd *
                         mo::constants::glomap_dust_density), 1.0 / 3.0)*
                         std::exp(1.5*std::pow(std::log(mo::constants::sigma_coarse), 2.0));
  const double alpha = (mode == DustMode::accumulation) ? alpha_a : alpha_c;

  // set diameter to zero if either number or mass are zero, else calculate diameter
  double diameter = 0.0;
  if (num > 0.0 && mass > 0.0) {
    diameter = alpha*std::pow((mass / num), 1.0 / 3.0);
  }

  return diameter;
}

double getFraction(double diameter, DustMode mode, DustBin bin) {
  // function to calculate fraction of a given mode in a given bin, using the error function

  const double Dmin_bin = (bin == DustBin::bin1) ? mo::constants::Dmin_bin1 :
                          mo::constants::Dmin_bin2;
  const double Dmax_bin = (bin == DustBin::bin1) ? mo::constants::Dmax_bin1 :
                          mo::constants::Dmax_bin2;
  const double sigma = (mode == DustMode::accumulation) ? mo::constants::sigma_acc :
                       mo::constants::sigma_coarse;

  double frac = std::erf((std::log(Dmax_bin / diameter)) /
                         (std::sqrt(2) * std::log(sigma))) -
                std::erf((std::log(Dmin_bin / diameter)) /
                         (std::sqrt(2) * std::log(sigma)));
  return frac;
}

void eval_dust_bin_mass_concentration_nl(atlas::FieldSet & stateFlds,
              DustBin bin) {
  oops::Log::trace() << "[eval_dust_bin_mass_concentration_nl()] starting for bin "
                     << static_cast<int>(bin) << " dust..." << std::endl;

  // input fields
  const auto mcorView = make_view<const double, 2>(
                      stateFlds["mass_fraction_of_dust_coarse_aerosol_particles_in_air"]);
  const auto maccView = make_view<const double, 2>(
                      stateFlds["mass_fraction_of_dust_accumulation_aerosol_particles_in_air"]);
  const auto ncorView = make_view<const double, 2>(
                      stateFlds["number_fraction_of_coarse_aerosol_particles_in_air"]);
  const auto naccView = make_view<const double, 2>(
                      stateFlds["number_fraction_of_accumulation_aerosol_particles_in_air"]);

  // variable name for the given dust bin
  const std::string dust_name = "mass_fraction_of_dust00" + std::to_string(static_cast<int>(bin))
                                + "_in_air";

  // output fields
  auto mbinView = make_view<double, 2>(stateFlds[dust_name]);

  // Indexing
  auto fspace = stateFlds["mass_fraction_of_dust_coarse_aerosol_"
                          "particles_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = stateFlds["mass_fraction_of_dust"
                                    "_coarse_aerosol_particles_in_air"].shape(1);

  // Dust variable transformation, looping over all grid points and levels
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      double acc_diameter = getDiameter(naccView(jn, jl), maccView(jn, jl), DustMode::accumulation);
      double cor_diameter = getDiameter(ncorView(jn, jl), mcorView(jn, jl), DustMode::coarse);
      double f_acc = 0.0;
      if (acc_diameter > 0.0) {
        f_acc = getFraction(acc_diameter, DustMode::accumulation, bin);
      }
      double f_cor = 0.0;
      if (cor_diameter > 0.0) {
        f_cor = getFraction(cor_diameter, DustMode::coarse, bin);
      }
      mbinView(jn, jl) = 0.5 * f_acc*maccView(jn, jl) + 0.5 * f_cor*mcorView(jn, jl);
    }
  }

  stateFlds[dust_name].set_dirty();
  oops::Log::trace() << "[eval_dust_bin_mass_concentration_nl()] ... exit" << std::endl;
}


}  // namespace mo
