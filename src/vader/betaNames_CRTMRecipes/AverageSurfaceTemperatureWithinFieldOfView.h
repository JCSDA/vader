/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class AverageSurfaceTemperatureWithinFieldOfView_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(AverageSurfaceTemperatureWithinFieldOfView_A_Parameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief AverageSurfaceTemperatureWithinFieldOfView_A passes the sea skin temperature through
 *         as the average surface temperature within the field of view.
 *
 *  \details Formula: t_fov = t_sea
 *           where t_sea is skin_temperature_at_surface_where_sea (K),
 *           and t_fov is average_surface_temperature_within_field_of_view (K).
 */
class AverageSurfaceTemperatureWithinFieldOfView_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef AverageSurfaceTemperatureWithinFieldOfView_A_Parameters Parameters_;

  AverageSurfaceTemperatureWithinFieldOfView_A(const Parameters_ &, const VaderConfigVars &);

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
