/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <vector>

#include "oops/util/abor1_cpp.h"
#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/GeopotentialHeightLevels.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char GeopotentialHeightLevels_B::Name[] = "GeopotentialHeightLevels_B";
const oops::Variables GeopotentialHeightLevels_B::Ingredients{
    std::vector<std::string>{
    "geopotential_height_at_surface",
    "virtual_temperature",
    "ln_air_pressure_at_interface"}};

// Register the maker
static RecipeMaker<GeopotentialHeightLevels_B> makerGeopotentialHeightLevels_B_(
    GeopotentialHeightLevels_B::Name);

GeopotentialHeightLevels_B::GeopotentialHeightLevels_B(const Parameters_ & params,
                                                     const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "GeopotentialHeightLevels_B::GeopotentialHeightLevels_B(params)"
        << std::endl;
}

std::string GeopotentialHeightLevels_B::name() const
{
    return GeopotentialHeightLevels_B::Name;
}

oops::Variable GeopotentialHeightLevels_B::product() const
{
    return oops::Variable{"geopotential_height_levels"};
}

oops::Variables GeopotentialHeightLevels_B::ingredients() const
{
    return GeopotentialHeightLevels_B::Ingredients;
}

oops::Variables GeopotentialHeightLevels_B::trajectoryVars() const
{
  return oops::Variables{std::vector<std::string>{"ln_air_pressure_at_interface"}};
}

size_t GeopotentialHeightLevels_B::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("ln_air_pressure_at_interface").shape(1);
}
atlas::FunctionSpace GeopotentialHeightLevels_B::productFunctionSpace(const atlas::FieldSet &
                                                                                    afieldset) const
{
    return afieldset.field("ln_air_pressure_at_interface").functionspace();
}

// -------------------------------------------------------------------------------------------------

void GeopotentialHeightLevels_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering GeopotentialHeightLevels_B::executeNL function"
                       << std::endl;

    const double rdry = configVariables_.getDouble("gas_constant_of_dry_air");
    const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
    const double rovg = rdry/grav;

    atlas::Field z_surf = afieldset.field("geopotential_height_at_surface");
    atlas::Field tv = afieldset.field("virtual_temperature");
    atlas::Field ln_p_int = afieldset.field("ln_air_pressure_at_interface");
    atlas::Field z_int = afieldset.field("geopotential_height_levels");

    const int nlev = tv.shape(1);
    const int nint = ln_p_int.shape(1);

    if (nlev < 2 || nint < 2) {
        oops::Log::error() << "GeopotentialHeightLevels_B::executeNL: "
               "need at least 2 full levels and 2 interfaces" << std::endl;
        ABORT("GeopotentialHeightLevels_B::executeNL: "
              "need at least 2 full levels and 2 interfaces");
    }

    if (nint != nlev + 1) {
        oops::Log::error() << "GeopotentialHeightLevels_B::executeNL: "
               "number of interfaces must be one more than the number of full levels"
                           << std::endl;
        ABORT("GeopotentialHeightLevels_B::executeNL: number of interfaces "
              "must be one more than the number of full levels");
    }

    util::for_each_column(
    [&](const auto z_surf_col,
        const auto tv_col,
        const auto ln_p_int_col,
        auto z_int_col) {
        // Index 0 is the surface interface; integrate upward with increasing index.
        z_int_col(0) = z_surf_col(0);

        for (int k = 0; k < nint-1; ++k) {
            double dln = ln_p_int_col(k+1) - ln_p_int_col(k);
            z_int_col(k + 1) = z_int_col(k) - rovg * tv_col(k) * dln;
        }
    },
    z_surf,
    tv,
    ln_p_int,
    z_int);

    oops::Log::trace() << "leaving GeopotentialHeightLevels_B::executeNL function"
                       << std::endl;
}

// ------------------------------------------------------------------------------------------------

}  // namespace vader
