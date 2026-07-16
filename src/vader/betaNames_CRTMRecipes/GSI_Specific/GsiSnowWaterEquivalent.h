/*
 * (C) Copyright 2026 UCAR
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

class GsiSnowWaterEquivalent_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(GsiSnowWaterEquivalent_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief GsiSnowWaterEquivalent_A produces gsi_snow_water_equivalent from sheleg
 *
 *  \details Copy of GSI's local_swe intermediate: sheleg with missing-value sentinels (|x|>1e11)
 *           reset to zero. Used by other CRTM surface recipes (snow-thickness, surface-type index).
 */
class GsiSnowWaterEquivalent_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef GsiSnowWaterEquivalent_AParameters Parameters_;

    GsiSnowWaterEquivalent_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;
};

}  // namespace vader
