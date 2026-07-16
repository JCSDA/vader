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

class SkinTemperatureAtSurfaceWhereSea_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SkinTemperatureAtSurfaceWhereSea_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
  oops::Parameter<double> minOceanTemperature{
     "minimum ocean temperature",
     "Lower bound on skin temperature for ocean surfaces (K).",
     270.0,
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief SkinTemperatureAtSurfaceWhereSea_A computes skin temperature at surface where sea
 *
 *  \details Formula: tskin_sea = tskin
 *           where tskin is skin_temperature_at_surface (K),
 *           and tskin_sea is skin_temperature_at_surface_where_sea (K).
 *           Lower bound on skin temperature for ocean surfaces defaults to 270 K.
 */
class SkinTemperatureAtSurfaceWhereSea_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef SkinTemperatureAtSurfaceWhereSea_A_Parameters Parameters_;

  SkinTemperatureAtSurfaceWhereSea_A(const Parameters_ &, const VaderConfigVars &);

  // Recipe base class overrides
  std::string name() const override;
  oops::Variable product() const override;
  oops::Variables ingredients() const override;
  size_t productLevels(const atlas::FieldSet &) const override;
  oops::Variables trajectoryVars() const override;
  atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
  bool hasTLAD() const override { return true; }
  void executeNL(atlas::FieldSet &) override;
  void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
  void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
  double minOceanTemperature_;
};

}  // namespace vader
