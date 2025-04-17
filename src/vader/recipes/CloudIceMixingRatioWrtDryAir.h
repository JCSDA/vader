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

class CloudIceMixingRatioWrtDryAir_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(CloudIceMixingRatioWrtDryAir_AParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

class CloudIceMixingRatioWrtDryAir_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(CloudIceMixingRatioWrtDryAir_BParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief CloudIceMixingRatioWrtDryAir_A class defines a recipe for cloud ice
 *         vapor mixing ratio wrt dry air
 *
 *  \details This instantiation of RecipeBase produces the cloud ice mixing ratio wrt dry air
 *          increment using the increment of cloud ice mixing ratio wrt moist air
 *          and condensed water
 *          The trajectory uses quantities that involve mixing ratios wrt dry air
 */
class CloudIceMixingRatioWrtDryAir_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef CloudIceMixingRatioWrtDryAir_AParameters Parameters_;

    CloudIceMixingRatioWrtDryAir_A(const Parameters_ &, const VaderConfigVars &);

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
/*! \brief CloudIceMixingRatioWrtDryAir_B class defines a recipe for cloud ice
 *         vapor mixing ratio wrt dry air
 *
 *  \details This instantiation of RecipeBase produces the cloud ice mixing ratio wrt dry air
 *          increment using the increment of cloud ice mixing ratio wrt moist air
 *          and condensed water
 *          The trajectory uses quantities that involve mixing ratios wrt moist air
 *          and condensed water.
 *          This is the more standard recipe
 */
class CloudIceMixingRatioWrtDryAir_B : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef CloudIceMixingRatioWrtDryAir_BParameters Parameters_;

    CloudIceMixingRatioWrtDryAir_B(const Parameters_ &, const VaderConfigVars &);

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
