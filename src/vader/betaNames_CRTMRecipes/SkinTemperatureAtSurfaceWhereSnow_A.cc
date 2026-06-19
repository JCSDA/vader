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
#include "vader/betaNames_CRTMRecipes/SkinTemperatureAtSurfaceWhereSnow.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char SkinTemperatureAtSurfaceWhereSnow_A::Name[] = "SkinTemperatureAtSurfaceWhereSnow_A";
const oops::Variables SkinTemperatureAtSurfaceWhereSnow_A::Ingredients{std::vector<std::string>{
                            "skin_temperature"}};

// Register the maker
static RecipeMaker<SkinTemperatureAtSurfaceWhereSnow_A> makerSkinTemperatureAtSurfaceWhereSnow_A_(
                                SkinTemperatureAtSurfaceWhereSnow_A::Name);

SkinTemperatureAtSurfaceWhereSnow_A::SkinTemperatureAtSurfaceWhereSnow_A(const Parameters_ & params,
                   const VaderConfigVars & configVariables) {
  oops::Log::trace()
    << "SkinTemperatureAtSurfaceWhereSnow_A::SkinTemperatureAtSurfaceWhereSnow_A(params)"
    << std::endl;
}

std::string SkinTemperatureAtSurfaceWhereSnow_A::name() const {
  return SkinTemperatureAtSurfaceWhereSnow_A::Name;
}

oops::Variable SkinTemperatureAtSurfaceWhereSnow_A::product() const {
  return oops::Variable{"skin_temperature_at_surface_where_snow"};
}

oops::Variables SkinTemperatureAtSurfaceWhereSnow_A::ingredients() const {
  return SkinTemperatureAtSurfaceWhereSnow_A::Ingredients;
}

size_t SkinTemperatureAtSurfaceWhereSnow_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace SkinTemperatureAtSurfaceWhereSnow_A::productFunctionSpace(
       const atlas::FieldSet & afieldset) const {
  return afieldset.field("skin_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereSnow_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereSnow_A::executeNL" << std::endl;

  util::for_each_value(
      [](const double tskin,
         double& tskinsnow) { tskinsnow = tskin; },
      afieldset.field("skin_temperature"),
      afieldset.field("skin_temperature_at_surface_where_snow"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereSnow_A::executeNL" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereSnow_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereSnow_A::executeTL" << std::endl;

  util::for_each_value(
      [](const double tskin_tl,
         double& tskinsnow_tl) { tskinsnow_tl = tskin_tl; },
      afieldsetTL.field("skin_temperature"),
      afieldsetTL.field("skin_temperature_at_surface_where_snow"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereSnow_A::executeTL" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereSnow_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereSnow_A::executeAD" << std::endl;

  util::for_each_value(
      [](double& tskin_ad,
         double& tskinsnow_ad) {
          tskin_ad += tskinsnow_ad;
          tskinsnow_ad = 0.0;
      },
      afieldsetAD.field("skin_temperature"),
      afieldsetAD.field("skin_temperature_at_surface_where_snow"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereSnow_A::executeAD" << std::endl;
}

}  // namespace vader
