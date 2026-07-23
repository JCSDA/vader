/*
* (C) Crown Copyright 2025 Met Office
*
* This software is licensed under the terms of the Apache Licence Version 2.0
* which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
*/


#include <cmath>
#include <vector>

#include "atlas/array/MakeView.h"
#include "atlas/field.h"
#include "atlas/field/FieldSet.h"
#include "atlas/functionspace.h"

#include "mo/constants.h"
#include "mo/eval_dust_2mode_variables.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {

/// \brief Evaluates aerosol mass fractions and number fractions for the two dust
/// modes in GLOMAP/UKCA (LFRic) from the 2-bin CLASSIC dust mass fractions (UM).
/// \param[in,out] stateFlds FieldSet containing the input and output fields.
/// \note The input fields are:
///   - `mass_fraction_of_dust001_in_air`
///   - `mass_fraction_of_dust002_in_air`
/// \note The output fields are:
///   - `mass_fraction_of_dust_coarse_aerosol_particles_in_air`
///   - `mass_fraction_of_dust_accumulation_aerosol_particles_in_air`
///   - `number_fraction_of_coarse_aerosol_particles_in_air`
///   - `number_fraction_of_accumulation_aerosol_particles_in_air`
/// \details Derives bi-modal mass and number fractions for the UKCA/GLOMAP
//  dust model, used in LFRic, from the 2-bin CLASSIC dust mass fractions used in
//  the UM. Firstly the 2-bin dust mass fractions are transformed into the 6-bin
//  mass fractions for the 6-bin CLASSIC model (used for climate). Then these latter
//  variables are transformed into the mass and number fractions of the accumulation
//  and coarse dust modes in UKCA/GLOMAP. The fraction of each 2-bin dust mass fraction
//  that contributes to the mass fraction of each dust mode is determined from the p1
//  and p2 arrays in mo/constants.h. A scaling factor for these arrays is also applied,
//  p1_scale and p2_scale as defined in mo/constants.h. If these scalings are set to 1.0,
//  then the total mass is conserved in these transforms, otherwise it is not. Modal
//  number fractions are derived from the mass fractions using the representative particle
//  diameters for 6 CLASSIC dust bins, drep6c,defined in mo/constants.h.

void eval_dust_accumulation_mode_mass_fraction_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_accumulation_mode_mass_fraction_nl()] starting ..."
                     << std::endl;

  // Input fields
  const auto mbin1View = make_view<const double, 2>(
                      stateFlds["mass_fraction_of_dust001_in_air"]);

  // Output fields
  auto maccView = make_view<double, 2>(
                      stateFlds["mass_fraction_of_dust_accumulation_aerosol_particles_in_air"]);

  // Indexing
  auto fspace = stateFlds["mass_fraction_of_dust001_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = stateFlds["mass_fraction_of_dust001_in_air"].shape(1);

  // Dust variable transformation
  double mass_bin6_2;
  double mass_bin6_3;

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      // Transform from 2 to 6 bin mass fractions
      mass_bin6_2 = mbin1View(jn, jl)*mo::constants::p1[1]*mo::constants::p1_scale;
      mass_bin6_3 = mbin1View(jn, jl)*mo::constants::p1[2]*mo::constants::p1_scale;
      // Transform 6 bin mass fractions to accumulation mode mass fraction
      maccView(jn, jl) = mass_bin6_2 + mass_bin6_3 * 0.5;
    }
  }

  stateFlds["mass_fraction_of_dust_accumulation_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_accumulation_mode_mass_fraction_nl()] ... exit" << std::endl;
}

void eval_dust_coarse_mode_mass_fraction_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_coarse_mode_mass_fraction_nl()] starting ..." << std::endl;

  // Input fields
  const auto mbin1View = make_view<const double, 2>(
                      stateFlds["mass_fraction_of_dust001_in_air"]);
  const auto mbin2View = make_view<const double, 2>(
                      stateFlds["mass_fraction_of_dust002_in_air"]);

  // Output fields
  auto mcorView = make_view<double, 2>(
                      stateFlds["mass_fraction_of_dust_coarse_aerosol_particles_in_air"]);

  // Indexing
  auto fspace = stateFlds["mass_fraction_of_dust001_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = stateFlds["mass_fraction_of_dust001_in_air"].shape(1);

  // Transform variables
  double mass_bin6_3;
  double mass_bin6_4;
  double mass_bin6_5;

  // Dust variable transformation
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      // Transform from 2 to 6 bin mass fractions
      mass_bin6_3 = mbin1View(jn, jl)*mo::constants::p1[2]*mo::constants::p1_scale;
      mass_bin6_4 = mbin1View(jn, jl)*mo::constants::p1[3]*mo::constants::p1_scale +
                    mbin2View(jn, jl)*mo::constants::p2[3]*mo::constants::p2_scale;
      mass_bin6_5 = mbin2View(jn, jl)*mo::constants::p2[4]*mo::constants::p2_scale;
      // Transform 6 bin mass fractions to coarse mode mass fraction
      mcorView(jn, jl) = mass_bin6_3 * 0.5 + mass_bin6_4 + mass_bin6_5;
    }
  }

  stateFlds["mass_fraction_of_dust_coarse_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_coarse_mode_mass_fraction_nl()] ... exit" << std::endl;
}

