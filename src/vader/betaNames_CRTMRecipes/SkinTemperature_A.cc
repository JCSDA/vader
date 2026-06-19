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
#include "vader/betaNames_CRTMRecipes/SkinTemperature.h"

namespace vader {

// Static attribute initialization
const char SkinTemperature_A::Name[] = "SkinTemperature_A";
const oops::Variables SkinTemperature_A::Ingredients{
    std::vector<std::string>{"air_temperature", "sea_surface_temperature",
                             "water_area_fraction", "land_area_fraction",
                             "ice_area_fraction", "surface_snow_area_fraction"}};

// Register the maker
static RecipeMaker<SkinTemperature_A> makerSkinTemperature_A_(
    SkinTemperature_A::Name);

SkinTemperature_A::SkinTemperature_A(const Parameters_ & params,
                                       const VaderConfigVars & configVariables) :
  configVariables_(configVariables) {
  oops::Log::trace() << "SkinTemperature_A::SkinTemperature_A(params)"
                     << std::endl;
}

std::string SkinTemperature_A::name() const {
  return SkinTemperature_A::Name;
}

oops::Variable SkinTemperature_A::product() const {
  return oops::Variable{"skin_temperature"};
}

oops::Variables SkinTemperature_A::ingredients() const {
  return SkinTemperature_A::Ingredients;
}

oops::Variables SkinTemperature_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{
      "water_area_fraction", "land_area_fraction",
      "ice_area_fraction", "surface_snow_area_fraction",
      "air_temperature", "sea_surface_temperature"}};
}

size_t SkinTemperature_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace SkinTemperature_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void SkinTemperature_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SkinTemperature_A::executeNL starting" << std::endl;

  const double T_freeze = 273.15;
  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldset.field("air_temperature").shape(1) - 1 : 0;

  util::for_each_column(
      [&, surfLevel](const auto air_temp_col,
          const auto sst_col,
          const auto water_frac_col,
          const auto land_frac_col,
          const auto ice_frac_col,
          const auto snow_frac_col,
          auto skin_temp_col) {
          double tskin = 0.0;
          if (water_frac_col(0) > 0.0)
            tskin += water_frac_col(0) * sst_col(0);
          if (ice_frac_col(0) > 0.0)
            tskin += ice_frac_col(0) * std::min(sst_col(0), T_freeze);
          if (snow_frac_col(0) > 0.0)
            tskin += snow_frac_col(0) * std::min(air_temp_col(surfLevel), T_freeze);
          if (land_frac_col(0) > 0.0)
            tskin += land_frac_col(0) * air_temp_col(surfLevel);
          skin_temp_col(0) = tskin;
      },
      afieldset.field("air_temperature"),
      afieldset.field("sea_surface_temperature"),
      afieldset.field("water_area_fraction"),
      afieldset.field("land_area_fraction"),
      afieldset.field("ice_area_fraction"),
      afieldset.field("surface_snow_area_fraction"),
      afieldset.field("skin_temperature"));

  oops::Log::trace() << "SkinTemperature_A::executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SkinTemperature_A::executeTL(atlas::FieldSet & afieldsetTL,
                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SkinTemperature_A::executeTL starting" << std::endl;

  const double T_freeze = 273.15;
  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldsetTraj.field("air_temperature").shape(1) - 1 : 0;

  util::for_each_column(
      [&, surfLevel](const auto air_temp_traj_col,
          const auto sst_traj_col,
          const auto water_frac_col,
          const auto land_frac_col,
          const auto ice_frac_col,
          const auto snow_frac_col,
          const auto air_temp_tl_col,
          const auto sst_tl_col,
          auto skin_temp_tl_col) {
          double tskin_tl = 0.0;
          tskin_tl += water_frac_col(0) * sst_tl_col(0);
          if (ice_frac_col(0) > 0.0) {
            if (sst_traj_col(0) < T_freeze) {
              tskin_tl += ice_frac_col(0) * sst_tl_col(0);
            }
          }
          if (snow_frac_col(0) > 0.0) {
            if (air_temp_traj_col(surfLevel) < T_freeze) {
              tskin_tl += snow_frac_col(0) * air_temp_tl_col(surfLevel);
            }
          }
          tskin_tl += land_frac_col(0) * air_temp_tl_col(surfLevel);
          skin_temp_tl_col(0) = tskin_tl;
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetTraj.field("sea_surface_temperature"),
      afieldsetTraj.field("water_area_fraction"),
      afieldsetTraj.field("land_area_fraction"),
      afieldsetTraj.field("ice_area_fraction"),
      afieldsetTraj.field("surface_snow_area_fraction"),
      afieldsetTL.field("air_temperature"),
      afieldsetTL.field("sea_surface_temperature"),
      afieldsetTL.field("skin_temperature"));

  oops::Log::trace() << "SkinTemperature_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void SkinTemperature_A::executeAD(atlas::FieldSet & afieldsetAD,
                                   const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "SkinTemperature_A::executeAD starting" << std::endl;

  const double T_freeze = 273.15;
  const bool topDown = configVariables_.getBool("levels_are_top_down");
  const int surfLevel = topDown ? afieldsetTraj.field("air_temperature").shape(1) - 1 : 0;

  util::for_each_column(
      [&, surfLevel](const auto air_temp_traj_col,
          const auto sst_traj_col,
          const auto water_frac_col,
          const auto land_frac_col,
          const auto ice_frac_col,
          const auto snow_frac_col,
          auto air_temp_ad_col,
          auto sst_ad_col,
          auto skin_temp_ad_col) {
          sst_ad_col(0) += water_frac_col(0) * skin_temp_ad_col(0);
          if (ice_frac_col(0) > 0.0 && sst_traj_col(0) < T_freeze) {
            sst_ad_col(0) += ice_frac_col(0) * skin_temp_ad_col(0);
          }
          if (snow_frac_col(0) > 0.0 && air_temp_traj_col(surfLevel) < T_freeze) {
            air_temp_ad_col(surfLevel) += snow_frac_col(0) * skin_temp_ad_col(0);
          }
          air_temp_ad_col(surfLevel) += land_frac_col(0) * skin_temp_ad_col(0);
          skin_temp_ad_col(0) = 0.0;
      },
      afieldsetTraj.field("air_temperature"),
      afieldsetTraj.field("sea_surface_temperature"),
      afieldsetTraj.field("water_area_fraction"),
      afieldsetTraj.field("land_area_fraction"),
      afieldsetTraj.field("ice_area_fraction"),
      afieldsetTraj.field("surface_snow_area_fraction"),
      afieldsetAD.field("air_temperature"),
      afieldsetAD.field("sea_surface_temperature"),
      afieldsetAD.field("skin_temperature"));

  oops::Log::trace() << "SkinTemperature_A::executeAD done" << std::endl;
}

}  // namespace vader
