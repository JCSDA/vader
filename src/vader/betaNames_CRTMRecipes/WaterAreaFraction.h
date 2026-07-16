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

class WaterAreaFraction_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WaterAreaFraction_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief WaterAreaFraction_A class defines a recipe for water area fraction
 *
 *  \details This instantiation of RecipeBase produces water_area_fraction
 *           using landmask and seaice_fraction as input.
 *           Formula: water_area_fraction = (1 - landmask) * (1 - seaice_fraction)
 */
class WaterAreaFraction_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef WaterAreaFraction_AParameters Parameters_;

    WaterAreaFraction_A(const Parameters_ &, const VaderConfigVars &);

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

class WaterAreaFraction_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WaterAreaFraction_BParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief WaterAreaFraction_B mirrors the fv3-jedi crtm_surface water-coverage helper.
 *
 *  \details Produces water_area_fraction = 1 where gsi_surface_type_index == 0 (sea), else 0.
 */
class WaterAreaFraction_B : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef WaterAreaFraction_BParameters Parameters_;

    WaterAreaFraction_B(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;
};

}  // namespace vader
