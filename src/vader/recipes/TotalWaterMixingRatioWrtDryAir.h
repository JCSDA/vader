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

class TotalWaterMixingRatioWrtDryAir_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(TotalWaterMixingRatioWrtDryAir_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief TotalWaterMixingRatioWrtDryAir_A class defines a recipe for total mixing ratio
 *
 *  \details This instantiation of RecipeBase produces total mixing ratio using
 *           moist air and condensed water mixing ratio wrt dry air as inputs.
 */
class TotalWaterMixingRatioWrtDryAir_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef TotalWaterMixingRatioWrtDryAir_AParameters Parameters_;

    TotalWaterMixingRatioWrtDryAir_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return false; }
    void executeNL(atlas::FieldSet &) override;

 private:
};

}  // namespace vader
