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

class MassContentOfGraupelInAtmosphereLayer_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(MassContentOfGraupelInAtmosphereLayer_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief The class 'MassContentOfGraupelInAtmosphereLayer_A' defines a recipe for
 *         mass content of graupel in atmosphere layer
 *
 *  \details This instantiation of RecipeBase produces mass content of graupel
 *           from mixing ratio via geometric conversion.
 *           Inputs: graupel (mixing ratio), air_pressure_thickness
 *           Output units: kg m-2
 */
class MassContentOfGraupelInAtmosphereLayer_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef MassContentOfGraupelInAtmosphereLayer_AParameters Parameters_;

    MassContentOfGraupelInAtmosphereLayer_A(const Parameters_ &, const VaderConfigVars &);

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
