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
#include "vader/betaNames_CRTMRecipes/SkinTemperatureAtSurfaceWhereIce.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char SkinTemperatureAtSurfaceWhereIce_A::Name[] = "SkinTemperatureAtSurfaceWhereIce_A";
const oops::Variables SkinTemperatureAtSurfaceWhereIce_A::Ingredients{std::vector<std::string>{
                            "skin_temperature"}};

// Register the maker
static RecipeMaker<SkinTemperatureAtSurfaceWhereIce_A> makerSkinTemperatureAtSurfaceWhereIce_A_(
                                SkinTemperatureAtSurfaceWhereIce_A::Name);

SkinTemperatureAtSurfaceWhereIce_A::SkinTemperatureAtSurfaceWhereIce_A(const Parameters_ & params,
                   const VaderConfigVars & configVariables) {
  oops::Log::trace()
    << "SkinTemperatureAtSurfaceWhereIce_A::SkinTemperatureAtSurfaceWhereIce_A(params)"
    << std::endl;
}

std::string SkinTemperatureAtSurfaceWhereIce_A::name() const {
  return SkinTemperatureAtSurfaceWhereIce_A::Name;
}

oops::Variable SkinTemperatureAtSurfaceWhereIce_A::product() const {
  return oops::Variable{"skin_temperature_at_surface_where_ice"};
}

oops::Variables SkinTemperatureAtSurfaceWhereIce_A::ingredients() const {
  return SkinTemperatureAtSurfaceWhereIce_A::Ingredients;
}

size_t SkinTemperatureAtSurfaceWhereIce_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace SkinTemperatureAtSurfaceWhereIce_A::productFunctionSpace(
       const atlas::FieldSet & afieldset) const {
  return afieldset.field("skin_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereIce_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereIce_A::executeNL" << std::endl;

  util::for_each_value(
      [](const double tskin,
         double& tskinice) { tskinice = tskin; },
      afieldset.field("skin_temperature"),
      afieldset.field("skin_temperature_at_surface_where_ice"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereIce_A::executeNL" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereIce_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereIce_A::executeTL" << std::endl;

  util::for_each_value(
      [](const double tskin_tl,
         double& tskinice_tl) { tskinice_tl = tskin_tl; },
      afieldsetTL.field("skin_temperature"),
      afieldsetTL.field("skin_temperature_at_surface_where_ice"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereIce_A::executeTL" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereIce_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereIce_A::executeAD" << std::endl;

  util::for_each_value(
      [](double& tskin_ad,
         double& tskinice_ad) {
          tskin_ad += tskinice_ad;
          tskinice_ad = 0.0;
      },
      afieldsetAD.field("skin_temperature"),
      afieldsetAD.field("skin_temperature_at_surface_where_ice"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereIce_A::executeAD" << std::endl;
}

}  // namespace vader
