/*
 * (C) Copyright 2025 UCAR
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

class WaterVaporMixingRatioWrtMoistAir_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WaterVaporMixingRatioWrtMoistAir_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief WaterVaporMixingRatioWrtMoistAir_A class defines a recipe for water_vapor_mixing_ratio_
 *         wrt_moist_air (specific humidity). 
 *
 *  \details This instantiation of RecipeBase produces water_vapor_mixing_ratio_wrt_moist_air (q,
 *           specific humidity) using water_vapor_mixing_ratio_wrt_dry_air (r, humidity mixing 
 *           ratio).
 *
 */
class WaterVaporMixingRatioWrtMoistAir_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef WaterVaporMixingRatioWrtMoistAir_AParameters Parameters_;

    WaterVaporMixingRatioWrtMoistAir_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

}  // namespace vader
