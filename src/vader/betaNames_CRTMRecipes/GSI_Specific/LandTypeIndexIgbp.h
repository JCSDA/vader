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

class LandTypeIndexIgbp_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(LandTypeIndexIgbp_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief LandTypeIndexIgbp_A mirrors the fv3-jedi crtm_surface land-type-IGBP helper.
 *
 *  \details Default is 1 (evergreen needleleaf forest). Over land, looks up the IGBP CRTM index
 *           via the identity mapping (model IGBP -> CRTM IGBP).
 */
class LandTypeIndexIgbp_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef LandTypeIndexIgbp_AParameters Parameters_;

    LandTypeIndexIgbp_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;
};

}  // namespace vader
