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

class SkinTemperatureAtSurfaceWhereSnow_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SkinTemperatureAtSurfaceWhereSnow_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
  oops::Parameter<double> maxSnowTemperature{
     "maximum snow temperature",
     "Upper bound on skin temperature for snow surfaces (K).",
     280.0,
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief SkinTemperatureAtSurfaceWhereSnow_A creates skin_temperature_at_surface_where_snow from
 *         skin_temperature_at_surface, with a configurable maximum allowed temperature.
 */
class SkinTemperatureAtSurfaceWhereSnow_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef SkinTemperatureAtSurfaceWhereSnow_A_Parameters Parameters_;

  SkinTemperatureAtSurfaceWhereSnow_A(const Parameters_ &, const VaderConfigVars &);

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
  double maxSnowTemperature_;
};

}  // namespace vader
