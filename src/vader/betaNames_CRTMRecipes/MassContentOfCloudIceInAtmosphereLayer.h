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

class MassContentOfCloudIceInAtmosphereLayer_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(MassContentOfCloudIceInAtmosphereLayer_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief The class 'MassContentOfCloudIceInAtmosphereLayer_A' defines a recipe for
 *         mass content of cloud ice in atmosphere layer
 *
 *  \details This instantiation of RecipeBase produces mass content of cloud ice
 *           from mixing ratio via geometric conversion.
 *           Inputs: cloud_ice (mixing ratio), air_pressure_thickness
 *           Output units: kg m-2
 */
class MassContentOfCloudIceInAtmosphereLayer_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef MassContentOfCloudIceInAtmosphereLayer_AParameters Parameters_;

    MassContentOfCloudIceInAtmosphereLayer_A(const Parameters_ &, const VaderConfigVars &);

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

class MassContentOfCloudIceInAtmosphereLayer_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(MassContentOfCloudIceInAtmosphereLayer_BParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
  // Apply a land/sea mask to the output (matching fv3-jedi crtm_ade_efr "use_mask" option).
  //   "land": zero where slmsk != 0 (i.e. not ocean)
  //   "sea":  zero where slmsk == 0 (i.e. ocean)
  //   "none": no masking (default)
  oops::Parameter<std::string> maskOver{"mask over", std::string("none"), this};
};

/*! \brief 'MassContentOfCloudIceInAtmosphereLayer_B' reproduces the fv3-jedi
 *         GSI-flavor mass content calculation from `crtm_ade_efr`.
 *
 *  \details Inputs: cloud_liquid_ice (kg/kg), air_pressure_thickness (Pa), slmsk
 *           Output: kg m-2 with `min_qx = 1e-8` threshold and optional land/sea mask.
 */
class MassContentOfCloudIceInAtmosphereLayer_B : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef MassContentOfCloudIceInAtmosphereLayer_BParameters Parameters_;

    MassContentOfCloudIceInAtmosphereLayer_B(const Parameters_ &, const VaderConfigVars &);

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
