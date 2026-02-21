/*
 * (C) Copyright 2023-2026 UCAR
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

class LnAirPressureAtInterface_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(LnAirPressureAtInterface_AParameters, RecipeParametersBase)

 public:
    oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief LnAirPressureAtInterface_A class defines a recipe for ln_air_pressure_at_interface
 *         from air_pressure_levels
 *
 *         NL:
 *             ln_p_int(j, k) = ln( p_int(j, k) )
 *         TL:
 *             ln_p_int'(j, k) = p_int'(j, k) / p_int(j, k)
 *         AD:
 *             p_int_ad(j, k) += ln_p_int_ad(j, k) / p_int(j, k)
 *             ln_p_int_ad(j, k) = 0
 *
 *         where:
 *         - p_int is air_pressure_levels (Pa)
 *         - ln_p_int is ln_air_pressure_at_interface (unitless)
 *         - j indexes horizontal points (0..npoint-1)
 *         - k indexes interfaces (0..nint-1)
 */
class LnAirPressureAtInterface_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef LnAirPressureAtInterface_AParameters Parameters_;

    LnAirPressureAtInterface_A(const Parameters_ &, const VaderConfigVars &);

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
