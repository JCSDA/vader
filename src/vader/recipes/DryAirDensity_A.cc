/*
 * (C) Copyright 2024 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "oops/util/Logger.h"
#include "vader/recipes/DryAirDensity.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DryAirDensity_A::Name[] = "DryAirDensity_A";
const oops::Variables DryAirDensity_A::
    Ingredients{std::vector<std::string>{"air_temperature", "air_pressure"}};

// Register the maker
static RecipeMaker<DryAirDensity_A>
    makerDryAirDensity_A_(DryAirDensity_A::Name);

DryAirDensity_A::DryAirDensity_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables) :
        configVariables_{configVariables}

{
    oops::Log::trace() <<
        "DryAirDensity_A::DryAirDensity_A(params)" << std::endl;
}

std::string DryAirDensity_A::name() const
{
    return DryAirDensity_A::Name;
}

oops::Variable DryAirDensity_A::product() const
{
    return oops::Variable("dry_air_density");
}

oops::Variables DryAirDensity_A::ingredients() const
{
    return DryAirDensity_A::Ingredients;
}

oops::Variables DryAirDensity_A::trajectoryVars() const
{
    return DryAirDensity_A::Ingredients;
}

size_t DryAirDensity_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["air_pressure"].shape(1);
}

atlas::FunctionSpace DryAirDensity_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset.field("air_pressure").functionspace();
}

void DryAirDensity_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering DryAirDensity_A::executeNL function"
      << std::endl;
    double rdgas = 287.58f;
    atlas::Field air_t = afieldset.field("air_temperature");
    atlas::Field air_p = afieldset.field("air_pressure");
    auto air_t_view = atlas::array::make_view<double, 2>(air_t);
    auto air_p_view = atlas::array::make_view<double, 2>(air_p);

    atlas::Field air_dens = afieldset.field("dry_air_density");
    auto air_dens_view = atlas::array::make_view<double, 2>(air_dens);

    int nlevels = air_t.shape(1);
    size_t grid_size = air_t.size()/nlevels;

    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        air_dens_view(jnode, level) = air_p_view(jnode, level)/(rdgas*air_t_view(jnode, level));
      }
    }
    oops::Log::trace() << "leaving DryAirDensity_A::executeNL function" << std::endl;
}

void DryAirDensity_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensity_A::executeTL function"
        << std::endl;
    double rdgas = 287.58f;

    atlas::Field air_t = afieldsetTraj.field("air_temperature");
    atlas::Field air_p = afieldsetTraj.field("air_pressure");
    auto air_t_view = atlas::array::make_view<double, 2>(air_t);
    auto air_p_view = atlas::array::make_view<double, 2>(air_p);

    atlas::Field tl_air_t = afieldsetTL.field("air_temperature");
    atlas::Field tl_air_p = afieldsetTL.field("air_pressure");
    auto tl_air_t_view = atlas::array::make_view<double, 2>(tl_air_t);
    auto tl_air_p_view = atlas::array::make_view<double, 2>(tl_air_p);

    atlas::Field tl_air_dens = afieldsetTL.field("dry_air_density");
    auto tl_air_dens_view = atlas::array::make_view<double, 2>(tl_air_dens);

    int nlevels = air_t.shape(1);
    size_t grid_size = air_t.size()/nlevels;

    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        tl_air_dens_view(jnode, level) = tl_air_p_view(jnode, level)
                                        /(rdgas*air_t_view(jnode, level))
                                        -tl_air_t_view(jnode, level)
                                        *(air_p_view(jnode, level)/rdgas)
                                        /air_t_view(jnode, level)/air_t_view(jnode, level);
      }
    }
    oops::Log::trace() << "leaving DryAirDensity_A::executeTL function" << std::endl;
}

void DryAirDensity_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensity::executeAD function"
        << std::endl;

    double rdgas = 287.58f;

    atlas::Field air_t = afieldsetTraj.field("air_temperature");
    atlas::Field air_p = afieldsetTraj.field("air_pressure");
    auto air_t_view = atlas::array::make_view<double, 2>(air_t);
    auto air_p_view = atlas::array::make_view<double, 2>(air_p);

    atlas::Field ad_air_dens = afieldsetAD.field("dry_air_density");
    auto ad_air_dens_view = atlas::array::make_view<double, 2>(ad_air_dens);

    atlas::Field ad_air_t = afieldsetAD.field("air_temperature");
    atlas::Field ad_air_p = afieldsetAD.field("air_pressure");
    auto ad_air_t_view = atlas::array::make_view<double, 2>(ad_air_t);
    auto ad_air_p_view = atlas::array::make_view<double, 2>(ad_air_p);

    int nlevels = air_t.shape(1);
    size_t grid_size = air_t.size()/nlevels;

    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        ad_air_p_view(jnode, level) +=
                 ad_air_dens_view(jnode, level)/(rdgas*air_t_view(jnode, level));
        ad_air_t_view(jnode, level) +=
                -ad_air_dens_view(jnode, level)*(air_p_view(jnode, level)/rdgas)
                /air_t_view(jnode, level)/air_t_view(jnode, level);
        ad_air_dens_view(jnode, level) = 0.0;
      }
    }
    oops::Log::trace() << "leaving DryAirDensity_A::executeAD function" << std::endl;
}

}  // namespace vader
