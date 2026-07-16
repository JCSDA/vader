/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/GSI_Specific/GsiSnowWaterEquivalent.h"

namespace vader {

// Static attribute initialization
const char GsiSnowWaterEquivalent_A::Name[] = "GsiSnowWaterEquivalent_A";
const oops::Variables GsiSnowWaterEquivalent_A::Ingredients{
    std::vector<std::string>{"sheleg"}};

// Register the maker
static RecipeMaker<GsiSnowWaterEquivalent_A> makerGsiSnowWaterEquivalent_A_(
    GsiSnowWaterEquivalent_A::Name);

GsiSnowWaterEquivalent_A::GsiSnowWaterEquivalent_A(const Parameters_ & params,
                                                   const VaderConfigVars & configVariables) {
  oops::Log::trace() << "GsiSnowWaterEquivalent_A::GsiSnowWaterEquivalent_A(params)" << std::endl;
}

std::string GsiSnowWaterEquivalent_A::name() const {
  return GsiSnowWaterEquivalent_A::Name;
}

oops::Variable GsiSnowWaterEquivalent_A::product() const {
  return oops::Variable{"gsi_snow_water_equivalent"};
}

oops::Variables GsiSnowWaterEquivalent_A::ingredients() const {
  return GsiSnowWaterEquivalent_A::Ingredients;
}

size_t GsiSnowWaterEquivalent_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace GsiSnowWaterEquivalent_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("sheleg").functionspace();
}

// -------------------------------------------------------------------------------------------------

void GsiSnowWaterEquivalent_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "GsiSnowWaterEquivalent_A::executeNL starting" << std::endl;

  util::for_each_value(
      [](const double sheleg,
         double& swe) {
          swe = (std::abs(sheleg) > 10.0e10) ? 0.0 : sheleg;
      },
      afieldset.field("sheleg"),
      afieldset.field("gsi_snow_water_equivalent"));

  oops::Log::trace() << "GsiSnowWaterEquivalent_A::executeNL done" << std::endl;
}

}  // namespace vader
