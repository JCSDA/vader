/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/GSI_Specific/GsiSurfaceTypeIndex.h"

namespace vader {

// Static attribute initialization
const char GsiSurfaceTypeIndex_A::Name[] = "GsiSurfaceTypeIndex_A";
const oops::Variables GsiSurfaceTypeIndex_A::Ingredients{
    std::vector<std::string>{"slmsk", "gsi_snow_water_equivalent"}};

// Register the maker
static RecipeMaker<GsiSurfaceTypeIndex_A> makerGsiSurfaceTypeIndex_A_(
    GsiSurfaceTypeIndex_A::Name);

GsiSurfaceTypeIndex_A::GsiSurfaceTypeIndex_A(const Parameters_ & params,
                                             const VaderConfigVars & configVariables) {
  oops::Log::trace() << "GsiSurfaceTypeIndex_A::GsiSurfaceTypeIndex_A(params)" << std::endl;
}

std::string GsiSurfaceTypeIndex_A::name() const {
  return GsiSurfaceTypeIndex_A::Name;
}

oops::Variable GsiSurfaceTypeIndex_A::product() const {
  return oops::Variable{"gsi_surface_type_index"};
}

oops::Variables GsiSurfaceTypeIndex_A::ingredients() const {
  return GsiSurfaceTypeIndex_A::Ingredients;
}

size_t GsiSurfaceTypeIndex_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace GsiSurfaceTypeIndex_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("slmsk").functionspace();
}

// -------------------------------------------------------------------------------------------------

void GsiSurfaceTypeIndex_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "GsiSurfaceTypeIndex_A::executeNL starting" << std::endl;

  const double minswe = 0.1;

  util::for_each_value(
      [minswe](const double slmsk,
               const double swe,
               double& sfc_type) {
          const int64_t idx = std::lround(slmsk);
          sfc_type = (idx >= 1 && swe > minswe) ? 3.0 : static_cast<double>(idx);
      },
      afieldset.field("slmsk"),
      afieldset.field("gsi_snow_water_equivalent"),
      afieldset.field("gsi_surface_type_index"));

  oops::Log::trace() << "GsiSurfaceTypeIndex_A::executeNL done" << std::endl;
}

}  // namespace vader
