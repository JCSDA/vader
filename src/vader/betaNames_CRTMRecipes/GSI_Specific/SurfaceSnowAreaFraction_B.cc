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
#include "vader/betaNames_CRTMRecipes/SurfaceSnowAreaFraction.h"

namespace vader {

const char SurfaceSnowAreaFraction_B::Name[] = "SurfaceSnowAreaFraction_B";
const oops::Variables SurfaceSnowAreaFraction_B::Ingredients{
    std::vector<std::string>{"gsi_surface_type_index"}};

static RecipeMaker<SurfaceSnowAreaFraction_B> makerSurfaceSnowAreaFraction_B_(
    SurfaceSnowAreaFraction_B::Name);

SurfaceSnowAreaFraction_B::SurfaceSnowAreaFraction_B(const Parameters_ & params,
                                                     const VaderConfigVars & configVariables) {
  oops::Log::trace() << "SurfaceSnowAreaFraction_B::SurfaceSnowAreaFraction_B(params)" << std::endl;
}

std::string SurfaceSnowAreaFraction_B::name() const { return SurfaceSnowAreaFraction_B::Name; }

oops::Variable SurfaceSnowAreaFraction_B::product() const {
  return oops::Variable{"surface_snow_area_fraction"};
}

oops::Variables SurfaceSnowAreaFraction_B::ingredients() const {
  return SurfaceSnowAreaFraction_B::Ingredients;
}

size_t SurfaceSnowAreaFraction_B::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace SurfaceSnowAreaFraction_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void SurfaceSnowAreaFraction_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SurfaceSnowAreaFraction_B::executeNL starting" << std::endl;

  util::for_each_value(
      [](const double sfc_type, double& snow_frac) {
          snow_frac = (std::lround(sfc_type) == 3) ? 1.0 : 0.0;
      },
      afieldset.field("gsi_surface_type_index"),
      afieldset.field("surface_snow_area_fraction"));

  oops::Log::trace() << "SurfaceSnowAreaFraction_B::executeNL done" << std::endl;
}

}  // namespace vader
