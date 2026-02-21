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

namespace vader
{

// -------------------------------------------------------------------------------------------------

class LnAirPressure_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(LnAirPressure_AParameters, RecipeParametersBase)

 public:
    oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief LnAirPressure_A class defines a recipe for ln_air_pressure from air_pressure
 *
 *         NL:
 *             ln_p(j, k) = ln( p(j, k) )
 *         TL:
 *             ln_p'(j, k) = p'(j, k) / p(j, k)
 *         AD:
 *             p_ad(j, k) += ln_p_ad(j, k) / p(j, k)
 *             ln_p_ad(j, k) = 0
 *
 *         where:
 *         - p is air_pressure (Pa)
 *         - ln_p is ln(air_pressure) (unitless)
 *         - j indexes horizontal points (0..npoint-1)
 *         - k indexes vertical levels (0..nlevels-1)
 */
class LnAirPressure_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef LnAirPressure_AParameters Parameters_;

    LnAirPressure_A(const Parameters_ &, const VaderConfigVars &);

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
};

// -------------------------------------------------------------------------------------------------

}  // namespace vader
