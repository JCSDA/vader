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

class EffectiveRadiusOfSnowParticle_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(EffectiveRadiusOfSnowParticle_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief The class 'EffectiveRadiusOfSnowParticle_A' defines a recipe for
 *         effective radius of snow particle
 *
 *  \details This instantiation of RecipeBase produces effective radius
 *           using a simplified diagnostic approximation with temperature-dependent
 *           relationship, based on Martin et al. (1994, J. Atmos. Sci., 51, 1823-1842)
 *           Inputs: air_temperature
 *           Output units: m
 */
class EffectiveRadiusOfSnowParticle_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef EffectiveRadiusOfSnowParticle_AParameters Parameters_;

    EffectiveRadiusOfSnowParticle_A(const Parameters_ &, const VaderConfigVars &);

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

}  // namespace vader
