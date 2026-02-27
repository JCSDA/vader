/*
 * (C) Copyright 2024  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/WindReductionFactorAt10m.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WindReductionFactorAt10m_A::Name[] = "WindReductionFactorAt10m_A";
const oops::Variables WindReductionFactorAt10m_A::Ingredients{
      std::vector<std::string>{"eastward_wind_at_surface",
                               "northward_wind_at_surface",
                               "eastward_wind",
                               "northward_wind"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<WindReductionFactorAt10m_A>
       makerWindReductionFactorAt10m_A_(WindReductionFactorAt10m_A::Name);

// -------------------------------------------------------------------------------------------------

WindReductionFactorAt10m_A::WindReductionFactorAt10m_A(
                            const WindReductionFactorAt10m_AParameters & params,
                            const VaderConfigVars & configVariables):
                            configVariables_{configVariables}
{
    oops::Log::trace() << "WindReductionFactorAt10m_A::WindReductionFactorAt10m_A Starting"
                       << std::endl;
    oops::Log::trace() << "WindReductionFactorAt10m_A::WindReductionFactorAt10m_A Done"
                       << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string WindReductionFactorAt10m_A::name() const {
    return WindReductionFactorAt10m_A::Name;
}

// -------------------------------------------------------------------------------------------------

    // variable name used in yaml file
oops::Variable WindReductionFactorAt10m_A::product() const {
    return oops::Variable("wind_reduction_factor_at_10m");
}

// -------------------------------------------------------------------------------------------------

oops::Variables WindReductionFactorAt10m_A::ingredients() const {
    return WindReductionFactorAt10m_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t WindReductionFactorAt10m_A::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("eastward_wind_at_surface").shape(1);
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace WindReductionFactorAt10m_A::productFunctionSpace(const atlas::FieldSet
& afieldset) const {
    return afieldset.field("eastward_wind_at_surface").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WindReductionFactorAt10m_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "WindReductionFactorAt10m_A::executeNL Starting" << std::endl;

    // output:
    atlas::Field f10m = afieldset.field("wind_reduction_factor_at_10m");
    f10m.metadata().set("units", "none");

    // Reduce by 1 since index begins at 0
    const int nLevel = afieldset.field("eastward_wind").shape(1) - 1;

    util::for_each_column(
      [&](const auto uu_10m_col,
          const auto vv_10m_col,
          const auto uu_col,
          const auto vv_col,
          auto f10m_col) {
        f10m_col(0) = std::sqrt(std::pow(uu_10m_col(0), 2) + std::pow(vv_10m_col(0), 2));
        if (f10m_col(0) > 0) {
            f10m_col(0) = f10m_col(0) / std::sqrt(std::pow(uu_col(nLevel), 2)
                                      + std::pow(vv_col(nLevel), 2));
        } else {
            f10m_col(0) = 1.0;
        }
      },
      afieldset["eastward_wind_at_surface"],
      afieldset["northward_wind_at_surface"],
      afieldset["eastward_wind"],
      afieldset["northward_wind"],
      f10m);

    oops::Log::trace() << "WindReductionFactorAt10m_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