void eval_dust_accumulation_mode_number_fraction_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_accumulation_mode_number_fraction_nl()] starting ..."
                     << std::endl;

  // Input fields
  const auto mbin1View = make_view<const double, 2>(
                      stateFlds["mass_fraction_of_dust001_in_air"]);

  // Output fields
  auto naccView = make_view<double, 2>(
                      stateFlds["number_fraction_of_accumulation_aerosol_particles_in_air"]);

  // Indexing
  auto fspace = stateFlds["mass_fraction_of_dust001_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = stateFlds["mass_fraction_of_dust001_in_air"].shape(1);

  // Transform variables
  double mass_bin6_2;
  double mass_bin6_3;

  // Dust variable transformation
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      // Transform from 2 to 6 bin mass fractions
      mass_bin6_2 = mbin1View(jn, jl)*mo::constants::p1[1]*mo::constants::p1_scale;
      mass_bin6_3 = mbin1View(jn, jl)*mo::constants::p1[2]*mo::constants::p1_scale;
      // Transform from 6 bin mass fractions to accumulation mode number fraction
      naccView(jn, jl) = mo::constants::volconst * mass_bin6_2 / mo::constants::drep6c[1] +
                         mo::constants::volconst * mass_bin6_3 * 0.5 / mo::constants::drep6c[2];
    }
  }

  stateFlds["number_fraction_of_accumulation_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_accumulation_mode_number_fraction_nl()] ... exit" << std::endl;
}

void eval_dust_coarse_mode_number_fraction_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_coarse_mode_number_fraction_nl()] starting ..." << std::endl;

  // Input fields
  const auto mbin1View = make_view<const double, 2>(
                      stateFlds["mass_fraction_of_dust001_in_air"]);
  const auto mbin2View = make_view<const double, 2>(
                      stateFlds["mass_fraction_of_dust002_in_air"]);

  // Output fields
  auto ncorView = make_view<double, 2>(
                      stateFlds["number_fraction_of_coarse_aerosol_particles_in_air"]);

  // Indexing
  auto fspace = stateFlds["mass_fraction_of_dust001_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = stateFlds["mass_fraction_of_dust001_in_air"].shape(1);

  // Transform variables
  double mass_bin6_3;
  double mass_bin6_4;
  double mass_bin6_5;

  // Dust variable transformation
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      // Transform from 2 to 6 bin mass fractions
      mass_bin6_3 = mbin1View(jn, jl)*mo::constants::p1[2]*mo::constants::p1_scale;
      mass_bin6_4 = mbin1View(jn, jl)*mo::constants::p1[3]*mo::constants::p1_scale +
                    mbin2View(jn, jl)*mo::constants::p2[3]*mo::constants::p2_scale;
      mass_bin6_5 = mbin2View(jn, jl)*mo::constants::p2[4]*mo::constants::p2_scale;
      // Transform from 6 bin mass fractions to coarse mode number fraction
      ncorView(jn, jl) = mo::constants::volconst * mass_bin6_3 * 0.5 / mo::constants::drep6c[2] +
                         mo::constants::volconst * mass_bin6_4 / mo::constants::drep6c[3] +
                         mo::constants::volconst * mass_bin6_5 / mo::constants::drep6c[4];
    }
  }

  stateFlds["number_fraction_of_coarse_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_coarse_mode_number_fraction_nl()] ... exit" << std::endl;
}

// TL code

