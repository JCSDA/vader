/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/GeopotentialHeightAtSurface.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char GeopotentialHeightAtSurface_A::Name[] = "GeopotentialHeightAtSurface_A";
const oops::Variables GeopotentialHeightAtSurface_A::Ingredients{std::vector<std::string>{
                                                        "geopotential_at_surface"}};

// Register the maker
static RecipeMaker<GeopotentialHeightAtSurface_A> makerGeopotentialHeightAtSurface_A_(
                                                              GeopotentialHeightAtSurface_A::Name);

GeopotentialHeightAtSurface_A::GeopotentialHeightAtSurface_A(const Parameters_ & params,
                                   const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "GeopotentialHeightAtSurface_A::GeopotentialHeightAtSurface_A(params)"
      << std::endl;
}

std::string GeopotentialHeightAtSurface_A::name() const
{
    return GeopotentialHeightAtSurface_A::Name;
}

oops::Variable GeopotentialHeightAtSurface_A::product() const
{
    return oops::Variable{"geopotential_height_at_surface"};
}

oops::Variables GeopotentialHeightAtSurface_A::ingredients() const
{
    return GeopotentialHeightAtSurface_A::Ingredients;
}

size_t GeopotentialHeightAtSurface_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace GeopotentialHeightAtSurface_A::productFunctionSpace(const atlas::FieldSet &
                                                                                  afieldset) const
{
    return afieldset.field("geopotential_at_surface").functionspace();
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightAtSurface_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering GeopotentialHeightAtSurface_A::executeNL" << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](const double phi_surf,
          double& z_surf) {
          z_surf = phi_surf * inv_g;
      },
      afieldset["geopotential_at_surface"],
      afieldset["geopotential_height_at_surface"]);

    oops::Log::trace() << "leaving GeopotentialHeightAtSurface_A::executeNL" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightAtSurface_A::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering GeopotentialHeightAtSurface_A::executeTL function" << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](const double phi_surf_tl,
          double& z_surf_tl) {
          z_surf_tl = phi_surf_tl * inv_g;
      },
      afieldsetTL["geopotential_at_surface"],
      afieldsetTL["geopotential_height_at_surface"]);

    oops::Log::trace() << "leaving GeopotentialHeightAtSurface_A::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightAtSurface_A::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering GeopotentialHeightAtSurface_A::executeAD function" << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](double& phi_surf_ad,
          double& z_surf_ad) {
          phi_surf_ad += z_surf_ad * inv_g;
          z_surf_ad = 0.0;
      },
      afieldsetAD["geopotential_at_surface"],
      afieldsetAD["geopotential_height_at_surface"]);

    oops::Log::trace() << "leaving GeopotentialHeightAtSurface_A::executeAD function" << std::endl;
}

}  // namespace vader
