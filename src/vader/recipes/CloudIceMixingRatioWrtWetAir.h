/*
 * (C) Crown Copyright 2023-2025 Met Office.
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

class CloudIceMixingRatioWrtWetAir_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(CloudIceMixingRatioWrtWetAir_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief CloudIceMixingRatioWrtWetAir_A class defines a recipe for cloud ice mixing ratio wrt moist air
 *         and condensed water
 *
 *  \details This instantiation of RecipeBase produces the cloud ice mixing ratio wrt moist air
 *          and condensed water using the cloud ice mixing ratio wrt dry air and the moist air
 *          and condensed water mixing ratio wrt dry air.
 */
class CloudIceMixingRatioWrtWetAir_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef CloudIceMixingRatioWrtWetAir_AParameters Parameters_;

    CloudIceMixingRatioWrtWetAir_A(const Parameters_ &, const VaderConfigVars &);

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
};

}  // namespace vader
