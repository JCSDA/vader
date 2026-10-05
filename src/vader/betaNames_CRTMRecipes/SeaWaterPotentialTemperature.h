/*
 * (C) Copyright 2023 UCAR
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
#include "OceanConversions/OceanConversions.interface.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

class SeaWaterPotentialTemperature_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SeaWaterPotentialTemperature_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief SeaWaterPotentialTemperature_A class defines a recipe for sea water potential
 *         temperature
 *
 *  \details This instantiation of RecipeBase produces sea water potential temperature
 *           using insitu water temperature, salinity, longitude, lattitude, and
 *           sea water depth information.
 */
class SeaWaterPotentialTemperature_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef SeaWaterPotentialTemperature_AParameters Parameters_;

    SeaWaterPotentialTemperature_A(const Parameters_ &, const VaderConfigVars &);

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

// -------------------------------------------------------------------------------------------------

}  // namespace vader
