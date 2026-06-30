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
#include "vader/recipes/GeopotentialHeightLevels.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char GeopotentialHeightLevels_A::Name[] = "GeopotentialHeightLevels_A";
const oops::Variables GeopotentialHeightLevels_A::Ingredients{std::vector<std::string>{
                                                        "geopotential_levels"}};

// Register the maker
static RecipeMaker<GeopotentialHeightLevels_A> makerGeopotentialHeightLevels_A_(
                                                            GeopotentialHeightLevels_A::Name);

GeopotentialHeightLevels_A::GeopotentialHeightLevels_A(const Parameters_ & params,
                                   const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "GeopotentialHeightLevels_A::GeopotentialHeightLevels_A(params)"
      << std::endl;
}

std::string GeopotentialHeightLevels_A::name() const
{
    return GeopotentialHeightLevels_A::Name;
}

oops::Variable GeopotentialHeightLevels_A::product() const
{
    return oops::Variable{"geopotential_height_levels"};
}

oops::Variables GeopotentialHeightLevels_A::ingredients() const
{
    return GeopotentialHeightLevels_A::Ingredients;
}

size_t GeopotentialHeightLevels_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("geopotential_levels").shape(1);
}

atlas::FunctionSpace GeopotentialHeightLevels_A::productFunctionSpace(const atlas::FieldSet &
                                                                                    afieldset) const
{
    return afieldset.field("geopotential_levels").functionspace();
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightLevels_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering GeopotentialHeightLevels_A::executeNL function"
      << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](const double phi_int,
          double& z_int) {
          z_int = phi_int * inv_g;
      },
      afieldset["geopotential_levels"],
      afieldset["geopotential_height_levels"]);

  oops::Log::trace() << "leaving GeopotentialHeightLevels_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightLevels_A::executeTL(atlas::FieldSet & afieldsetTL,
                                          const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering GeopotentialHeightLevels_A::executeTL function"
      << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](const double phi_int_tl,
          double& z_int_tl) {
          z_int_tl = phi_int_tl * inv_g;
      },
      afieldsetTL["geopotential_levels"],
      afieldsetTL["geopotential_height_levels"]);

    oops::Log::trace() << "leaving GeopotentialHeightLevels_A::executeTL function"
      << std::endl;
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightLevels_A::executeAD(atlas::FieldSet & afieldsetAD,
                                          const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering GeopotentialHeightLevels_A::executeAD function"
      << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](double& phi_int_ad,
          double& z_int_ad) {
          phi_int_ad += z_int_ad * inv_g;
          z_int_ad = 0.0;
      },
      afieldsetAD["geopotential_levels"],
      afieldsetAD["geopotential_height_levels"]);

    oops::Log::trace() << "leaving GeopotentialHeightLevels_A::executeAD function"
      << std::endl;
}

}  // namespace vader
