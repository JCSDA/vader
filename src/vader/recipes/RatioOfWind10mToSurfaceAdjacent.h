/*
 * (C) Copyright 2024 UCAR
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

class RatioOfWind10mToSurfaceAdjacent_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(RatioOfWind10mToSurfaceAdjacent_AParameters, RecipeParametersBase)

 public:
    oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief RatioOfWind10mToSurfaceAdjacent_A class defines a recipe to calculate surface wind
 * reduction factor
 *
 *  \details This recipe uses model provided u10/v10 to calculate a ratio against
 *  the winds at the lowest model level for use with the "VertInterp" operator:
 *  ratio_of_wind_at_10m_to_wind_at_surface_adjacent_layer = |V(10m)| / |V(lowest level)|.
 *  A calm 10 m wind gives a ratio of 0, so ratio x lowest-level wind reproduces it.
 *  Where the lowest-level wind is calm the ratio is undefined, and the recipe returns 1.
 */
class RatioOfWind10mToSurfaceAdjacent_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef RatioOfWind10mToSurfaceAdjacent_AParameters Parameters_;

    RatioOfWind10mToSurfaceAdjacent_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

// -------------------------------------------------------------------------------------------------

}  // namespace vader
