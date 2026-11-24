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
#include "vader/recipes/WindFromComponents.h"
#include "vader/recipes/WindSpeedAtSurfaceAdjacentLayer.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WindSpeedAtSurfaceAdjacentLayer_A::Name[] =
    "WindSpeedAtSurfaceAdjacentLayer_A";
const oops::Variables WindSpeedAtSurfaceAdjacentLayer_A::Ingredients{std::vector<std::string>{
                            "eastward_wind_at_surface_adjacent_layer",
                            "northward_wind_at_surface_adjacent_layer"}};

// Register the maker
static RecipeMaker<WindSpeedAtSurfaceAdjacentLayer_A>
    makerWindSpeedAtSurfaceAdjacentLayer_A_(WindSpeedAtSurfaceAdjacentLayer_A::Name);

WindSpeedAtSurfaceAdjacentLayer_A::WindSpeedAtSurfaceAdjacentLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) {
  oops::Log::trace() << "WindSpeedAtSurfaceAdjacentLayer_A::"
                     << "WindSpeedAtSurfaceAdjacentLayer_A(params)"
                     << std::endl;
}

std::string WindSpeedAtSurfaceAdjacentLayer_A::name() const {
  return WindSpeedAtSurfaceAdjacentLayer_A::Name;
}

oops::Variable WindSpeedAtSurfaceAdjacentLayer_A::product() const {
  return oops::Variable{"wind_speed_at_surface_adjacent_layer"};
}

oops::Variables WindSpeedAtSurfaceAdjacentLayer_A::ingredients() const {
  return WindSpeedAtSurfaceAdjacentLayer_A::Ingredients;
}

oops::Variables WindSpeedAtSurfaceAdjacentLayer_A::trajectoryVars() const {
  return WindSpeedAtSurfaceAdjacentLayer_A::Ingredients;
}

size_t WindSpeedAtSurfaceAdjacentLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace WindSpeedAtSurfaceAdjacentLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("eastward_wind_at_surface_adjacent_layer").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAtSurfaceAdjacentLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "WindSpeedAtSurfaceAdjacentLayer_A::executeNL starting" << std::endl;
  windSpeedNL(afieldset["eastward_wind_at_surface_adjacent_layer"],
              afieldset["northward_wind_at_surface_adjacent_layer"],
              afieldset["wind_speed_at_surface_adjacent_layer"]);
  oops::Log::trace() << "WindSpeedAtSurfaceAdjacentLayer_A::executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAtSurfaceAdjacentLayer_A::executeTL(atlas::FieldSet & afieldsetTL,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindSpeedAtSurfaceAdjacentLayer_A::executeTL starting" << std::endl;
  windSpeedTL(afieldsetTraj["eastward_wind_at_surface_adjacent_layer"],
              afieldsetTraj["northward_wind_at_surface_adjacent_layer"],
              afieldsetTL["eastward_wind_at_surface_adjacent_layer"],
              afieldsetTL["northward_wind_at_surface_adjacent_layer"],
              afieldsetTL["wind_speed_at_surface_adjacent_layer"]);
  oops::Log::trace() << "WindSpeedAtSurfaceAdjacentLayer_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAtSurfaceAdjacentLayer_A::executeAD(atlas::FieldSet & afieldsetAD,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindSpeedAtSurfaceAdjacentLayer_A::executeAD starting" << std::endl;
  windSpeedAD(afieldsetTraj["eastward_wind_at_surface_adjacent_layer"],
              afieldsetTraj["northward_wind_at_surface_adjacent_layer"],
              afieldsetAD["eastward_wind_at_surface_adjacent_layer"],
              afieldsetAD["northward_wind_at_surface_adjacent_layer"],
              afieldsetAD["wind_speed_at_surface_adjacent_layer"]);
  oops::Log::trace() << "WindSpeedAtSurfaceAdjacentLayer_A::executeAD done" << std::endl;
}

}  // namespace vader
