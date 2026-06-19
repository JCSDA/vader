/*
 * (C) Copyright 2025 UCAR
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

class SkinTemperatureAtSurfaceWhereIce_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SkinTemperatureAtSurfaceWhereIce_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief SkinTemperatureAtSurfaceWhereIce_A computes skin temperature at surface where ice
 *
 *  \details Formula: tskin_ice = tskin
 *           where tskin is skin_temperature_at_surface (K),
 *           and tskin_ice is skin_temperature_at_surface_where_ice (K).
 */
class SkinTemperatureAtSurfaceWhereIce_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef SkinTemperatureAtSurfaceWhereIce_A_Parameters Parameters_;

  SkinTemperatureAtSurfaceWhereIce_A(const Parameters_ &, const VaderConfigVars &);

  // Recipe base class overrides
  std::string name() const override;
  oops::Variable product() const override;
  oops::Variables ingredients() const override;
  size_t productLevels(const atlas::FieldSet &) const override;
  atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
  bool hasTLAD() const override { return true; }
  void executeNL(atlas::FieldSet &) override;
  void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
  void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;
};

}  // namespace vader
