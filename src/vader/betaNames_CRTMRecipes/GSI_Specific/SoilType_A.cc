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
#include "vader/betaNames_CRTMRecipes/GSI_Specific/SoilType.h"

namespace vader {

const char SoilType_A::Name[] = "SoilType_A";
const oops::Variables SoilType_A::Ingredients{
    std::vector<std::string>{"gsi_surface_type_index", "stype"}};

static RecipeMaker<SoilType_A> makerSoilType_A_(SoilType_A::Name);

// GSI map_model_soil_to_crtm_mwave_soil (1-based in Fortran; here 0-based).
static constexpr std::array<int, 16> kMwaveSoil = {
    1, 1, 4, 2, 2, 8, 7, 2, 6, 5, 2, 3, 8, 1, 6, 9};

SoilType_A::SoilType_A(const Parameters_ & params,
                       const VaderConfigVars & configVariables) {
  oops::Log::trace() << "SoilType_A::SoilType_A(params)" << std::endl;
}

std::string SoilType_A::name() const { return SoilType_A::Name; }

oops::Variable SoilType_A::product() const { return oops::Variable{"soil_type"}; }

oops::Variables SoilType_A::ingredients() const { return SoilType_A::Ingredients; }

size_t SoilType_A::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace SoilType_A::productFunctionSpace(const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void SoilType_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SoilType_A::executeNL starting" << std::endl;

  const int table_size = static_cast<int>(kMwaveSoil.size());

  util::for_each_value(
      [table_size](const double sfc_type, const double stype_r, double& out) {
          if (std::lround(sfc_type) == 1) {
              int stype = static_cast<int>(std::lround(stype_r));
              stype = std::min(std::max(1, stype), table_size);
              out = static_cast<double>(kMwaveSoil[stype - 1]);
          } else {
              out = 1.0;  // coarse loamy sand default
          }
      },
      afieldset.field("gsi_surface_type_index"),
      afieldset.field("stype"),
      afieldset.field("soil_type"));

  oops::Log::trace() << "SoilType_A::executeNL done" << std::endl;
}

}  // namespace vader
