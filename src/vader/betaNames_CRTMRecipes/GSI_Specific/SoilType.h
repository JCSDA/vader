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

class SoilType_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SoilType_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief SoilType_A mirrors the fv3-jedi crtm_surface soil-type helper.
 *
 *  \details Default is 1 (coarse loamy sand in the CRTM microwave 9-type system).
 *           Over land, looks up the CRTM microwave soil index via the GSI table.
 *           Note: unlike the *_npoess / *_igbp routines, this routine does NOT apply max(1, .)
 *           protection to the looked-up value, preserving the Fortran behavior bit-for-bit.
 */
class SoilType_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef SoilType_AParameters Parameters_;

    SoilType_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;
};

}  // namespace vader
