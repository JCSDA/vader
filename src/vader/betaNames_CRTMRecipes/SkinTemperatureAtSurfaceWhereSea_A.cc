/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/SkinTemperatureAtSurfaceWhereSea.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char SkinTemperatureAtSurfaceWhereSea_A::Name[] = "SkinTemperatureAtSurfaceWhereSea_A";
const oops::Variables SkinTemperatureAtSurfaceWhereSea_A::Ingredients{std::vector<std::string>{
                            "skin_temperature"}};

// Register the maker
static RecipeMaker<SkinTemperatureAtSurfaceWhereSea_A> makerSkinTemperatureAtSurfaceWhereSea_A_(
                                SkinTemperatureAtSurfaceWhereSea_A::Name);

SkinTemperatureAtSurfaceWhereSea_A::SkinTemperatureAtSurfaceWhereSea_A(const Parameters_ & params,
                   const VaderConfigVars & configVariables) {
  oops::Log::trace()
    << "SkinTemperatureAtSurfaceWhereSea_A::SkinTemperatureAtSurfaceWhereSea_A(params)"
    << std::endl;
}

std::string SkinTemperatureAtSurfaceWhereSea_A::name() const {
  return SkinTemperatureAtSurfaceWhereSea_A::Name;
}

oops::Variable SkinTemperatureAtSurfaceWhereSea_A::product() const {
  return oops::Variable{"skin_temperature_at_surface_where_sea"};
}

oops::Variables SkinTemperatureAtSurfaceWhereSea_A::ingredients() const {
  return SkinTemperatureAtSurfaceWhereSea_A::Ingredients;
}

size_t SkinTemperatureAtSurfaceWhereSea_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace SkinTemperatureAtSurfaceWhereSea_A::productFunctionSpace(
       const atlas::FieldSet & afieldset) const {
  return afieldset.field("skin_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereSea_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereSea_A::executeNL" << std::endl;

  util::for_each_value(
      [](const double tskin,
         double& tskinsea) { tskinsea = tskin; },
      afieldset.field("skin_temperature"),
      afieldset.field("skin_temperature_at_surface_where_sea"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereSea_A::executeNL" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereSea_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereSea_A::executeTL" << std::endl;

  util::for_each_value(
      [](const double tskin_tl,
         double& tskinsea_tl) { tskinsea_tl = tskin_tl; },
      afieldsetTL.field("skin_temperature"),
      afieldsetTL.field("skin_temperature_at_surface_where_sea"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereSea_A::executeTL" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereSea_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereSea_A::executeAD" << std::endl;

  util::for_each_value(
      [](double& tskin_ad,
         double& tskinsea_ad) {
          tskin_ad += tskinsea_ad;
          tskinsea_ad = 0.0;
      },
      afieldsetAD.field("skin_temperature"),
      afieldsetAD.field("skin_temperature_at_surface_where_sea"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereSea_A::executeAD" << std::endl;
}

}  // namespace vader
