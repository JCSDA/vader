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
#include "vader/betaNames_CRTMRecipes/GSI_Specific/VegetationTypeIndex.h"

namespace vader {

const char VegetationTypeIndex_A::Name[] = "VegetationTypeIndex_A";
const oops::Variables VegetationTypeIndex_A::Ingredients{
    std::vector<std::string>{"gsi_surface_type_index", "vtype"}};

static RecipeMaker<VegetationTypeIndex_A> makerVegetationTypeIndex_A_(VegetationTypeIndex_A::Name);

// GSI map_model_sfc_to_crtm_mwave_vege (1-based in Fortran; here 0-based).
static constexpr std::array<int, 20> kMwaveVege = {
    4, 1, 5, 2, 3, 8, 9, 6, 6, 7, 8, 12, 7, 12, 13, 11, 0, 10, 10, 11};

VegetationTypeIndex_A::VegetationTypeIndex_A(const Parameters_ & params,
                                             const VaderConfigVars & configVariables) {
  oops::Log::trace() << "VegetationTypeIndex_A::VegetationTypeIndex_A(params)" << std::endl;
}

std::string VegetationTypeIndex_A::name() const { return VegetationTypeIndex_A::Name; }

oops::Variable VegetationTypeIndex_A::product() const {
  return oops::Variable{"vegetation_type_index"};
}

oops::Variables VegetationTypeIndex_A::ingredients() const {
  return VegetationTypeIndex_A::Ingredients;
}

size_t VegetationTypeIndex_A::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace VegetationTypeIndex_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void VegetationTypeIndex_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "VegetationTypeIndex_A::executeNL starting" << std::endl;

  const int table_size = static_cast<int>(kMwaveVege.size());

  util::for_each_value(
      [table_size](const double sfc_type, const double vtype_r, double& out) {
          if (std::lround(sfc_type) == 1) {
              int vtype = static_cast<int>(std::lround(vtype_r));
              vtype = std::min(std::max(1, vtype), table_size);
              out = static_cast<double>(std::max(1, kMwaveVege[vtype - 1]));
          } else {
              out = 4.0;  // evergreen needleleaf forest default
          }
      },
      afieldset.field("gsi_surface_type_index"),
      afieldset.field("vtype"),
      afieldset.field("vegetation_type_index"));

  oops::Log::trace() << "VegetationTypeIndex_A::executeNL done" << std::endl;
}

}  // namespace vader
