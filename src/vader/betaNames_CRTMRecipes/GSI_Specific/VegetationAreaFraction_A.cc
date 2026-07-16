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

#include "atlas/array.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/GSI_Specific/VegetationAreaFraction.h"

namespace vader {

const char VegetationAreaFraction_A::Name[] = "VegetationAreaFraction_A";
const oops::Variables VegetationAreaFraction_A::Ingredients{std::vector<std::string>{
    "gsi_surface_type_index", "vfrac", "vtype", "stype"}};

static RecipeMaker<VegetationAreaFraction_A>
    makerVegetationAreaFraction_A_(VegetationAreaFraction_A::Name);

VegetationAreaFraction_A::VegetationAreaFraction_A(const Parameters_ & params,
                                                   const VaderConfigVars & configVariables) {
  oops::Log::trace() << "VegetationAreaFraction_A::VegetationAreaFraction_A(params)" << std::endl;
}

std::string VegetationAreaFraction_A::name() const { return VegetationAreaFraction_A::Name; }

oops::Variable VegetationAreaFraction_A::product() const {
  return oops::Variable{"vegetation_area_fraction"};
}

oops::Variables VegetationAreaFraction_A::ingredients() const {
  return VegetationAreaFraction_A::Ingredients;
}

size_t VegetationAreaFraction_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace VegetationAreaFraction_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void VegetationAreaFraction_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "VegetationAreaFraction_A::executeNL starting" << std::endl;

  auto sfc_type_view = atlas::array::make_view<const double, 2>(
      afieldset.field("gsi_surface_type_index"));
  auto vfrac_view = atlas::array::make_view<const double, 2>(afieldset.field("vfrac"));
  auto vtype_view = atlas::array::make_view<const double, 2>(afieldset.field("vtype"));
  auto stype_view = atlas::array::make_view<const double, 2>(afieldset.field("stype"));
  auto out_view = atlas::array::make_view<double, 2>(
      afieldset.field("vegetation_area_fraction"));

  const size_t nx = sfc_type_view.shape(0);

  bool any_glacial = false;
  for (size_t i = 0; i < nx; ++i) {
    const int64_t sfc = std::lround(sfc_type_view(i, 0));
    if (sfc == 1) {
      out_view(i, 0) = vfrac_view(i, 0);
      const int64_t vtype = std::lround(vtype_view(i, 0));
      const int64_t stype = std::lround(stype_view(i, 0));
      if (vtype == 15 || stype == 16) any_glacial = true;
    } else {
      out_view(i, 0) = 0.0;
    }
  }

  if (any_glacial) {
    for (size_t i = 0; i < nx; ++i) out_view(i, 0) = 0.0;
  }

  oops::Log::trace() << "VegetationAreaFraction_A::executeNL done" << std::endl;
}

}  // namespace vader
