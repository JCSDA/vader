/*
 * (C) Copyright 2025 UCAR.
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

namespace vader
{

// -------------------------------------------------------------------------------------------------

class MassContentOfCloudLiquidWaterInAtmosphereLayer_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(MassContentOfCloudLiquidWaterInAtmosphereLayer_AParameters,
                       RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief The class 'MassContentOfCloudLiquidWaterInAtmosphereLayer_A' defines a recipe for
 *         mass content of cloud liquid water in atmosphere layer
 *
 *  \details This instantiation of RecipeBase produces mass content of cloud liquid water
 *           from mixing ratio via geometric conversion.
 *           Inputs: cloud_liquid_water (mixing ratio), air_pressure_thickness
 *           Output units: kg m-2
 */
class MassContentOfCloudLiquidWaterInAtmosphereLayer_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef MassContentOfCloudLiquidWaterInAtmosphereLayer_AParameters Parameters_;

    MassContentOfCloudLiquidWaterInAtmosphereLayer_A(const Parameters_ &, const VaderConfigVars &);

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

// -------------------------------------------------------------------------------------------------

class MassContentOfCloudLiquidWaterInAtmosphereLayer_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(MassContentOfCloudLiquidWaterInAtmosphereLayer_BParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
  // Apply a land/sea mask to the output (matching fv3-jedi crtm_ade_efr "use_mask" option).
  //   "land": zero where slmsk != 0 (i.e. not ocean)
  //   "sea":  zero where slmsk == 0 (i.e. ocean)
  //   "none": no masking (default)
  oops::Parameter<std::string> maskOver{"mask over", std::string("none"), this};
};

/*! \brief 'MassContentOfCloudLiquidWaterInAtmosphereLayer_B' reproduces the fv3-jedi
 *         GSI-flavor mass content calculation from `crtm_ade_efr`.
 *
 *  \details Inputs: cloud_liquid_water (kg/kg), air_pressure_thickness (Pa), slmsk
 *           Output: kg m-2.
 *           - Cells with cloud_liquid_water < 1e-8 are set to 0 (matching min_qx
 *             threshold in `hydro_mixr_to_wpath`).
 *           - The land/sea mask follows the fv3-jedi rule `seamask = slmsk == 0`.
 */
class MassContentOfCloudLiquidWaterInAtmosphereLayer_B : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef MassContentOfCloudLiquidWaterInAtmosphereLayer_BParameters Parameters_;

    MassContentOfCloudLiquidWaterInAtmosphereLayer_B(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;

    bool hasTLAD() const override { return false; }
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
    std::string maskOver_;
};

// -------------------------------------------------------------------------------------------------

}  // namespace vader
