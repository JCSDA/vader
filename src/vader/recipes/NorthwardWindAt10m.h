/*
 * (C) Copyright 2021-2022 UCAR
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

class NorthwardWindAt10m_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(NorthwardWindAt10m_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief NorthwardWindAt10m_A computes the northward wind at 10 m (nonlinear only)
 *
 *  \details northward_wind_at_10m = f10m * northward_wind(lowest model level), where f10m is
 *           ratio_of_wind_at_10m_to_wind_at_surface_adjacent_layer (the 10 m wind over the
 *           lowest-level wind). The TL/AD are in NorthwardWindAt10m_B, because they
 *           need the ratio only from the trajectory, not as an increment.
 */
class NorthwardWindAt10m_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef NorthwardWindAt10m_AParameters Parameters_;

    NorthwardWindAt10m_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

// ------------------------------------------------------------------------------------------------

class NorthwardWindAt10m_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(NorthwardWindAt10m_BParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief NorthwardWindAt10m_B is the TL/AD of NorthwardWindAt10m_A (no nonlinear method)
 *
 *  \details d(northward_wind_at_10m) = f10m * d(northward_wind(lowest model level)), with f10m
 *           (ratio_of_wind_at_10m_to_wind_at_surface_adjacent_layer) taken from the trajectory
 *           and held fixed. The only increment ingredient is northward_wind.
 */
class NorthwardWindAt10m_B : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef NorthwardWindAt10m_BParameters Parameters_;

    NorthwardWindAt10m_B(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    oops::Variables trajectoryVars() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    bool hasNL() const override { return false; }
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

}  // namespace vader
