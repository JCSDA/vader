/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/SkinTemperatureAtSurfaceWhereSea.h"

namespace vader {

const char SkinTemperatureAtSurfaceWhereSea_A::Name[] = "SkinTemperatureAtSurfaceWhereSea_A";
const oops::Variables SkinTemperatureAtSurfaceWhereSea_A::Ingredients{
    std::vector<std::string>{"skin_temperature_at_surface"}};

static RecipeMaker<SkinTemperatureAtSurfaceWhereSea_A>
    makerSkinTemperatureAtSurfaceWhereSea_A_(SkinTemperatureAtSurfaceWhereSea_A::Name);

SkinTemperatureAtSurfaceWhereSea_A::SkinTemperatureAtSurfaceWhereSea_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) {
  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereSea_A::SkinTemperatureAtSurfaceWhereSea_A"
                     << std::endl;
  minOceanTemperature_ = params.minOceanTemperature;
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

oops::Variables SkinTemperatureAtSurfaceWhereSea_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"skin_temperature_at_surface"}};
}

size_t SkinTemperatureAtSurfaceWhereSea_A::productLevels(const atlas::FieldSet & afieldset) const
{
  return 1;
}

atlas::FunctionSpace SkinTemperatureAtSurfaceWhereSea_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("skin_temperature_at_surface").functionspace();
}

void SkinTemperatureAtSurfaceWhereSea_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereSea_A::executeNL starting" << std::endl;

  util::for_each_value(
      [this](const double tskin, double& tskin_sea) {
          tskin_sea = std::max(tskin, minOceanTemperature_);
      },
      afieldset.field("skin_temperature_at_surface"),
      afieldset.field("skin_temperature_at_surface_where_sea"));

  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereSea_A::executeNL done" << std::endl;
}

void SkinTemperatureAtSurfaceWhereSea_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereSea_A::executeTL starting" << std::endl;

  util::for_each_value(
      [this](const double tskin, const double tskin_tl, double& tskin_sea_tl) {
          tskin_sea_tl = (tskin >= minOceanTemperature_) ? tskin_tl : 0.0;
      },
      afieldsetTraj.field("skin_temperature_at_surface"),
      afieldsetTL.field("skin_temperature_at_surface"),
      afieldsetTL.field("skin_temperature_at_surface_where_sea"));

  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereSea_A::executeTL done" << std::endl;
}

void SkinTemperatureAtSurfaceWhereSea_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereSea_A::executeAD starting" << std::endl;

  util::for_each_value(
      [this](const double tskin, double& tskin_ad, double& tskin_sea_ad) {
          if (tskin >= minOceanTemperature_) {
              tskin_ad += tskin_sea_ad;
          }
          tskin_sea_ad = 0.0;
      },
      afieldsetTraj.field("skin_temperature_at_surface"),
      afieldsetAD.field("skin_temperature_at_surface"),
      afieldsetAD.field("skin_temperature_at_surface_where_sea"));

  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereSea_A::executeAD done" << std::endl;
}

}  // namespace vader
