/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/GSI_Specific/LandTypeIndexNpoess.h"

namespace vader {

const char LandTypeIndexNpoess_A::Name[] = "LandTypeIndexNpoess_A";
const oops::Variables LandTypeIndexNpoess_A::Ingredients{
    std::vector<std::string>{"gsi_surface_type_index", "vtype"}};

static RecipeMaker<LandTypeIndexNpoess_A>
    makerLandTypeIndexNpoess_A_(LandTypeIndexNpoess_A::Name);

// GSI igbp_to_npoess lookup (1-based in Fortran; here 0-based with index = vtype-1).
// PINE_FOREST=9, BROADLEAF_FOREST=8, BROADLEAF_PINE_FOREST=12, SCRUB=7, SCRUB_SOIL=19,
// BROADLEAF_BRUSH=17, TILLED_SOIL=2, URBAN_CONCRETE=15, INVALID_LAND=0, COMPACTED_SOIL=1,
// TUNDRA=10
static constexpr std::array<int, 20> kIgbpToNpoess = {
    9, 8, 9, 8, 12, 7, 19, 17, 17, 7, 17, 2, 15, 2, 0, 1, 0, 10, 10, 10};

LandTypeIndexNpoess_A::LandTypeIndexNpoess_A(const Parameters_ & params,
                                             const VaderConfigVars & configVariables) {
  oops::Log::trace() << "LandTypeIndexNpoess_A::LandTypeIndexNpoess_A(params)" << std::endl;
}

std::string LandTypeIndexNpoess_A::name() const { return LandTypeIndexNpoess_A::Name; }

oops::Variable LandTypeIndexNpoess_A::product() const {
  return oops::Variable{"land_type_index_NPOESS"};
}

oops::Variables LandTypeIndexNpoess_A::ingredients() const {
  return LandTypeIndexNpoess_A::Ingredients;
}

size_t LandTypeIndexNpoess_A::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace LandTypeIndexNpoess_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void LandTypeIndexNpoess_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "LandTypeIndexNpoess_A::executeNL starting" << std::endl;

  const int table_size = static_cast<int>(kIgbpToNpoess.size());

  util::for_each_value(
      [table_size](const double sfc_type, const double vtype_r, double& out) {
          if (std::lround(sfc_type) == 1) {
              int vtype = static_cast<int>(std::lround(vtype_r));
              vtype = std::min(std::max(1, vtype), table_size);
              out = static_cast<double>(std::max(1, kIgbpToNpoess[vtype - 1]));
          } else {
              out = 9.0;  // pine forest default
          }
      },
      afieldset.field("gsi_surface_type_index"),
      afieldset.field("vtype"),
      afieldset.field("land_type_index_NPOESS"));

  oops::Log::trace() << "LandTypeIndexNpoess_A::executeNL done" << std::endl;
}

}  // namespace vader
