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

class WindSpeedAt10m_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WindSpeedAt10m_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief WindSpeedAt10m_A mirrors the fv3-jedi crtm_surface wind_speed helper.
 *
 *  \details Formula: speed = f10m * sqrt(u^2 + v^2), where f10m is the wind reduction factor at
 *           10m, and u,v are the surface adjacent values for eastward/northward wind components.
 */
class WindSpeedAt10m_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef WindSpeedAt10m_A_Parameters Parameters_;

  WindSpeedAt10m_A(const Parameters_ &, const VaderConfigVars &);

  std::string name() const override;
  oops::Variable product() const override;
  oops::Variables ingredients() const override;
  size_t productLevels(const atlas::FieldSet &) const override;
  atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
  void executeNL(atlas::FieldSet &) override;

 private:
  const VaderConfigVars & configVariables_;
};
// -------------------------------------------------------------------------------------------------

/*! \brief WindSpeedAt10m_B mirrors the fv3-jedi crtm_surface wind_speed helper.
 *
 *  \details Formula: speed = f10m * sqrt(u^2 + v^2), where f10m is the wind reduction factor at
 *           10m, and u,v are eastward/northward wind components at surface.
 */
class WindSpeedAt10m_B_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WindSpeedAt10m_B_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

class WindSpeedAt10m_B : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef WindSpeedAt10m_B_Parameters Parameters_;

  WindSpeedAt10m_B(const Parameters_ &, const VaderConfigVars &);

  std::string name() const override;
  oops::Variable product() const override;
  oops::Variables ingredients() const override;
  size_t productLevels(const atlas::FieldSet &) const override;
  atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
  void executeNL(atlas::FieldSet &) override;
};

}  // namespace vader
