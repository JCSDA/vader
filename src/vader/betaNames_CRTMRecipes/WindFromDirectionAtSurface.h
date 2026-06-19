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

class WindFromDirectionAtSurface_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WindFromDirectionAtSurface_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief WindFromDirectionAtSurface_A computes wind direction at surface
 *
 *  \details Formula: direction = atan2(-u, -v) * 180/pi
 *           where u is eastward_wind, v is northward_wind
 *           Result is in degrees (0-360), where 0 is North, 90 is East
 *           Full TL/AD support.
 */
class WindFromDirectionAtSurface_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef WindFromDirectionAtSurface_A_Parameters Parameters_;

  WindFromDirectionAtSurface_A(const Parameters_ &, const VaderConfigVars &);

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
