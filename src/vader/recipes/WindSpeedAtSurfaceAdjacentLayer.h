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
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class WindSpeedAtSurfaceAdjacentLayer_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WindSpeedAtSurfaceAdjacentLayer_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief WindSpeedAtSurfaceAdjacentLayer_A computes the wind speed in the surface-adjacent layer
 *
 *  \details Formula: speed = sqrt(u^2 + v^2)
 *           where u and v are eastward_wind_at_surface_adjacent_layer and
 *           northward_wind_at_surface_adjacent_layer (the lowest model level).
 *           Product: wind_speed_at_surface_adjacent_layer.
 *           For the 10 m wind speed see WindSpeedAt10m_A/_B/_C.
 *           Full TL/AD support.
 */
class WindSpeedAtSurfaceAdjacentLayer_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef WindSpeedAtSurfaceAdjacentLayer_A_Parameters Parameters_;

  WindSpeedAtSurfaceAdjacentLayer_A(const Parameters_ &, const VaderConfigVars &);

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
};

}  // namespace vader
