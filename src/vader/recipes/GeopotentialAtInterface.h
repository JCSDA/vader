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

class GeopotentialAtInterface_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(GeopotentialAtInterface_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief GeopotentialAtInterface_A computes geopotential at model interfaces
 *
 *  Uses hydrostatic integration with layer-averaged virtual temperature.
 *  Inputs: geopotential at full levels, virtual_temperature, ln_air_pressure at full levels
 *  and interfaces. Output: geopotential_at_interface (m^2 s^-2).
 *  Full TL/AD support.
 */

class GeopotentialAtInterface_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef GeopotentialAtInterface_A_Parameters Parameters_;

    GeopotentialAtInterface_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    oops::Variables trajectoryVars() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

}  // namespace vader
