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

class DustBin1MassConcentration_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(DustBin1MassConcentration_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------

/*! \brief DustBin1MassConcentration_A class defines a recipe for dust mass concentration
 *         in bin 1 of the CLASSIC model
 *
 *  \details This instantiation of RecipeBase produces dust mass concentrations in bin 1
 *           using mass_fraction_of_dust_coarse_aerosol_particles_in_air,
 *           mass_fraction_of_dust_accumulation_aerosol_particles_in_air,
 *           number_fraction_of_coarse_aerosol_particles_in_air, and
 *           number_fraction_of_accumulation_aerosol_particles_in_air as inputs.
 */
class DustBin1MassConcentration_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef DustBin1MassConcentration_A_Parameters Parameters_;

    DustBin1MassConcentration_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return false; }
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};


// ------------------------------------------------------------------------------------------------

}  // namespace vader
