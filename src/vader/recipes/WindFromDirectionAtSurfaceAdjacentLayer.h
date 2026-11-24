/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class WindFromDirectionAtSurfaceAdjacentLayer_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WindFromDirectionAtSurfaceAdjacentLayer_A_Parameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief WindFromDirectionAtSurfaceAdjacentLayer_A computes the meteorological wind-from
 *         direction in the surface-adjacent layer (the lowest model level).
 *
 *  \details Formula: direction = atan2(-u, -v) * radToDeg, normalized to [0, 360).
 *           0 degrees = wind coming from north, 90 = from east.
 *           radToDeg is read from configVariables ("radians_to_degrees").
 *           Ingredients: eastward/northward_wind_at_surface_adjacent_layer.
 *           Product: wind_from_direction_at_surface_adjacent_layer.
 *           Full TL/AD support.
 *           Note that CRTM uses the wind-toward direction (WindToDirectionAt10m_A), not this one.
 */
class WindFromDirectionAtSurfaceAdjacentLayer_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef WindFromDirectionAtSurfaceAdjacentLayer_A_Parameters Parameters_;

  WindFromDirectionAtSurfaceAdjacentLayer_A(const Parameters_ &, const VaderConfigVars &);

  // Recipe base class overrides
  std::string name() const override;
  oops::Variable product() const override;
  oops::Variables ingredients() const override;
  oops::Variables trajectoryVars() const override;
  size_t productLevels(const atlas::FieldSet &) const override;
  atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
  bool hasTLAD() const override { return true; }
  void executeNL(atlas::FieldSet &) override;
  void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
  void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
  double radToDeg_;
};

}  // namespace vader
