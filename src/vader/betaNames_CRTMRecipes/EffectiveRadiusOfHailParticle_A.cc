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
#include "vader/betaNames_CRTMRecipes/EffectiveRadiusOfHailParticle.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char EffectiveRadiusOfHailParticle_A::Name[] =
    "EffectiveRadiusOfHailParticle_A";
const oops::Variables EffectiveRadiusOfHailParticle_A::Ingredients{
    std::vector<std::string>{"air_temperature"}};

// Register the maker
static RecipeMaker<EffectiveRadiusOfHailParticle_A>
  makerEffectiveRadiusOfHailParticle_A_(
      EffectiveRadiusOfHailParticle_A::Name);

// -------------------------------------------------------------------------------------------------

EffectiveRadiusOfHailParticle_A::EffectiveRadiusOfHailParticle_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "EffectiveRadiusOfHailParticle_A::"
                     << "EffectiveRadiusOfHailParticle_A" << std::endl;
}

std::string EffectiveRadiusOfHailParticle_A::name() const {
  return EffectiveRadiusOfHailParticle_A::Name;
}

oops::Variable EffectiveRadiusOfHailParticle_A::product() const {
  return oops::Variable{"effective_radius_of_hail_particle"};
}

oops::Variables EffectiveRadiusOfHailParticle_A::ingredients() const {
  return EffectiveRadiusOfHailParticle_A::Ingredients;
}

size_t EffectiveRadiusOfHailParticle_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace EffectiveRadiusOfHailParticle_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfHailParticle_A::executeNL(
    atlas::FieldSet & afieldset) {
  oops::Log::trace() << "EffectiveRadiusOfHailParticle_A::"
                     << "executeNL starting" << std::endl;

  const double r_hail = 5.0e-3;  // 5 mm (m)
  util::for_each_value(
      [=](double& reff) { reff = r_hail; },
      afieldset.field("effective_radius_of_hail_particle"));

  oops::Log::trace() << "EffectiveRadiusOfHailParticle_A::"
                     << "executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfHailParticle_A::executeTL(
    atlas::FieldSet & afieldsetTL, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfHailParticle_A::executeTL starting" << std::endl;
  util::for_each_value(
      [](double& reff_tl) { reff_tl = 0.0; },
      afieldsetTL.field("effective_radius_of_hail_particle"));
  oops::Log::trace() << "EffectiveRadiusOfHailParticle_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfHailParticle_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfHailParticle_A::executeAD starting" << std::endl;
  util::for_each_value(
      [](double& reff_ad) { reff_ad = 0.0; },
      afieldsetAD.field("effective_radius_of_hail_particle"));
  oops::Log::trace() << "EffectiveRadiusOfHailParticle_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
