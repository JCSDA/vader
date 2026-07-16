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

class WindToDirectionAtSurface_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WindToDirectionAtSurface_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief WindToDirectionAtSurface_A computes the wind-toward direction at surface.
 *
 *  \details Formula: direction = atan2(u, v) * radToDeg, normalized to [0, 360).
 *           0 degrees = wind blowing toward north, 90 = toward east.
 *           This is the CRTM convention (south-to-north wind = 0 degrees).
 *           radToDeg is read from configVariables ("radians_to_degrees").
 *           Full TL/AD support.
 */
class WindToDirectionAtSurface_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef WindToDirectionAtSurface_A_Parameters Parameters_;

  WindToDirectionAtSurface_A(const Parameters_ &, const VaderConfigVars &);

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
