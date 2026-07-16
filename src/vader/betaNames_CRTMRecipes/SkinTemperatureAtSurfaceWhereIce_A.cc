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
#include "vader/betaNames_CRTMRecipes/SkinTemperatureAtSurfaceWhereIce.h"

namespace vader {

const char SkinTemperatureAtSurfaceWhereIce_A::Name[] = "SkinTemperatureAtSurfaceWhereIce_A";
const oops::Variables SkinTemperatureAtSurfaceWhereIce_A::Ingredients{
    std::vector<std::string>{"skin_temperature_at_surface"}};

static RecipeMaker<SkinTemperatureAtSurfaceWhereIce_A>
    makerSkinTemperatureAtSurfaceWhereIce_A_(SkinTemperatureAtSurfaceWhereIce_A::Name);

SkinTemperatureAtSurfaceWhereIce_A::SkinTemperatureAtSurfaceWhereIce_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) {
  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereIce_A::SkinTemperatureAtSurfaceWhereIce_A"
                     << std::endl;
  maxIceTemperature_ = params.maxIceTemperature;
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

oops::Variables SkinTemperatureAtSurfaceWhereIce_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"skin_temperature_at_surface"}};
}

size_t SkinTemperatureAtSurfaceWhereIce_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace SkinTemperatureAtSurfaceWhereIce_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("skin_temperature_at_surface").functionspace();
}

void SkinTemperatureAtSurfaceWhereIce_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereIce_A::executeNL starting" << std::endl;

  util::for_each_value(
      [this](const double tskin, double& tskin_ice) {
          tskin_ice = std::min(tskin, maxIceTemperature_);
      },
      afieldset.field("skin_temperature_at_surface"),
      afieldset.field("skin_temperature_at_surface_where_ice"));

  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereIce_A::executeNL done" << std::endl;
}

void SkinTemperatureAtSurfaceWhereIce_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereIce_A::executeTL starting" << std::endl;

  util::for_each_value(
      [this](const double tskin, const double tskin_tl, double& tskin_ice_tl) {
          tskin_ice_tl = (tskin <= maxIceTemperature_) ? tskin_tl : 0.0;
      },
      afieldsetTraj.field("skin_temperature_at_surface"),
      afieldsetTL.field("skin_temperature_at_surface"),
      afieldsetTL.field("skin_temperature_at_surface_where_ice"));

  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereIce_A::executeTL done" << std::endl;
}

void SkinTemperatureAtSurfaceWhereIce_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereIce_A::executeAD starting" << std::endl;

  util::for_each_value(
      [this](const double tskin, double& tskin_ad, double& tskin_ice_ad) {
          if (tskin <= maxIceTemperature_) {
              tskin_ad += tskin_ice_ad;
          }
          tskin_ice_ad = 0.0;
      },
      afieldsetTraj.field("skin_temperature_at_surface"),
      afieldsetAD.field("skin_temperature_at_surface"),
      afieldsetAD.field("skin_temperature_at_surface_where_ice"));

  oops::Log::trace() << "SkinTemperatureAtSurfaceWhereIce_A::executeAD done" << std::endl;
}

}  // namespace vader
