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
#include "vader/betaNames_CRTMRecipes/SkinTemperatureAtSurfaceWhereLand.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char SkinTemperatureAtSurfaceWhereLand_A::Name[] = "SkinTemperatureAtSurfaceWhereLand_A";
const oops::Variables SkinTemperatureAtSurfaceWhereLand_A::Ingredients{std::vector<std::string>{
                            "skin_temperature"}};

// Register the maker
static RecipeMaker<SkinTemperatureAtSurfaceWhereLand_A> makerSkinTemperatureAtSurfaceWhereLand_A_(
                                SkinTemperatureAtSurfaceWhereLand_A::Name);

SkinTemperatureAtSurfaceWhereLand_A::SkinTemperatureAtSurfaceWhereLand_A(const Parameters_ & params,
                   const VaderConfigVars & configVariables) {
  oops::Log::trace()
    << "SkinTemperatureAtSurfaceWhereLand_A::SkinTemperatureAtSurfaceWhereLand_A(params)"
    << std::endl;
}

std::string SkinTemperatureAtSurfaceWhereLand_A::name() const {
  return SkinTemperatureAtSurfaceWhereLand_A::Name;
}

oops::Variable SkinTemperatureAtSurfaceWhereLand_A::product() const {
  return oops::Variable{"skin_temperature_at_surface_where_land"};
}

oops::Variables SkinTemperatureAtSurfaceWhereLand_A::ingredients() const {
  return SkinTemperatureAtSurfaceWhereLand_A::Ingredients;
}

size_t SkinTemperatureAtSurfaceWhereLand_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace SkinTemperatureAtSurfaceWhereLand_A::productFunctionSpace(
       const atlas::FieldSet & afieldset) const {
  return afieldset.field("skin_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereLand_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereLand_A::executeNL" << std::endl;

  util::for_each_value(
      [](const double tskin,
         double& tskinland) { tskinland = tskin; },
      afieldset.field("skin_temperature"),
      afieldset.field("skin_temperature_at_surface_where_land"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereLand_A::executeNL" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereLand_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereLand_A::executeTL" << std::endl;

  util::for_each_value(
      [](const double tskin_tl,
         double& tskinland_tl) { tskinland_tl = tskin_tl; },
      afieldsetTL.field("skin_temperature"),
      afieldsetTL.field("skin_temperature_at_surface_where_land"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereLand_A::executeTL" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void SkinTemperatureAtSurfaceWhereLand_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "entering SkinTemperatureAtSurfaceWhereLand_A::executeAD" << std::endl;

  util::for_each_value(
      [](double& tskin_ad,
         double& tskinland_ad) {
          tskin_ad += tskinland_ad;
          tskinland_ad = 0.0;
      },
      afieldsetAD.field("skin_temperature"),
      afieldsetAD.field("skin_temperature_at_surface_where_land"));

  oops::Log::trace() << "leaving SkinTemperatureAtSurfaceWhereLand_A::executeAD" << std::endl;
}

}  // namespace vader
