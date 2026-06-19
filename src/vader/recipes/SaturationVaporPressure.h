
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

/*! \brief The class 'SaturationVaporPressure_A' defines a recipe for 'saturation water pressure (svp)'
 *
 *  \details This instantiation of RecipeBase produces saturation vapour pressure (svp)
 *           using air temperature as input.
 *
 */
class SaturationVaporPressure_A : public RecipeBase
{
 public:
    static const char Name[];

    typedef EmptyRecipeParameters Parameters_;

    SaturationVaporPressure_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;

    void executeNL(atlas::FieldSet &) override;
};

// -------------------------------------------------------------------------------------------------

class SaturationVaporPressure_B_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SaturationVaporPressure_B_Parameters, RecipeParametersBase)
 public:
};

// -------------------------------------------------------------------------------------------------

/*! \brief The class 'SaturationVaporPressure_B' defines a recipe for 'saturation vapor pressure'
 *
 *  \details This instantiation of RecipeBase produces saturation vapor pressure (svp)
 *           using Murphy & Koop (2005) formula with TL/AD support.
 *           Handles both liquid water (T >= 273.15K) and ice (T < 273.15K).
 *           Input: air_temperature
 *           Output: svp (Pa)
 *
 *           Reference: Murphy & Koop (2005), valid 110-332K with ~0.01% accuracy
 */
class SaturationVaporPressure_B : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef SaturationVaporPressure_B_Parameters Parameters_;

    SaturationVaporPressure_B(const Parameters_ &, const VaderConfigVars &);

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
