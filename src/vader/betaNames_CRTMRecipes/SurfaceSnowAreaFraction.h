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

class SurfaceSnowAreaFraction_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SurfaceSnowAreaFraction_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief SurfaceSnowAreaFraction_A class defines a recipe for surface snow area fraction
 *
 *  \details This instantiation of RecipeBase produces surface_snow_area_fraction
 *           using landmask and surface_snow_thickness as input.
 *           Formula: surface_snow_area_fraction = landmask * (surface_snow_thickness > 0 ? 1 : 0)
 */
class SurfaceSnowAreaFraction_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef SurfaceSnowAreaFraction_AParameters Parameters_;

    SurfaceSnowAreaFraction_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    oops::Variables trajectoryVars() const override;
    void executeNL(atlas::FieldSet &) override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;
};

}  // namespace vader
