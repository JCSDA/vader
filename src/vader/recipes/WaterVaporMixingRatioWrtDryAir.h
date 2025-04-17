/*
 * (C) Crown Copyright 2025 Met Office.
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

class WaterVaporMixingRatioWrtDryAir_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WaterVaporMixingRatioWrtDryAir_AParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

class WaterVaporMixingRatioWrtDryAir_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WaterVaporMixingRatioWrtDryAir_BParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief WaterVaporMixingRatioWrtDryAir_A class defines a recipe for water
 *         vapor mixing ratio wrt dry air
 *
 *  \details This instantiation of RecipeBase produces the water vapor mixing ratio wrt dry air
 *          increment using the increment of water vapor mixing ratio wrt moist air
 *          and condensed water
 *          The trajectory uses quantities that involve mixing ratios wrt dry air
 */
class WaterVaporMixingRatioWrtDryAir_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef WaterVaporMixingRatioWrtDryAir_AParameters Parameters_;

    WaterVaporMixingRatioWrtDryAir_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    oops::Variables trajectoryVars() const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    bool hasNL() const override { return false; }
    void executeNL(atlas::FieldSet &) override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};

// ------------------------------------------------------------------------------------------------
/*! \brief WaterVaporMixingRatioWrtDryAir_B class defines a recipe for water
 *         vapor mixing ratio wrt dry air
 *
 *  \details This instantiation of RecipeBase produces the water vapor mixing ratio wrt dry air
 *          increment using the increment of water vapor mixing ratio wrt moist air
 *          and condensed water
 *          The trajectory uses quantities that involve mixing ratios wrt moist air
 *          and condensed water.
 *          This is the more standard recipe
 */
class WaterVaporMixingRatioWrtDryAir_B : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef WaterVaporMixingRatioWrtDryAir_BParameters Parameters_;

    WaterVaporMixingRatioWrtDryAir_B(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    oops::Variables trajectoryVars() const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    bool hasNL() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};
}  // namespace vader
