/*
 * (C) Copyright 2025 UCAR
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

class EffectiveRadiusOfCloudIceParticle_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(EffectiveRadiusOfCloudIceParticle_AParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief The class 'EffectiveRadiusOfCloudIceParticle_A' defines a recipe for
 *         effective radius of ice particle
 *
 *  \details This instantiation of RecipeBase produces effective radius
 *           using a simplified diagnostic approximation with temperature-dependent
 *           relationship, based on Martin et al. (1994, J. Atmos. Sci., 51, 1823-1842)
 *           Inputs: air_temperature
 *           Output units: m
 */
class EffectiveRadiusOfCloudIceParticle_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef EffectiveRadiusOfCloudIceParticle_AParameters Parameters_;

    EffectiveRadiusOfCloudIceParticle_A(const Parameters_ &, const VaderConfigVars &);

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

class EffectiveRadiusOfCloudIceParticle_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(EffectiveRadiusOfCloudIceParticle_BParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief 'EffectiveRadiusOfCloudIceParticle_B' reproduces the fv3-jedi
 *         GSI-flavor cloud-ice effective radius from `crtm_ade_efr`.
 *
 *  \details Inputs: air_temperature (K), cloud_liquid_ice (kg/kg),
 *           air_pressure (Pa), water_vapor_mixing_ratio_wrt_moist_air (kg/kg).
 *           Output units: microns. Computed only where qi >= 1e-8 (else 0).
 *           rho_air = p / (rdry * T * (1 + zvir * max(q, 0)))
 *           wcontent = qi * rho_air
 *           T-banded power law:
 *             T - tice < -50: (1250/9.917) * wcontent^0.109
 *             T - tice < -40: (1250/9.337) * wcontent^0.08
 *             T - tice < -30: (1250/9.208) * wcontent^0.055
 *             else:           (1250/9.387) * wcontent^0.031
 *             qi_efr = max(5, qi_efr)
 */
class EffectiveRadiusOfCloudIceParticle_B : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef EffectiveRadiusOfCloudIceParticle_BParameters Parameters_;

    EffectiveRadiusOfCloudIceParticle_B(const Parameters_ &, const VaderConfigVars &);

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
