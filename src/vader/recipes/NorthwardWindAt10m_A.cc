/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "oops/util/Logger.h"
#include "vader/recipes/NorthwardWindAt10m.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char vwind_at_10m_A::Name[] = "vwind_at_10m_A";
const oops::Variables vwind_at_10m_A::Ingredients{std::vector<std::string>{"northward_wind"}};

// Register the maker
static RecipeMaker<vwind_at_10m_A> makeruwind_at_10m_A_(vwind_at_10m_A::Name);

vwind_at_10m_A::vwind_at_10m_A(const Parameters_ & params,
                               const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "vwind_at_10m_A::vwind_at_10m_A(params)" << std::endl;
}

std::string vwind_at_10m_A::name() const
{
    return vwind_at_10m_A::Name;
}

oops::Variable vwind_at_10m_A::product() const
{
    return oops::Variable{"northward_wind_at_10m"};
}

oops::Variables vwind_at_10m_A::ingredients() const
{
    return vwind_at_10m_A::Ingredients;
}

oops::Variables vwind_at_10m_A::trajectoryVars() const
{
    return oops::Variables{};
}

size_t vwind_at_10m_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace vwind_at_10m_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset["northward_wind"].functionspace();
}

void vwind_at_10m_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering vwind_at_10m_A::executeNL function"
      << std::endl;

    auto northward_wind_view = atlas::array::make_view<double, 2>(afieldset["northward_wind"]);
    auto vwind_at_10m_A_view =
                             atlas::array::make_view<double, 2>(afieldset["northward_wind_at_10m"]);

    size_t no_of_horizontal_pts = vwind_at_10m_A_view.shape(0);
    for ( size_t jnode = 0; jnode < no_of_horizontal_pts ; ++jnode ) {
      vwind_at_10m_A_view(jnode, 0) = northward_wind_view(jnode, 0);
    }
    oops::Log::trace() << "leaving vwind_at_10m_A::executeNL function" << std::endl;
}

void vwind_at_10m_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering vwind_at_10m_A::executeTL function"
        << std::endl;

    auto tl_northward_wind_view = atlas::array::make_view<double, 2>(afieldsetTL["northward_wind"]);
    auto tl_vwind_at_10m_A_view =
                           atlas::array::make_view<double, 2>(afieldsetTL["northward_wind_at_10m"]);

    size_t no_of_horizontal_pts = tl_vwind_at_10m_A_view.shape(0);
    for ( size_t jnode = 0; jnode < no_of_horizontal_pts ; ++jnode ) {
      tl_vwind_at_10m_A_view(jnode, 0) = tl_northward_wind_view(jnode, 0);
    }
    oops::Log::trace() << "leaving vwind_at_10m_A::executeTL function" << std::endl;
}

void vwind_at_10m_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering vwind_at_10m_A::executeAD function"
        << std::endl;

    auto ad_northward_wind_view = atlas::array::make_view<double, 2>(afieldsetAD["northward_wind"]);
    auto ad_vwind_at_10m_A_view =
                           atlas::array::make_view<double, 2>(afieldsetAD["northward_wind_at_10m"]);

    size_t no_of_horizontal_pts = ad_northward_wind_view.shape(0);
    for ( size_t jnode = 0; jnode < no_of_horizontal_pts ; ++jnode ) {
      ad_northward_wind_view(jnode, 0) += ad_vwind_at_10m_A_view(jnode, 0);
      ad_vwind_at_10m_A_view(jnode, 0) = 0.0;
    }
    oops::Log::trace() << "leaving vwind_at_10m_A::executeAD function" << std::endl;
}

}  // namespace vader