void eval_dust_accumulation_mode_mass_fraction_tl(atlas::FieldSet & incFlds,
                                          const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_accumulation_mode_mass_fraction_tl()] starting ..."
                     << std::endl;

  // Increment Fields
  const auto mbin1IncView = make_view<const double, 2>(incFlds["mass_fraction_of_dust001_in_air"]);
  auto maccIncView = make_view<double, 2>(
    incFlds["mass_fraction_of_dust_accumulation_aerosol_particles_in_air"]);

  // Indexing
  auto fspace = incFlds["mass_fraction_of_dust001_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = incFlds["mass_fraction_of_dust001_in_air"].shape(1);

  // Dust variable transformation
  double mass_bin6_2;
  double mass_bin6_3;

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      // Transform from 2 to 6 bin mass fractions
      mass_bin6_2 = mbin1IncView(jn, jl)*mo::constants::p1[1]*mo::constants::p1_scale;
      mass_bin6_3 = mbin1IncView(jn, jl)*mo::constants::p1[2]*mo::constants::p1_scale;
      // Transform from 6 bin mass fractions to accumulation mode mass fraction
      maccIncView(jn, jl) = mass_bin6_2 + mass_bin6_3 * 0.5;
    }
  }

  incFlds["mass_fraction_of_dust_accumulation_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_accumulation_mode_mass_fraction_tl()] ... exit" << std::endl;
}

void eval_dust_coarse_mode_mass_fraction_tl(atlas::FieldSet & incFlds,
                                          const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_coarse_mode_mass_fraction_tl()] starting ..." << std::endl;

  // Increment Fields
  const auto mbin1IncView = make_view<const double, 2>(incFlds["mass_fraction_of_dust001_in_air"]);
  const auto mbin2IncView = make_view<const double, 2>(incFlds["mass_fraction_of_dust002_in_air"]);
  auto mcorIncView = make_view<double, 2>(
    incFlds["mass_fraction_of_dust_coarse_aerosol_particles_in_air"]);

    // Indexing
  auto fspace = incFlds["mass_fraction_of_dust001_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = incFlds["mass_fraction_of_dust001_in_air"].shape(1);

  // Transform variables
  double mass_bin6_3;
  double mass_bin6_4;
  double mass_bin6_5;

  // Dust variable transformation
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      // transform from 2 to 6 bin mass fractions
      mass_bin6_3 = mbin1IncView(jn, jl)*mo::constants::p1[2]*mo::constants::p1_scale;
      mass_bin6_4 = mbin1IncView(jn, jl)*mo::constants::p1[3]*mo::constants::p1_scale +
                    mbin2IncView(jn, jl)*mo::constants::p2[3]*mo::constants::p2_scale;
      mass_bin6_5 = mbin2IncView(jn, jl)*mo::constants::p2[4]*mo::constants::p2_scale;
      // transform from 6 bin mass fractions to coarse mode mass fraction
      mcorIncView(jn, jl) = mass_bin6_3 * 0.5 + mass_bin6_4 + mass_bin6_5;
    }
  }

  incFlds["mass_fraction_of_dust_coarse_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_coarse_mode_mass_fraction_tl()] ... exit" << std::endl;
}

void eval_dust_accumulation_mode_number_fraction_tl(atlas::FieldSet & incFlds,
                                          const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_accumulation_mode_number_fraction_tl()] starting ..."
                     << std::endl;

  // Increment Fields
  const auto mbin1IncView = make_view<const double, 2>(incFlds["mass_fraction_of_dust001_in_air"]);
  auto naccIncView = make_view<double, 2>(
    incFlds["number_fraction_of_accumulation_aerosol_particles_in_air"]);

    // Indexing
  auto fspace = incFlds["mass_fraction_of_dust001_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = incFlds["mass_fraction_of_dust001_in_air"].shape(1);

  // Transform variables
  double mass_bin6_2;
  double mass_bin6_3;

  // Dust variable transformation
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      // Transform from 2 to 6 bin mass fractions
      mass_bin6_2 = mbin1IncView(jn, jl)*mo::constants::p1[1]*mo::constants::p1_scale;
      mass_bin6_3 = mbin1IncView(jn, jl)*mo::constants::p1[2]*mo::constants::p1_scale;
      // Transform from 6 bin mass fraction to accumulation mode number fraction
      naccIncView(jn, jl) = mo::constants::volconst * mass_bin6_2 / mo::constants::drep6c[1] +
                            mo::constants::volconst * mass_bin6_3 * 0.5 / mo::constants::drep6c[2];
    }
  }

  incFlds["number_fraction_of_accumulation_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_accumulation_mode_number_fraction_tl()] ... exit" << std::endl;
}

