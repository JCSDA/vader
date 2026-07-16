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
#include "vader/betaNames_CRTMRecipes/GSI_Specific/LandTypeIndexIgbp.h"

namespace vader {

const char LandTypeIndexIgbp_A::Name[] = "LandTypeIndexIgbp_A";
const oops::Variables LandTypeIndexIgbp_A::Ingredients{
    std::vector<std::string>{"gsi_surface_type_index", "vtype"}};

static RecipeMaker<LandTypeIndexIgbp_A> makerLandTypeIndexIgbp_A_(LandTypeIndexIgbp_A::Name);

// GSI igbp_to_igbp lookup: identity mapping (1..20).
static constexpr std::array<int, 20> kIgbpToIgbp = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

LandTypeIndexIgbp_A::LandTypeIndexIgbp_A(const Parameters_ & params,
                                         const VaderConfigVars & configVariables) {
  oops::Log::trace() << "LandTypeIndexIgbp_A::LandTypeIndexIgbp_A(params)" << std::endl;
}

std::string LandTypeIndexIgbp_A::name() const { return LandTypeIndexIgbp_A::Name; }

oops::Variable LandTypeIndexIgbp_A::product() const {
  return oops::Variable{"land_type_index_IGBP"};
}

oops::Variables LandTypeIndexIgbp_A::ingredients() const {
  return LandTypeIndexIgbp_A::Ingredients;
}

size_t LandTypeIndexIgbp_A::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace LandTypeIndexIgbp_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void LandTypeIndexIgbp_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "LandTypeIndexIgbp_A::executeNL starting" << std::endl;

  const int table_size = static_cast<int>(kIgbpToIgbp.size());

  util::for_each_value(
      [table_size](const double sfc_type, const double vtype_r, double& out) {
          if (std::lround(sfc_type) == 1) {
              int vtype = static_cast<int>(std::lround(vtype_r));
              vtype = std::min(std::max(1, vtype), table_size);
              out = static_cast<double>(std::max(1, kIgbpToIgbp[vtype - 1]));
          } else {
              out = 1.0;  // evergreen needleleaf forest default
          }
      },
      afieldset.field("gsi_surface_type_index"),
      afieldset.field("vtype"),
      afieldset.field("land_type_index_IGBP"));

  oops::Log::trace() << "LandTypeIndexIgbp_A::executeNL done" << std::endl;
}

}  // namespace vader
