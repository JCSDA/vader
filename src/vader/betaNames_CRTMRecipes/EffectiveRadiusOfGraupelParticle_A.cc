/*
 * (C) Copyright 2025 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/EffectiveRadiusOfGraupelParticle.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char EffectiveRadiusOfGraupelParticle_A::Name[] =
    "EffectiveRadiusOfGraupelParticle_A";
const oops::Variables EffectiveRadiusOfGraupelParticle_A::Ingredients{
    std::vector<std::string>{"air_temperature"}};

// Register the maker
static RecipeMaker<EffectiveRadiusOfGraupelParticle_A>
  makerEffectiveRadiusOfGraupelParticle_A_(
      EffectiveRadiusOfGraupelParticle_A::Name);

// -------------------------------------------------------------------------------------------------

EffectiveRadiusOfGraupelParticle_A::EffectiveRadiusOfGraupelParticle_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "EffectiveRadiusOfGraupelParticle_A::"
                     << "EffectiveRadiusOfGraupelParticle_A" << std::endl;
}

std::string EffectiveRadiusOfGraupelParticle_A::name() const {
  return EffectiveRadiusOfGraupelParticle_A::Name;
}

oops::Variable EffectiveRadiusOfGraupelParticle_A::product() const {
  return oops::Variable{"effective_radius_of_graupel_particle"};
}

oops::Variables EffectiveRadiusOfGraupelParticle_A::ingredients() const {
  return EffectiveRadiusOfGraupelParticle_A::Ingredients;
}

size_t EffectiveRadiusOfGraupelParticle_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace EffectiveRadiusOfGraupelParticle_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfGraupelParticle_A::executeNL(
    atlas::FieldSet & afieldset) {
  oops::Log::trace() << "EffectiveRadiusOfGraupelParticle_A::"
                     << "executeNL starting" << std::endl;

  const double r_graupel = 750.0e-6;  // 750 micrometers (m)
  util::for_each_value(
      [=](double& reff) { reff = r_graupel; },
      afieldset.field("effective_radius_of_graupel_particle"));

  oops::Log::trace() << "EffectiveRadiusOfGraupelParticle_A::"
                     << "executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfGraupelParticle_A::executeTL(
    atlas::FieldSet & afieldsetTL, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfGraupelParticle_A::executeTL starting" << std::endl;
  util::for_each_value(
      [](double& reff_tl) { reff_tl = 0.0; },
      afieldsetTL.field("effective_radius_of_graupel_particle"));
  oops::Log::trace() << "EffectiveRadiusOfGraupelParticle_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfGraupelParticle_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfGraupelParticle_A::executeAD starting" << std::endl;
  util::for_each_value(
      [](double& reff_ad) { reff_ad = 0.0; },
      afieldsetAD.field("effective_radius_of_graupel_particle"));
  oops::Log::trace() << "EffectiveRadiusOfGraupelParticle_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
