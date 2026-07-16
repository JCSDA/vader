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
#include "vader/betaNames_CRTMRecipes/GSI_Specific/SoilTemperature.h"

namespace vader {

const char SoilTemperature_A::Name[] = "SoilTemperature_A";
const oops::Variables SoilTemperature_A::Ingredients{std::vector<std::string>{
    "gsi_surface_type_index", "tslb", "vtype", "stype"}};

static RecipeMaker<SoilTemperature_A> makerSoilTemperature_A_(SoilTemperature_A::Name);

SoilTemperature_A::SoilTemperature_A(const Parameters_ & params,
                                     const VaderConfigVars & configVariables) {
  oops::Log::trace() << "SoilTemperature_A::SoilTemperature_A(params)" << std::endl;
}

std::string SoilTemperature_A::name() const { return SoilTemperature_A::Name; }

oops::Variable SoilTemperature_A::product() const {
  return oops::Variable{"soil_temperature"};
}

oops::Variables SoilTemperature_A::ingredients() const { return SoilTemperature_A::Ingredients; }

size_t SoilTemperature_A::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace SoilTemperature_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void SoilTemperature_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SoilTemperature_A::executeNL starting" << std::endl;

  auto sfc_type_view = atlas::array::make_view<const double, 2>(
      afieldset.field("gsi_surface_type_index"));
  auto stc_view = atlas::array::make_view<const double, 2>(afieldset.field("tslb"));
  auto vtype_view = atlas::array::make_view<const double, 2>(afieldset.field("vtype"));
  auto stype_view = atlas::array::make_view<const double, 2>(afieldset.field("stype"));
  auto soil_temp_view = atlas::array::make_view<double, 2>(afieldset.field("soil_temperature"));

  const size_t nx = sfc_type_view.shape(0);

  // Pass 1: soil_temperature = stc where slmsk==1, else 0
  bool any_glacial = false;
  for (size_t i = 0; i < nx; ++i) {
    const int64_t sfc = std::lround(sfc_type_view(i, 0));
    if (sfc == 1) {
      soil_temp_view(i, 0) = stc_view(i, 0);
      const int64_t vtype = std::lround(vtype_view(i, 0));
      const int64_t stype = std::lround(stype_view(i, 0));
      if (vtype == 15 || stype == 16) any_glacial = true;
    } else {
      soil_temp_view(i, 0) = 0.0;
    }
  }

  // Pass 2: if any cell triggered the glacial reset, zero the entire field
  if (any_glacial) {
    for (size_t i = 0; i < nx; ++i) soil_temp_view(i, 0) = 0.0;
  }

  oops::Log::trace() << "SoilTemperature_A::executeNL done" << std::endl;
}

}  // namespace vader
