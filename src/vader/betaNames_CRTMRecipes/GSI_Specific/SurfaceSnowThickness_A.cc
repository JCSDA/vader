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
#include "vader/betaNames_CRTMRecipes/GSI_Specific/SurfaceSnowThickness.h"

namespace vader {

const char SurfaceSnowThickness_A::Name[] = "SurfaceSnowThickness_A";
const oops::Variables SurfaceSnowThickness_A::Ingredients{
    std::vector<std::string>{"gsi_surface_type_index", "gsi_snow_water_equivalent"}};

static RecipeMaker<SurfaceSnowThickness_A>
    makerSurfaceSnowThickness_A_(SurfaceSnowThickness_A::Name);

SurfaceSnowThickness_A::SurfaceSnowThickness_A(const Parameters_ & params,
                                               const VaderConfigVars & configVariables) {
  oops::Log::trace() << "SurfaceSnowThickness_A::SurfaceSnowThickness_A(params)" << std::endl;
}

std::string SurfaceSnowThickness_A::name() const { return SurfaceSnowThickness_A::Name; }

oops::Variable SurfaceSnowThickness_A::product() const {
  return oops::Variable{"surface_snow_thickness"};
}

oops::Variables SurfaceSnowThickness_A::ingredients() const {
  return SurfaceSnowThickness_A::Ingredients;
}

size_t SurfaceSnowThickness_A::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace SurfaceSnowThickness_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void SurfaceSnowThickness_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SurfaceSnowThickness_A::executeNL starting" << std::endl;

  util::for_each_value(
      [](const double sfc_type, const double swe, double& thickness) {
          thickness = (std::lround(sfc_type) == 3) ? swe : 0.0;
      },
      afieldset.field("gsi_surface_type_index"),
      afieldset.field("gsi_snow_water_equivalent"),
      afieldset.field("surface_snow_thickness"));

  oops::Log::trace() << "SurfaceSnowThickness_A::executeNL done" << std::endl;
}

}  // namespace vader
