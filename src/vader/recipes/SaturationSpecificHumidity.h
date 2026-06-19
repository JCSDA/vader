/*
 * (C) Crown Copyright 2024 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */


#pragma once

#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "vader/RecipeBase.h"

namespace vader
{

// -------------------------------------------------------------------------------------------------

/*! \brief The class 'SaturationSpecificHumidity_A' defines a recipe for
 *         'saturation specific humidity (qsat)'
 *
 *  \details This instantiation of RecipeBase produces saturation specific humidity (qsat)
 *           using air pressure, saturation vapour pressure (svp), and air temperature as inputs.
 *
 */
class SaturationSpecificHumidity_A : public RecipeBase
{
 public:
    static const char Name[];

    typedef EmptyRecipeParameters Parameters_;

    SaturationSpecificHumidity_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;

    void executeNL(atlas::FieldSet &) override;
};

// -------------------------------------------------------------------------------------------------

class SaturationSpecificHumidity_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SaturationSpecificHumidity_BParameters, RecipeParametersBase)
 public:
};

// -------------------------------------------------------------------------------------------------

/*! \brief The class 'SaturationSpecificHumidity_B' defines a recipe for qsat
 *
 *  \details This instantiation of RecipeBase produces saturation specific humidity (qsat)
 *           using exact formula with TL/AD support.
 *           Inputs: svp, air_pressure
 *           Output: water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation (kg/kg)
 *
 *           Formula: qsat = 0.622 * es / (p - 0.378 * es)
 */
class SaturationSpecificHumidity_B : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef SaturationSpecificHumidity_BParameters Parameters_;

    SaturationSpecificHumidity_B(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;

    bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    oops::Variables trajectoryVars() const override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

// -------------------------------------------------------------------------------------------------

}  // namespace vader
