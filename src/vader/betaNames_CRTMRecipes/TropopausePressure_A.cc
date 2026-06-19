/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/TropopausePressure.h"

namespace vader
{

// Static attribute initialization
const char TropopausePressure_A::Name[] = "TropopausePressure_A";
const oops::Variables TropopausePressure_A::Ingredients{
  std::vector<std::string>{"air_pressure", "air_temperature", "geopotential_height"}};

// Register the maker
static RecipeMaker<TropopausePressure_A> makerTropopausePressure_A_(TropopausePressure_A::Name);

// -------------------------------------------------------------------------------------------------

TropopausePressure_A::TropopausePressure_A(const Parameters_ & params,
                       const VaderConfigVars & configVariables) :
  configVariables_{configVariables}
{
  oops::Log::trace() << "TropopausePressure_A::TropopausePressure_A" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string TropopausePressure_A::name() const {
  return TropopausePressure_A::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable TropopausePressure_A::product() const {
  return oops::Variable{"tropopause_pressure"};
}

// -------------------------------------------------------------------------------------------------

oops::Variables TropopausePressure_A::ingredients() const {
  return TropopausePressure_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t TropopausePressure_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace TropopausePressure_A::productFunctionSpace(const atlas::FieldSet & afieldset)
const {
  return afieldset.field("air_pressure").functionspace();
}

// -------------------------------------------------------------------------------------------------

void TropopausePressure_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "TropopausePressure_A::executeNL Starting" << std::endl;

  const size_t nlevels = afieldset.field("air_pressure").shape(1);
  const double kappa = configVariables_.getDouble("kappa");
  const double p0 = configVariables_.getDouble("reference_pressure");

  // Tropopause detection thresholds
  const double P_50_HPA = 5000.0;
  const double P_150_HPA = 15000.0;
  const double MIN_PRESSURE_DIFF = 3500.0;
  const double MAX_PRESSURE_LIMIT = 70000.0;
  const double THETA_GRADIENT_THRESHOLD = 10.0 / 1500.0;

  // Algorithm expects p2[0] = surface (highest pressure), p2[nlevels-1] = top.
  const bool topDown = configVariables_.getBool("levels_are_top_down");

  util::for_each_column(
      [&](const auto p_col,
          const auto t_col,
          const auto z_col,
          auto tprs_col) {
          std::vector<double> p2(nlevels), t2(nlevels), z2(nlevels), theta(nlevels);
          for (size_t k = 0; k < nlevels; ++k) {
            const size_t src = topDown ? nlevels - k - 1 : k;
            p2[k] = p_col(src);
            t2[k] = t_col(src);
            z2[k] = z_col(src);
          }

          int k_p50 = 0;
          int k_p150 = 0;

          for (int k = nlevels - 1; k >= 0; --k) {
            theta[k] = t2[k] * std::pow(p0 / p2[k], kappa);
            if (p2[k] > P_50_HPA && k_p50 == 0) k_p50 = k;
            if (p2[k] > P_150_HPA && k_p150 == 0) k_p150 = k;
          }

          k_p150 = std::max(2, std::min(k_p150, k_p50 - 3));

          int k_tropo = k_p50 - 1;
          for (int k = std::max(0, k_p150 - 2); k >= 0; --k) {
            int k1 = k;
            int k2 = k1 + 2;

            while (k1 > 0 && (p2[k1] - p2[k2]) < MIN_PRESSURE_DIFF &&
                   p2[k1] < MAX_PRESSURE_LIMIT) {
              k1 = k1 - 1;
            }

            if (z2[k2] != z2[k1]) {
              double theta_gradient = (theta[k2] - theta[k1]) / (z2[k2] - z2[k1]);
              if (theta_gradient < THETA_GRADIENT_THRESHOLD) {
                k_tropo = static_cast<int>(std::round(0.5 * (k1 + k2 + 1)));
                break;
              }
            }
          }

          k_tropo = std::max(3, std::min(k_tropo, k_p50 - 1));
          tprs_col(0) = p2[k_tropo];
      },
      afieldset.field("air_pressure"),
      afieldset.field("air_temperature"),
      afieldset.field("geopotential_height"),
      afieldset.field("tropopause_pressure"));

  oops::Log::trace() << "TropopausePressure_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void TropopausePressure_A::executeTL(atlas::FieldSet & afieldsetTL,
                                     const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "TropopausePressure_A::executeTL Starting" << std::endl;
  // Tropopause detection is a discontinuous index search; TL is zero.
  util::for_each_value(
      [](double& tprs_tl) { tprs_tl = 0.0; },
      afieldsetTL.field("tropopause_pressure"));
  oops::Log::trace() << "TropopausePressure_A::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void TropopausePressure_A::executeAD(atlas::FieldSet & afieldsetAD,
                                     const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "TropopausePressure_A::executeAD Starting" << std::endl;
  util::for_each_value(
      [](double& tprs_ad) { tprs_ad = 0.0; },
      afieldsetAD.field("tropopause_pressure"));
  oops::Log::trace() << "TropopausePressure_A::executeAD Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
