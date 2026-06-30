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

class GeopotentialHeightLevels_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(GeopotentialHeightLevels_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

class GeopotentialHeightLevels_B_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(GeopotentialHeightLevels_B_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief GeopotentialHeightLevels_A class defines a recipe for geopotential_height_levels
 *         from geopotential_levels
 *
 *         NL:
 *             z_int(j, k) = phi_int(j, k) / g
 *         TL:
 *             z_int'(j, k) = phi_int'(j, k) / g
 *         AD:
 *             phi_int_ad(j, k) += z_int_ad(j, k) / g
 *             z_int_ad(j, k) = 0
 *
 *         where:
 *         - phi_int is geopotential at interfaces (m^2 s^-2)
 *         - g is standard_gravitational_acceleration (m s^-2)
 *         - z_int is geopotential_height at interfaces (m)
 *         - j is indexes horizontal points (0..npoint-1)
 *         - k is indexes interfaces (0..nint-1)
 */
class GeopotentialHeightLevels_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef GeopotentialHeightLevels_A_Parameters Parameters_;

    GeopotentialHeightLevels_A(const Parameters_ &, const VaderConfigVars &);

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

// ------------------------------------------------------------------------------------------------
/*! \brief GeopotentialHeightLevels_B class defines a recipe for geopotential_height_levels
 *         from geopotential_at_surface_height, virtual_temperature, and ln_air_pressure_at_interface,
 *         assuming hydrostatic balance.
 */
class GeopotentialHeightLevels_B : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef GeopotentialHeightLevels_B_Parameters Parameters_;

    GeopotentialHeightLevels_B(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    oops::Variables trajectoryVars() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return false; }
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

}  // namespace vader
