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
#include "vader/betaNames_CRTMRecipes/EffectiveRadiusOfRainParticle.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char EffectiveRadiusOfRainParticle_A::Name[] =
    "EffectiveRadiusOfRainParticle_A";
const oops::Variables EffectiveRadiusOfRainParticle_A::Ingredients{
    std::vector<std::string>{"air_temperature"}};

// Register the maker
static RecipeMaker<EffectiveRadiusOfRainParticle_A>
  makerEffectiveRadiusOfRainParticle_A_(
      EffectiveRadiusOfRainParticle_A::Name);

// -------------------------------------------------------------------------------------------------

EffectiveRadiusOfRainParticle_A::EffectiveRadiusOfRainParticle_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "EffectiveRadiusOfRainParticle_A::"
                     << "EffectiveRadiusOfRainParticle_A" << std::endl;
}

std::string EffectiveRadiusOfRainParticle_A::name() const {
  return EffectiveRadiusOfRainParticle_A::Name;
}

oops::Variable EffectiveRadiusOfRainParticle_A::product() const {
  return oops::Variable{"effective_radius_of_rain_particle"};
}

oops::Variables EffectiveRadiusOfRainParticle_A::ingredients() const {
  return EffectiveRadiusOfRainParticle_A::Ingredients;
}

size_t EffectiveRadiusOfRainParticle_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace EffectiveRadiusOfRainParticle_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfRainParticle_A::executeNL(
    atlas::FieldSet & afieldset) {
  oops::Log::trace() << "EffectiveRadiusOfRainParticle_A::"
                     << "executeNL starting" << std::endl;

  const double r_rain = 250.0;  // microns
  util::for_each_value(
      [=](double& reff) { reff = r_rain; },
      afieldset.field("effective_radius_of_rain_particle"));

  oops::Log::trace() << "EffectiveRadiusOfRainParticle_A::"
                     << "executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfRainParticle_A::executeTL(
    atlas::FieldSet & afieldsetTL, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfRainParticle_A::executeTL starting" << std::endl;
  util::for_each_value(
      [](double& reff_tl) { reff_tl = 0.0; },
      afieldsetTL.field("effective_radius_of_rain_particle"));
  oops::Log::trace() << "EffectiveRadiusOfRainParticle_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void EffectiveRadiusOfRainParticle_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "EffectiveRadiusOfRainParticle_A::executeAD starting" << std::endl;
  util::for_each_value(
      [](double& reff_ad) { reff_ad = 0.0; },
      afieldsetAD.field("effective_radius_of_rain_particle"));
  oops::Log::trace() << "EffectiveRadiusOfRainParticle_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
