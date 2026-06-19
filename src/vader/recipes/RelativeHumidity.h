/*
 * (C) Crown Copyright 2023 Met Office.
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

class RelativeHumidity_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(RelativeHumidity_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief RelativeHumidity_A class defines a recipe for relative humidity
 *
 *  \details This instantiation of RecipeBase produces relative humidity
 *           using air temperature (T), specific humidity (q),
 *           specific humidity assuming saturation (qsat) and the derivative of
 *           the logarithm of the saturation vapour pressure with respect to
 *           air temperature (dlsvpdT) as inputs.
 *           Its purpose is to test for adjoitness the adjoint and tangent code.
 *           WARNING! The output is in percents and not a ratio
 *           WARNING! The tangent-linear code is not the exact tangent linear
 *                    of the non-linear code:
 *           * The TL/AD code to be tested derives relative humidity increments
 *             from q and T increments, and from q, qsat and dlsvpdT states.
 *           * The non-linear code derives relative humidity state from q and
 *             qsat states.
 *
 */
class RelativeHumidity_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef RelativeHumidity_AParameters Parameters_;

    RelativeHumidity_A(const Parameters_ &, const VaderConfigVars &);

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

// ------------------------------------------------------------------------------------------------

class RelativeHumidity_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(RelativeHumidity_BParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief RelativeHumidity_B class defines a recipe for relative humidity
 *
 *  \details This instantiation of RecipeBase produces relative humidity
 *           using modular ingredients. Works with any qsat source.
 *           Formula: rh = q / qsat
 *           Inputs: water_vapor_mixing_ratio_wrt_moist_air,
 *                   water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation
 *           Output units: fraction (0-1, ESM convention)
 *           Full TL/AD support
 */
class RelativeHumidity_B : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef RelativeHumidity_BParameters Parameters_;

    RelativeHumidity_B(const Parameters_ &, const VaderConfigVars &);

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
