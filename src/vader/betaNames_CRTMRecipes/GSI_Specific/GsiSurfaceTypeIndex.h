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

class GsiSurfaceTypeIndex_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(GsiSurfaceTypeIndex_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief GsiSurfaceTypeIndex_A produces the GSI surface-type index used by CRTM surface recipes.
 *
 *  \details Values {0,1,2,3} = {sea,land,ice,snow}. Computed as nint(slmsk), then any cell with
 *           slmsk>=1 and snow water equivalent > minswe (0.1) is reassigned to 3.
 */
class GsiSurfaceTypeIndex_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef GsiSurfaceTypeIndex_AParameters Parameters_;

    GsiSurfaceTypeIndex_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;
};

}  // namespace vader