void eval_dust_coarse_mode_number_fraction_tl(atlas::FieldSet & incFlds,
                                          const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_coarse_mode_number_fraction_tl()] starting ..." << std::endl;

  // Increment Fields
  const auto mbin1IncView = make_view<const double, 2>(incFlds["mass_fraction_of_dust001_in_air"]);
  const auto mbin2IncView = make_view<const double, 2>(incFlds["mass_fraction_of_dust002_in_air"]);

  auto ncorIncView = make_view<double, 2>(
    incFlds["number_fraction_of_coarse_aerosol_particles_in_air"]);

    // Indexing
  auto fspace = incFlds["mass_fraction_of_dust001_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = incFlds["mass_fraction_of_dust001_in_air"].shape(1);

  // Transform variables
  double mass_bin6_3;
  double mass_bin6_4;
  double mass_bin6_5;

  // Dust variable transformation
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      // Transform from 2 to 6 bins
      mass_bin6_3 = mbin1IncView(jn, jl)*mo::constants::p1[2]*mo::constants::p1_scale;
      mass_bin6_4 = mbin1IncView(jn, jl)*mo::constants::p1[3]*mo::constants::p1_scale +
                    mbin2IncView(jn, jl)*mo::constants::p2[3]*mo::constants::p2_scale;
      mass_bin6_5 = mbin2IncView(jn, jl)*mo::constants::p2[4]*mo::constants::p2_scale;
      // Calculate coarse mode number fraction
      ncorIncView(jn, jl) = mo::constants::volconst *
                            ((mass_bin6_3 * 0.5 / mo::constants::drep6c[2]) +
                             (mass_bin6_4 / mo::constants::drep6c[3]) +
                             (mass_bin6_5 / mo::constants::drep6c[4]));
    }
  }

  incFlds["number_fraction_of_coarse_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_coarse_mode_number_fraction_tl()] ... exit" << std::endl;
}

void eval_dust_accumulation_mode_mass_fraction_ad(atlas::FieldSet & hatFlds,
                                          const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_accumulation_mode_mass_fraction_ad()] starting ..."
                     << std::endl;

  auto maccHatView = make_view<double, 2>(
                        hatFlds["mass_fraction_of_dust_accumulation_aerosol_particles_in_air"]);
  auto mbin1HatView = make_view<double, 2>(hatFlds["mass_fraction_of_dust001_in_air"]);

  // Indexing
  auto fspace = hatFlds["mass_fraction_of_dust_accumulation_aerosol_particles_in_air"].
                               functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = hatFlds["mass_fraction_of_dust_accumulation_aerosol_particles_in_air"].
                          shape(1);

    // Dust variable transformation
  double mass_bin6_2_hat;
  double mass_bin6_3_hat;

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      mass_bin6_2_hat = maccHatView(jn, jl);
      mass_bin6_3_hat = maccHatView(jn, jl) * 0.5;
      mbin1HatView(jn, jl) += mass_bin6_3_hat*mo::constants::p1[2]*mo::constants::p1_scale;
      mbin1HatView(jn, jl) += mass_bin6_2_hat*mo::constants::p1[1]*mo::constants::p1_scale;
      maccHatView(jn, jl) = 0.0;
    }
  }

  hatFlds["mass_fraction_of_dust001_in_air"].set_dirty();
  hatFlds["mass_fraction_of_dust_accumulation_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_accumulation_mode_mass_fraction_ad()] ... exit" << std::endl;
}

void eval_dust_coarse_mode_mass_fraction_ad(atlas::FieldSet & hatFlds,
                                          const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_coarse_mode_mass_fraction_ad()] starting ..." << std::endl;

  auto mcorHatView = make_view<double, 2>(
                        hatFlds["mass_fraction_of_dust_coarse_aerosol_particles_in_air"]);
  auto mbin1HatView = make_view<double, 2>(hatFlds["mass_fraction_of_dust001_in_air"]);
  auto mbin2HatView = make_view<double, 2>(hatFlds["mass_fraction_of_dust002_in_air"]);

  // Indexing
  auto fspace = hatFlds["mass_fraction_of_dust_coarse_aerosol_particles_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = hatFlds["mass_fraction_of_dust_coarse_aerosol_particles_in_air"].shape(1);

  // Transform variables
  double mass_bin6_3_hat;
  double mass_bin6_4_hat;
  double mass_bin6_5_hat;

  // Dust variable transformation
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      mass_bin6_3_hat = mcorHatView(jn, jl) * 0.5;
      mass_bin6_4_hat = mcorHatView(jn, jl);
      mass_bin6_5_hat = mcorHatView(jn, jl);
      mbin1HatView(jn, jl) += mass_bin6_3_hat*mo::constants::p1[2]*mo::constants::p1_scale;
      mbin1HatView(jn, jl) += mass_bin6_4_hat*mo::constants::p1[3]*mo::constants::p1_scale;
      mbin2HatView(jn, jl) += mass_bin6_4_hat*mo::constants::p2[3]*mo::constants::p2_scale;
      mbin2HatView(jn, jl) += mass_bin6_5_hat*mo::constants::p2[4]*mo::constants::p2_scale;
      mcorHatView(jn, jl) = 0.0;
    }
  }

  hatFlds["mass_fraction_of_dust001_in_air"].set_dirty();
  hatFlds["mass_fraction_of_dust002_in_air"].set_dirty();
  hatFlds["mass_fraction_of_dust_coarse_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_coarse_mode_mass_fraction_ad()] ... exit" << std::endl;
}

