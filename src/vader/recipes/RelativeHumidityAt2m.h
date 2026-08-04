/*
 * (C) Crown Copyright 2025 Met Office
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

class RelativeHumidityAt2m_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(RelativeHumidityAt2m_AParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief RelativeHumidityAt2m_A class defines a recipe for converting
 *         2m relative humidity from non-standard percentage to standard fraction units.
 *
 *  \details This recipe converts relative_humidity_at_2m_percentage (input, 0-100)
 *           to relative_humidity_at_2m (output, 0-1) using linear scaling.
 *           - NL:  relative_humidity_at_2m = relative_humidity_at_2m_percentage / 100
 *           - TL:  d(RH fraction) = d(RH percent) / 100
 *           - AD:  d(RH percent) += d(RH fraction) / 100
 *
 *           This is used to bridge input files that provide RH in percentage units
 *           to the internal fraction representation (ESM convention).
 */
class RelativeHumidityAt2m_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef RelativeHumidityAt2m_AParameters Parameters_;

    RelativeHumidityAt2m_A(const Parameters_ &, const VaderConfigVars &);

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
