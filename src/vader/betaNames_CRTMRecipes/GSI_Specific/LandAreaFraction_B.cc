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
#include "vader/betaNames_CRTMRecipes/LandAreaFraction.h"

namespace vader {

const char LandAreaFraction_B::Name[] = "LandAreaFraction_B";
const oops::Variables LandAreaFraction_B::Ingredients{
    std::vector<std::string>{"gsi_surface_type_index", "vtype", "stype"}};

static RecipeMaker<LandAreaFraction_B> makerLandAreaFraction_B_(LandAreaFraction_B::Name);

LandAreaFraction_B::LandAreaFraction_B(const Parameters_ & params,
                                       const VaderConfigVars & configVariables) {
  oops::Log::trace() << "LandAreaFraction_B::LandAreaFraction_B(params)" << std::endl;
}

std::string LandAreaFraction_B::name() const { return LandAreaFraction_B::Name; }

oops::Variable LandAreaFraction_B::product() const {
  return oops::Variable{"land_area_fraction"};
}

oops::Variables LandAreaFraction_B::ingredients() const { return LandAreaFraction_B::Ingredients; }

size_t LandAreaFraction_B::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace LandAreaFraction_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void LandAreaFraction_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "LandAreaFraction_B::executeNL starting" << std::endl;

  util::for_each_value(
      [](const double sfc_type,
         const double vtype_r,
         const double stype_r,
         double& land_frac) {
          if (std::lround(sfc_type) == 1) {
              const int64_t vtype = std::lround(vtype_r);
              const int64_t stype = std::lround(stype_r);
              land_frac = (vtype == 15 || stype == 16) ? 0.0 : 1.0;
          } else {
              land_frac = 0.0;
          }
      },
      afieldset.field("gsi_surface_type_index"),
      afieldset.field("vtype"),
      afieldset.field("stype"),
      afieldset.field("land_area_fraction"));

  oops::Log::trace() << "LandAreaFraction_B::executeNL done" << std::endl;
}

}  // namespace vader