void eval_dust_accumulation_mode_number_fraction_ad(atlas::FieldSet & hatFlds,
                                          const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_accumulation_mode_number_fraction_ad()] starting ..."
                     << std::endl;

  auto naccHatView = make_view<double, 2>(
                        hatFlds["number_fraction_of_accumulation_aerosol_particles_in_air"]);
  auto mbin1HatView = make_view<double, 2>(hatFlds["mass_fraction_of_dust001_in_air"]);

  // Indexing
  auto fspace = hatFlds["number_fraction_of_accumulation_aerosol_particles_in_air"].
                               functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = hatFlds["number_fraction_of_accumulation_aerosol_particles_in_air"].
                          shape(1);

  // Transform variables
  double mass_bin6_2_hat;
  double mass_bin6_3_hat;

  // Dust variable transformation
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      mass_bin6_2_hat = naccHatView(jn, jl) * mo::constants::volconst
                        / mo::constants::drep6c[1];
      mass_bin6_3_hat = naccHatView(jn, jl) * mo::constants::volconst * 0.5
                        / mo::constants::drep6c[2];
      mbin1HatView(jn, jl) += mass_bin6_3_hat*mo::constants::p1[2]*mo::constants::p1_scale;
      mbin1HatView(jn, jl) += mass_bin6_2_hat*mo::constants::p1[1]*mo::constants::p1_scale;
      naccHatView(jn, jl) = 0.0;
    }
  }

  hatFlds["mass_fraction_of_dust001_in_air"].set_dirty();
  hatFlds["number_fraction_of_accumulation_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_accumulation_mode_number_fraction_ad()] ... exit" << std::endl;
}

void eval_dust_coarse_mode_number_fraction_ad(atlas::FieldSet & hatFlds,
                                          const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dust_coarse_mode_number_fraction_ad()] starting ..." << std::endl;

  auto ncorHatView = make_view<double, 2>(
                        hatFlds["number_fraction_of_coarse_aerosol_particles_in_air"]);
  auto mbin1HatView = make_view<double, 2>(hatFlds["mass_fraction_of_dust001_in_air"]);
  auto mbin2HatView = make_view<double, 2>(hatFlds["mass_fraction_of_dust002_in_air"]);

  // Indexing
  auto fspace = hatFlds["number_fraction_of_coarse_aerosol_particles_in_air"].functionspace();
  const idx_t sizeOwned = util::getSizeOwned(fspace);
  const idx_t numLevels = hatFlds["number_fraction_of_coarse_aerosol_particles_in_air"].
                          shape(1);

  // Transform variables
  double mass_bin6_3_hat;
  double mass_bin6_4_hat;
  double mass_bin6_5_hat;

  // Dust variable transformation
  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < numLevels; ++jl) {
      mass_bin6_3_hat = ncorHatView(jn, jl) * mo::constants::volconst * 0.5
                        / mo::constants::drep6c[2];
      mass_bin6_4_hat = ncorHatView(jn, jl) * mo::constants::volconst
                        / mo::constants::drep6c[3];
      mass_bin6_5_hat = ncorHatView(jn, jl) * mo::constants::volconst
                        / mo::constants::drep6c[4];
      mbin1HatView(jn, jl) += mass_bin6_3_hat*mo::constants::p1[2]*mo::constants::p1_scale;
      mbin1HatView(jn, jl) += mass_bin6_4_hat*mo::constants::p1[3]*mo::constants::p1_scale;
      mbin2HatView(jn, jl) += mass_bin6_4_hat*mo::constants::p2[3]*mo::constants::p2_scale;
      mbin2HatView(jn, jl) += mass_bin6_5_hat*mo::constants::p2[4]*mo::constants::p2_scale;
      ncorHatView(jn, jl) = 0.0;
    }
  }

  hatFlds["mass_fraction_of_dust001_in_air"].set_dirty();
  hatFlds["mass_fraction_of_dust002_in_air"].set_dirty();
  hatFlds["number_fraction_of_coarse_aerosol_particles_in_air"].set_dirty();
  oops::Log::trace() << "[eval_dust_coarse_mode_number_fraction_ad()] ... exit" << std::endl;
}

}  // namespace mo
