/*
 * (C) Crown Copyright 2025 Met Office.
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

namespace vader {

class DustAccumulationModeMassFraction_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(DustAccumulationModeMassFraction_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------

/*! \brief DustAccumulationModeMassFraction_A class defines a recipe for dust mass fraction
 *         in the dust accumulation mode of the UKCA/GLOMAP model
 *
 *  \details This instantiation of RecipeBase produces the mass fraction increment in the
 *           dust accumulation mode of the UKCA/GLOMAP dust model using the mass increment
 *           of bin 1 of the 2-bin CLASSIC dust model.
 */
class DustAccumulationModeMassFraction_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef DustAccumulationModeMassFraction_A_Parameters Parameters_;

    DustAccumulationModeMassFraction_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    oops::Variables trajectoryVars() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    bool hasNL() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};


// ------------------------------------------------------------------------------------------------

}  // namespace vader
