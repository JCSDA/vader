/*
 * (C) Copyright 2021-2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_RECIPES_DRYAIRDENSITYLEVELSMINUSONE_H_
#define SRC_VADER_RECIPES_DRYAIRDENSITYLEVELSMINUSONE_H_

#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class DryAirDensityLevelsMinusOne_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(DryAirDensityLevelsMinusOne_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
  oops::Parameter<double> rd{"rd", "specific gas constant for dry air", 2.8705e2, this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief DryAirDensityLevelsMinusOne_A class defines a recipe for dry air density
 *
 *  \details This instantiation of RecipeBase produces dry air density
 *           using temperature and pressure as inputs. The parameter
 *           rd can be specified via the constructor configuration.
 *           If it is not, the code will provide a default value.
 */
class DryAirDensityLevelsMinusOne_A : public RecipeBase {
 public:
    static const char Name[];
    static const std::vector<std::string> Ingredients;

    typedef DryAirDensityLevelsMinusOne_AParameters Parameters_;

    explicit DryAirDensityLevelsMinusOne_A(const Parameters_ &);

    // Recipe base class overrides
    std::string name() const override;
    std::string product() const override;
    std::vector<std::string> ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    bool executeNL(atlas::FieldSet &) override;
    bool executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    bool executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    const double rd_;
};

}  // namespace vader

#endif  // SRC_VADER_RECIPES_DRYAIRDENSITYLEVELSMINUSONE_H_