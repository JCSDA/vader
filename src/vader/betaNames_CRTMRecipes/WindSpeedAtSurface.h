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

class WindSpeedAtSurface_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WindSpeedAtSurface_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief WindSpeedAtSurface_A computes wind speed at surface
 *
 *  \details Formula: speed = sqrt(u^2 + v^2)
 *           where u is eastward wind component and v is northward wind component.
 *           Full TL/AD support.
 */
class WindSpeedAtSurface_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef WindSpeedAtSurface_A_Parameters Parameters_;

  WindSpeedAtSurface_A(const Parameters_ &, const VaderConfigVars &);

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

class WindSpeedAtSurface_B_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WindSpeedAtSurface_B_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

}  // namespace vader
