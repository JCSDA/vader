/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/WindFromComponents.h"
#include "vader/recipes/WindToDirectionAtSurfaceAdjacentLayer.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WindToDirectionAtSurfaceAdjacentLayer_A::Name[] =
    "WindToDirectionAtSurfaceAdjacentLayer_A";
const oops::Variables WindToDirectionAtSurfaceAdjacentLayer_A::Ingredients{std::vector<std::string>{
                            "eastward_wind_at_surface_adjacent_layer",
                            "northward_wind_at_surface_adjacent_layer"}};

// Register the maker
static RecipeMaker<WindToDirectionAtSurfaceAdjacentLayer_A>
    makerWindToDirectionAtSurfaceAdjacentLayer_A_(WindToDirectionAtSurfaceAdjacentLayer_A::Name);

WindToDirectionAtSurfaceAdjacentLayer_A::WindToDirectionAtSurfaceAdjacentLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables) {
  oops::Log::trace() << "WindToDirectionAtSurfaceAdjacentLayer_A::"
                     << "WindToDirectionAtSurfaceAdjacentLayer_A(params)"
                     << std::endl;
  radToDeg_ = configVariables.getDouble("radians_to_degrees");
}

std::string WindToDirectionAtSurfaceAdjacentLayer_A::name() const {
  return WindToDirectionAtSurfaceAdjacentLayer_A::Name;
}

oops::Variable WindToDirectionAtSurfaceAdjacentLayer_A::product() const {
  return oops::Variable{"wind_to_direction_at_surface_adjacent_layer"};
}

oops::Variables WindToDirectionAtSurfaceAdjacentLayer_A::ingredients() const {
  return WindToDirectionAtSurfaceAdjacentLayer_A::Ingredients;
}

oops::Variables WindToDirectionAtSurfaceAdjacentLayer_A::trajectoryVars() const {
  return WindToDirectionAtSurfaceAdjacentLayer_A::Ingredients;
}

size_t WindToDirectionAtSurfaceAdjacentLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace WindToDirectionAtSurfaceAdjacentLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("eastward_wind_at_surface_adjacent_layer").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WindToDirectionAtSurfaceAdjacentLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "WindToDirectionAtSurfaceAdjacentLayer_A::executeNL starting" << std::endl;
  windToDirectionNL(afieldset["eastward_wind_at_surface_adjacent_layer"],
                    afieldset["northward_wind_at_surface_adjacent_layer"],
                    afieldset["wind_to_direction_at_surface_adjacent_layer"], radToDeg_);
  oops::Log::trace() << "WindToDirectionAtSurfaceAdjacentLayer_A::executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindToDirectionAtSurfaceAdjacentLayer_A::executeTL(atlas::FieldSet & afieldsetTL,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindToDirectionAtSurfaceAdjacentLayer_A::executeTL starting" << std::endl;
  windToDirectionTL(afieldsetTraj["eastward_wind_at_surface_adjacent_layer"],
                    afieldsetTraj["northward_wind_at_surface_adjacent_layer"],
                    afieldsetTL["eastward_wind_at_surface_adjacent_layer"],
                    afieldsetTL["northward_wind_at_surface_adjacent_layer"],
                    afieldsetTL["wind_to_direction_at_surface_adjacent_layer"], radToDeg_);
  oops::Log::trace() << "WindToDirectionAtSurfaceAdjacentLayer_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindToDirectionAtSurfaceAdjacentLayer_A::executeAD(atlas::FieldSet & afieldsetAD,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindToDirectionAtSurfaceAdjacentLayer_A::executeAD starting" << std::endl;
  windToDirectionAD(afieldsetTraj["eastward_wind_at_surface_adjacent_layer"],
                    afieldsetTraj["northward_wind_at_surface_adjacent_layer"],
                    afieldsetAD["eastward_wind_at_surface_adjacent_layer"],
                    afieldsetAD["northward_wind_at_surface_adjacent_layer"],
                    afieldsetAD["wind_to_direction_at_surface_adjacent_layer"], radToDeg_);
  oops::Log::trace() << "WindToDirectionAtSurfaceAdjacentLayer_A::executeAD done" << std::endl;
}

}  // namespace vader
