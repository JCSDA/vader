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
#include "vader/recipes/HeightAboveMeanSeaLevelAtSurface.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char HeightAboveMeanSeaLevelAtSurface_A::Name[] = "HeightAboveMeanSeaLevelAtSurface_A";
const oops::Variables HeightAboveMeanSeaLevelAtSurface_A::Ingredients{std::vector<std::string>{
                                                        "geopotential_height_at_surface"}};

// Register the maker
static RecipeMaker<HeightAboveMeanSeaLevelAtSurface_A> makerHeightAboveMeanSeaLevelAtSurface_A_(
                                                         HeightAboveMeanSeaLevelAtSurface_A::Name);

HeightAboveMeanSeaLevelAtSurface_A::HeightAboveMeanSeaLevelAtSurface_A(const Parameters_ & params,
                                   const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "HeightAboveMeanSeaLevelAtSurface_A::"
                       << "HeightAboveMeanSeaLevelAtSurface_A(params)"
                       << std::endl;
}

std::string HeightAboveMeanSeaLevelAtSurface_A::name() const
{
    return HeightAboveMeanSeaLevelAtSurface_A::Name;
}

oops::Variable HeightAboveMeanSeaLevelAtSurface_A::product() const
{
    return oops::Variable{"height_above_mean_sea_level_at_surface"};
}

oops::Variables HeightAboveMeanSeaLevelAtSurface_A::ingredients() const
{
    return HeightAboveMeanSeaLevelAtSurface_A::Ingredients;
}

size_t HeightAboveMeanSeaLevelAtSurface_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return 1;
}

atlas::FunctionSpace HeightAboveMeanSeaLevelAtSurface_A::productFunctionSpace(const
                                                                 atlas::FieldSet & afieldset) const
{
    return afieldset.field("geopotential_height_at_surface").functionspace();
}

// -------------------------------------------------------------------------------------------------

void HeightAboveMeanSeaLevelAtSurface_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering HeightAboveMeanSeaLevelAtSurface_A::executeNL" << std::endl;

    util::for_each_value(
      [](const double z_surf,
         double& z_surf_sea) {
         z_surf_sea = z_surf;
      },
      afieldset["geopotential_height_at_surface"],
      afieldset["height_above_mean_sea_level_at_surface"]);

    oops::Log::trace() << "leaving HeightAboveMeanSeaLevelAtSurface_A::executeNL" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void HeightAboveMeanSeaLevelAtSurface_A::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering HeightAboveMeanSeaLevelAtSurface_A::executeTL function"
      << std::endl;

    util::for_each_value(
      [](const double z_surf_tl,
         double& z_surf_sea_tl) {
         z_surf_sea_tl = z_surf_tl;
      },
      afieldsetTL["geopotential_height_at_surface"],
      afieldsetTL["height_above_mean_sea_level_at_surface"]);

    oops::Log::trace() << "leaving HeightAboveMeanSeaLevelAtSurface_A::executeTL function"
      << std::endl;
}

// -------------------------------------------------------------------------------------------------

void HeightAboveMeanSeaLevelAtSurface_A::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering HeightAboveMeanSeaLevelAtSurface_A::executeAD function"
      << std::endl;

    util::for_each_value(
      [](double& z_surf_ad,
         double& z_surf_sea_ad) {
         z_surf_ad += z_surf_sea_ad;
         z_surf_sea_ad = 0.0;
      },
      afieldsetAD["geopotential_height_at_surface"],
      afieldsetAD["height_above_mean_sea_level_at_surface"]);

    oops::Log::trace() << "leaving HeightAboveMeanSeaLevelAtSurface_A::executeAD function"
      << std::endl;
}

}  // namespace vader
