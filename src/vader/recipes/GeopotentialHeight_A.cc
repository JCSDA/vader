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
#include "vader/recipes/GeopotentialHeight.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char GeopotentialHeight_A::Name[] = "GeopotentialHeight_A";
const oops::Variables GeopotentialHeight_A::Ingredients{std::vector<std::string>{
                                                        "geopotential"}};

// Register the maker
static RecipeMaker<GeopotentialHeight_A> makerGeopotentialHeight_A_(GeopotentialHeight_A::Name);

GeopotentialHeight_A::GeopotentialHeight_A(const Parameters_ & params,
                                   const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "GeopotentialHeight_A::GeopotentialHeight_A(params)"
                       << configVariables_ << std::endl;
}

std::string GeopotentialHeight_A::name() const
{
    return GeopotentialHeight_A::Name;
}

oops::Variable GeopotentialHeight_A::product() const
{
    return oops::Variable{"geopotential_height"};
}

oops::Variables GeopotentialHeight_A::ingredients() const
{
    return GeopotentialHeight_A::Ingredients;
}

size_t GeopotentialHeight_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("geopotential").shape(1);
}

atlas::FunctionSpace GeopotentialHeight_A::productFunctionSpace(const atlas::FieldSet &
                                                                                    afieldset) const
{
    return afieldset.field("geopotential").functionspace();
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeight_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering GeopotentialHeight_A::executeNL function" << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](const double phi,
          double& z) {
          z = phi * inv_g;
      },
      afieldset["geopotential"],
      afieldset["geopotential_height"]);

  oops::Log::trace() << "leaving GeopotentialHeight_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeight_A::executeTL(atlas::FieldSet & afieldsetTL,
                               const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering GeopotentialHeight_A::executeTL function" << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](const double phi_tl,
          double& z_tl) {
          z_tl = phi_tl * inv_g;
      },
      afieldsetTL["geopotential"],
      afieldsetTL["geopotential_height"]);

  oops::Log::trace() << "leaving GeopotentialHeight_A::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeight_A::executeAD(atlas::FieldSet & afieldsetAD,
                               const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering GeopotentialHeight_A::executeAD function" << std::endl;

    // Extract values from client config
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double inv_g = 1.0 / grav;

    util::for_each_value(
      [&](double& phi_ad,
          double& z_ad) {
          phi_ad += z_ad * inv_g;
          z_ad = 0.0;
      },
      afieldsetAD["geopotential"],
      afieldsetAD["geopotential_height"]);

  oops::Log::trace() << "leaving GeopotentialHeight_A::executeAD function" << std::endl;
}

}  // namespace vader
