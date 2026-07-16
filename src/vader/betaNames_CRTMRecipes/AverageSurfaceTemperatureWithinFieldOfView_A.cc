/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/AverageSurfaceTemperatureWithinFieldOfView.h"

namespace vader {

const char AverageSurfaceTemperatureWithinFieldOfView_A::Name[] =
    "AverageSurfaceTemperatureWithinFieldOfView_A";
const oops::Variables AverageSurfaceTemperatureWithinFieldOfView_A::Ingredients{
    std::vector<std::string>{"skin_temperature_at_surface_where_sea"}};

static RecipeMaker<AverageSurfaceTemperatureWithinFieldOfView_A>
    makerAverageSurfaceTemperatureWithinFieldOfView_A_(
        AverageSurfaceTemperatureWithinFieldOfView_A::Name);

AverageSurfaceTemperatureWithinFieldOfView_A::AverageSurfaceTemperatureWithinFieldOfView_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) {
  oops::Log::trace() << "AverageSurfaceTemperatureWithinFieldOfView_A::"
                        "AverageSurfaceTemperatureWithinFieldOfView_A(params)" << std::endl;
}

std::string AverageSurfaceTemperatureWithinFieldOfView_A::name() const {
  return AverageSurfaceTemperatureWithinFieldOfView_A::Name;
}

oops::Variable AverageSurfaceTemperatureWithinFieldOfView_A::product() const {
  return oops::Variable{"average_surface_temperature_within_field_of_view"};
}

oops::Variables AverageSurfaceTemperatureWithinFieldOfView_A::ingredients() const {
  return AverageSurfaceTemperatureWithinFieldOfView_A::Ingredients;
}

size_t AverageSurfaceTemperatureWithinFieldOfView_A::productLevels(
    const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace AverageSurfaceTemperatureWithinFieldOfView_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("skin_temperature_at_surface_where_sea").functionspace();
}

void AverageSurfaceTemperatureWithinFieldOfView_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "AverageSurfaceTemperatureWithinFieldOfView_A::executeNL starting"
                     << std::endl;

  util::for_each_value(
      [](const double t_sea, double& t_fov) { t_fov = t_sea; },
      afieldset.field("skin_temperature_at_surface_where_sea"),
      afieldset.field("average_surface_temperature_within_field_of_view"));

  oops::Log::trace() << "AverageSurfaceTemperatureWithinFieldOfView_A::executeNL done" << std::endl;
}

void AverageSurfaceTemperatureWithinFieldOfView_A::executeTL(
    atlas::FieldSet & afieldsetTL, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "AverageSurfaceTemperatureWithinFieldOfView_A::executeTL starting"
                     << std::endl;

  util::for_each_value(
      [](const double t_sea_tl, double& t_fov_tl) { t_fov_tl = t_sea_tl; },
      afieldsetTL.field("skin_temperature_at_surface_where_sea"),
      afieldsetTL.field("average_surface_temperature_within_field_of_view"));

  oops::Log::trace() << "AverageSurfaceTemperatureWithinFieldOfView_A::executeTL done" << std::endl;
}

void AverageSurfaceTemperatureWithinFieldOfView_A::executeAD(
    atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "AverageSurfaceTemperatureWithinFieldOfView_A::executeAD starting"
                     << std::endl;

  util::for_each_value(
      [](double& t_sea_ad, double& t_fov_ad) {
          t_sea_ad += t_fov_ad;
          t_fov_ad = 0.0;
      },
      afieldsetAD.field("skin_temperature_at_surface_where_sea"),
      afieldsetAD.field("average_surface_temperature_within_field_of_view"));

  oops::Log::trace() << "AverageSurfaceTemperatureWithinFieldOfView_A::executeAD done" << std::endl;
}

}  // namespace vader
