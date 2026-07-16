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
#include "vader/betaNames_CRTMRecipes/IceAreaFraction.h"

namespace vader {

const char IceAreaFraction_B::Name[] = "IceAreaFraction_B";
const oops::Variables IceAreaFraction_B::Ingredients{
    std::vector<std::string>{"gsi_surface_type_index", "vtype", "stype"}};

static RecipeMaker<IceAreaFraction_B> makerIceAreaFraction_B_(IceAreaFraction_B::Name);

IceAreaFraction_B::IceAreaFraction_B(const Parameters_ & params,
                                     const VaderConfigVars & configVariables) {
  oops::Log::trace() << "IceAreaFraction_B::IceAreaFraction_B(params)" << std::endl;
}

std::string IceAreaFraction_B::name() const { return IceAreaFraction_B::Name; }

oops::Variable IceAreaFraction_B::product() const {
  return oops::Variable{"ice_area_fraction"};
}

oops::Variables IceAreaFraction_B::ingredients() const { return IceAreaFraction_B::Ingredients; }

size_t IceAreaFraction_B::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace IceAreaFraction_B::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void IceAreaFraction_B::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "IceAreaFraction_B::executeNL starting" << std::endl;

  util::for_each_value(
      [](const double sfc_type,
         const double vtype_r,
         const double stype_r,
         double& ice_frac) {
          const int64_t idx = std::lround(sfc_type);
          if (idx == 2) {
              ice_frac = 1.0;
          } else if (idx == 1) {
              const int64_t vtype = std::lround(vtype_r);
              const int64_t stype = std::lround(stype_r);
              ice_frac = (vtype == 15 || stype == 16) ? 1.0 : 0.0;
          } else {
              ice_frac = 0.0;
          }
      },
      afieldset.field("gsi_surface_type_index"),
      afieldset.field("vtype"),
      afieldset.field("stype"),
      afieldset.field("ice_area_fraction"));

  oops::Log::trace() << "IceAreaFraction_B::executeNL done" << std::endl;
}

}  // namespace vader
