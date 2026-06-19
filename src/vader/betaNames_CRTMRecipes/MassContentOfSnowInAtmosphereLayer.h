/*
 * (C) Copyright 2025 UCAR.
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

namespace vader
{

// -------------------------------------------------------------------------------------------------

class MassContentOfSnowInAtmosphereLayer_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(MassContentOfSnowInAtmosphereLayer_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief The class 'MassContentOfSnowInAtmosphereLayer_A' defines a recipe for
 *         mass content of snow in atmosphere layer
 *
 *  \details This instantiation of RecipeBase produces mass content of snow
 *           from mixing ratio via geometric conversion.
 *           Inputs: snow_water (mixing ratio), air_pressure_thickness
 *           Output units: kg m-2
 */
class MassContentOfSnowInAtmosphereLayer_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef MassContentOfSnowInAtmosphereLayer_AParameters Parameters_;

    MassContentOfSnowInAtmosphereLayer_A(const Parameters_ &, const VaderConfigVars &);

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

// -------------------------------------------------------------------------------------------------

}  // namespace vader
