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

class LeafAreaIndex_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(LeafAreaIndex_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief LeafAreaIndex_A mirrors the fv3-jedi crtm_surface_lai helper.
 *
 *  \details A simple triangle-wave function of day-of-year and latitude per
 *           GSI's crtm_interface, parameterized by the CRTM microwave vegetation type.
 *           Requires day_of_year from the VaderConfigVars.
 *           Ingredients: gsi_surface_type_index, vtype, stype, vegetation_type_index.
 *           Latitude is read from the functionspace of the ingredient fields rather than
 *           being supplied as a separate field.
 *           Glacial-land-ice cells (vtype==15 or stype==16) trigger a reset of the entire field
 *           to 0, matching the legacy Fortran behavior.
 */
class LeafAreaIndex_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef LeafAreaIndex_AParameters Parameters_;

    LeafAreaIndex_A(const Parameters_ &, const VaderConfigVars &);

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
