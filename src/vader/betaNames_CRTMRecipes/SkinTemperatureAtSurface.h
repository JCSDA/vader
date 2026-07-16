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

class SkinTemperatureAtSurface_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SkinTemperatureAtSurface_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief SkinTemperatureAtSurface_A class defines an area-fraction-based recipe for skin
 *         temperature at the surface
 *
 *  \details This instantiation of RecipeBase produces skin_temperature_at_surface
 *           using a weighted blend based on surface type area fractions:
 *           - water_area_fraction: uses SST
 *           - ice_area_fraction: uses SST capped at 273.15 K
 *           - surface_snow_area_fraction: uses air temperature capped at 273.15 K
 *           - land_area_fraction: uses air temperature
 *           Formula: Tskin = water_frac*SST + ice_frac*min(SST,273.15) +
 *                           snow_frac*min(Tair,273.15) + land_frac*Tair
 */
class SkinTemperatureAtSurface_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef SkinTemperatureAtSurface_AParameters Parameters_;

    SkinTemperatureAtSurface_A(const Parameters_ &, const VaderConfigVars &);

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
    const VaderConfigVars & configVariables_;
};

}  // namespace vader
