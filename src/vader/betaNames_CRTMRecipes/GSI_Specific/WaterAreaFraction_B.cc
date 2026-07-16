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
#include "vader/betaNames_CRTMRecipes/WaterAreaFraction.h"

namespace vader {

const char WaterAreaFraction_B::Name[] = "WaterAreaFraction_B";
const oops::Variables WaterAreaFraction_B::Ingredients{
    std::vector<std::string>{"gsi_surface_type_index"}};

static RecipeMaker<WaterAreaFraction_B> makerWaterAreaFraction_B_(WaterAreaFraction_B::Name);

WaterAreaFraction_B::WaterAreaFraction_B(const Parameters_ & params,
                                         const VaderConfigVars & configVariables) {
  oops::Log::trace() << "WaterAreaFraction_B::WaterAreaFraction_B(params)" << std::endl;
}

std::string WaterAreaFraction_B::name() const { return WaterAreaFraction_B::Name; }

oops::Variable WaterAreaFraction_B::product() const {
  return oops::Variable{"water_area_fraction"};
}

oops::Variables WaterAreaFraction_B::ingredients() const {
  return WaterAreaFraction_B::Ingredients;
}

size_t WaterAreaFraction_B::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace WaterAreaFraction_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void WaterAreaFraction_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "WaterAreaFraction_B::executeNL starting" << std::endl;

  util::for_each_value(
      [](const double sfc_type, double& water_frac) {
          water_frac = (std::lround(sfc_type) == 0) ? 1.0 : 0.0;
      },
      afieldset.field("gsi_surface_type_index"),
      afieldset.field("water_area_fraction"));

  oops::Log::trace() << "WaterAreaFraction_B::executeNL done" << std::endl;
}

}  // namespace vader
