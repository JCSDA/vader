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

class EffectiveRadiusOfCloudLiquidWaterParticle_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(EffectiveRadiusOfCloudLiquidWaterParticle_AParameters,
                       RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief The class 'EffectiveRadiusOfCloudLiquidWaterParticle_A' defines a recipe for
 *         effective radius of cloud liquid water particle
 *
 *  \details This instantiation of RecipeBase produces effective radius
 *           using a simplified diagnostic approximation with temperature-dependent
 *           relationship, based on Martin et al. (1994, J. Atmos. Sci., 51, 1823-1842)
 *           Inputs: air_temperature
 *           Output units: m
 */
class EffectiveRadiusOfCloudLiquidWaterParticle_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef EffectiveRadiusOfCloudLiquidWaterParticle_AParameters Parameters_;

    EffectiveRadiusOfCloudLiquidWaterParticle_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;

    bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    oops::Variables trajectoryVars() const override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

// -------------------------------------------------------------------------------------------------

class EffectiveRadiusOfCloudLiquidWaterParticle_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(EffectiveRadiusOfCloudLiquidWaterParticle_BParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief 'EffectiveRadiusOfCloudLiquidWaterParticle_B' reproduces the fv3-jedi
 *         GSI-flavor cloud-water effective radius from `crtm_ade_efr`.
 *
 *  \details Inputs: air_temperature (K), cloud_liquid_water (kg/kg)
 *           Output units: microns. Computed only where ql >= 1e-8 (else 0).
 *           Formula: reff = max(1, 5 + 5*min(1, (tice - T)*0.05))
 */
class EffectiveRadiusOfCloudLiquidWaterParticle_B : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef EffectiveRadiusOfCloudLiquidWaterParticle_BParameters Parameters_;

    EffectiveRadiusOfCloudLiquidWaterParticle_B(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;

    bool hasTLAD() const override { return false; }
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

// -------------------------------------------------------------------------------------------------

}  // namespace vader
