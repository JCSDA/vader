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

class GeopotentialHeightAtSurface_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(GeopotentialHeightAtSurface_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief GeopotentialHeightAtSurface_A class defines a recipe for geopotential_height_at_surface
 *         from geopotential_at_surface
 *
 *         NL:
 *             z_surf(j) = phi_surf(j) / g
 *         TL:
 *             z_surf'(j) = phi_surf'(j) / g
 *         AD:
 *             phi_surf_ad(j) += z_surf_ad(j) / g
 *             z_surf_ad(j) = 0
 *
 *         where:
 *         - phi_surf is geopotential_at_surface (m^2 s^-2)
 *         - g is standard_gravitational_acceleration (m s^-2)
 *         - z_surf is geopotential_height_at_surface (m)
 *         - j indexes horizontal points (0..npoint-1)
 */
class GeopotentialHeightAtSurface_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef GeopotentialHeightAtSurface_A_Parameters Parameters_;

    GeopotentialHeightAtSurface_A(const Parameters_ &, const VaderConfigVars &);

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

}  // namespace vader
