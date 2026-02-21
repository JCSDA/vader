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
#include "vader/recipes/GeopotentialHeightAtInterface.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char GeopotentialHeightAtInterface_A::Name[] = "GeopotentialHeightAtInterface_A";
const oops::Variables GeopotentialHeightAtInterface_A::Ingredients{std::vector<std::string>{
                                                        "geopotential_levels"}};

// Register the maker
static RecipeMaker<GeopotentialHeightAtInterface_A> makerGeopotentialHeightAtInterface_A_(
                                                            GeopotentialHeightAtInterface_A::Name);

GeopotentialHeightAtInterface_A::GeopotentialHeightAtInterface_A(const Parameters_ & params,
                                   const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "GeopotentialHeightAtInterface_A::GeopotentialHeightAtInterface_A(params)"
      << std::endl;
}

std::string GeopotentialHeightAtInterface_A::name() const
{
    return GeopotentialHeightAtInterface_A::Name;
}

oops::Variable GeopotentialHeightAtInterface_A::product() const
{
    return oops::Variable{"geopotential_height_levels"};
}

oops::Variables GeopotentialHeightAtInterface_A::ingredients() const
{
    return GeopotentialHeightAtInterface_A::Ingredients;
}

size_t GeopotentialHeightAtInterface_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("geopotential_levels").shape(1);
}

atlas::FunctionSpace GeopotentialHeightAtInterface_A::productFunctionSpace(const atlas::FieldSet &
                                                                                    afieldset) const
{
    return afieldset.field("geopotential_levels").functionspace();
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightAtInterface_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering GeopotentialHeightAtInterface_A::executeNL function"
      << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](const double phi_int, double& z_int) {
        z_int = phi_int * inv_g;
      },
      afieldset["geopotential_levels"],
      afieldset["geopotential_height_levels"]);

  oops::Log::trace() << "leaving GeopotentialHeightAtInterface_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightAtInterface_A::executeTL(atlas::FieldSet & afieldsetTL,
                                          const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering GeopotentialHeightAtInterface_A::executeTL function"
      << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](const double phi_int_tl, double& z_int_tl) {
        z_int_tl = phi_int_tl * inv_g;
      },
      afieldsetTL["geopotential_levels"],
      afieldsetTL["geopotential_height_levels"]);

    oops::Log::trace() << "leaving GeopotentialHeightAtInterface_A::executeTL function"
      << std::endl;
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightAtInterface_A::executeAD(atlas::FieldSet & afieldsetAD,
                                          const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering GeopotentialHeightAtInterface_A::executeAD function"
      << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](double& phi_int_ad, double& z_int_ad) {
        phi_int_ad += z_int_ad * inv_g;
        z_int_ad = 0.0;
      },
      afieldsetAD["geopotential_levels"],
      afieldsetAD["geopotential_height_levels"]);

    oops::Log::trace() << "leaving GeopotentialHeightAtInterface_A::executeAD function"
      << std::endl;
}

}  // namespace vader
