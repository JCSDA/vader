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

class GeopotentialHeight_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(GeopotentialHeight_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief GeopotentialHeight_A class defines a recipe for geopotential_height
 *         from geopotential
 *
 *         NL:
               z(j, k) = phi(j, k) / g
 *         TL:
               z'(j, k) = phi'(j, k) / g
 *         AD:
               phi_ad(j, k) += z_ad(j, k) / g
 *             z_ad(j, k) = 0
 *
 *         where:
 *         - phi is geopotential (m^2 s^-2)
 *         - g is standard_gravitational_acceleration (m s^-2)
 *         - z is geopotential_height (m)
 *         - j is indexes horizontal points
 *         - k is vertical levels
 *
 */
class GeopotentialHeight_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef GeopotentialHeight_A_Parameters Parameters_;

    GeopotentialHeight_A(const Parameters_ &, const VaderConfigVars &);

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

 private:
    const VaderConfigVars & configVariables_;
};

class GeopotentialHeight_B_Parameters
  : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(
      GeopotentialHeight_B_Parameters,
      RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
      "recipe name",
      this};

  oops::Parameter<bool> useEmpiricalFormula{
      "use empirical formula",
      true,
      this};
};

// -------------------------------------------------------------------------------------------------
/*! \brief GeopotentialHeight_B calculates geopotential_height
 *         using hydrostatic integration. By default, the empirical
 *         compressibility correction is applied; it may be disabled
 *         to use the standard hypsometric formula.
 *
 * Ingredients:
 *   air_pressure
 *   air_pressure_levels
 *   air_temperature
 *   water_vapor_mixing_ratio_wrt_dry_air
 *   virtual_temperature
 *   geopotential_height_at_surface
 *
 * This version consumes the dry-air mixing ratio and virtual
 * temperature produced by other Vader recipes.
 *
 * This recipe currently implements the nonlinear transform only.
 */
class GeopotentialHeight_B : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef GeopotentialHeight_B_Parameters Parameters_;

  GeopotentialHeight_B(const Parameters_ &, const VaderConfigVars &);

  std::string name() const override;
  oops::Variable product() const override;
  oops::Variables ingredients() const override;
  size_t productLevels(const atlas::FieldSet &) const override;
  atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
  void executeNL(atlas::FieldSet &) override;

 private:
  const VaderConfigVars & configVariables_;
  const bool useEmpiricalFormula_;
};

}  // namespace vader
