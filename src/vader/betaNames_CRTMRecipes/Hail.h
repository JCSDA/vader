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

class Hail_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(Hail_AParameters,
                       RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief The class 'Hail_A' defines a recipe for
 *         mixing ratio of hail
 *
 *  \details This instantiation of RecipeBase produces mixing ratio of hail
 *           using a simplified diagnostic approximation based on pressure-dependent
 *           RH thresholds and temperature
 *           Inputs: air_temperature, air_pressure, relative_humidity, air_pressure_at_surface
 *           Output units: kg kg-1
 */
class Hail_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef Hail_AParameters Parameters_;

    Hail_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;

    oops::Variables trajectoryVars() const override;
    bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;
};

// -------------------------------------------------------------------------------------------------

}  // namespace vader
