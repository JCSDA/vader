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
#include "vader/recipes/HeightAboveMeanSeaLevel.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char HeightAboveMeanSeaLevel_A::Name[] = "HeightAboveMeanSeaLevel_A";
const oops::Variables HeightAboveMeanSeaLevel_A::Ingredients{std::vector<std::string>{
                                                        "geopotential_height"}};

// Register the maker
static RecipeMaker<HeightAboveMeanSeaLevel_A> makerHeightAboveMeanSeaLevel_A_(
                                                         HeightAboveMeanSeaLevel_A::Name);

HeightAboveMeanSeaLevel_A::HeightAboveMeanSeaLevel_A(const Parameters_ & params,
                                   const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "HeightAboveMeanSeaLevel_A::"
                       << "HeightAboveMeanSeaLevel_A(params)"
                       << std::endl;
}

std::string HeightAboveMeanSeaLevel_A::name() const
{
    return HeightAboveMeanSeaLevel_A::Name;
}

oops::Variable HeightAboveMeanSeaLevel_A::product() const
{
    return oops::Variable{"height_above_mean_sea_level"};
}

oops::Variables HeightAboveMeanSeaLevel_A::ingredients() const
{
    return HeightAboveMeanSeaLevel_A::Ingredients;
}

size_t HeightAboveMeanSeaLevel_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("geopotential_height").shape(1);
}

atlas::FunctionSpace HeightAboveMeanSeaLevel_A::productFunctionSpace(const
                                                                 atlas::FieldSet & afieldset) const
{
    return afieldset.field("geopotential_height").functionspace();
}

// -------------------------------------------------------------------------------------------------

void HeightAboveMeanSeaLevel_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering HeightAboveMeanSeaLevel_A::executeNL" << std::endl;

    util::for_each_value(
      [](const double z,
         double& z_sea) {
         z_sea = z;
      },
      afieldset["geopotential_height"],
      afieldset["height_above_mean_sea_level"]);

    oops::Log::trace() << "leaving HeightAboveMeanSeaLevel_A::executeNL" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void HeightAboveMeanSeaLevel_A::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering HeightAboveMeanSeaLevel_A::executeTL function"
      << std::endl;

    util::for_each_value(
      [](const double z_tl,
         double& z_sea_tl) {
         z_sea_tl = z_tl;
      },
      afieldsetTL["geopotential_height"],
      afieldsetTL["height_above_mean_sea_level"]);

    oops::Log::trace() << "leaving HeightAboveMeanSeaLevel_A::executeTL function"
      << std::endl;
}

// -------------------------------------------------------------------------------------------------

void HeightAboveMeanSeaLevel_A::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering HeightAboveMeanSeaLevel_A::executeAD function"
      << std::endl;

    util::for_each_value(
      [](double& z_ad,
         double& z_sea_ad) {
         z_ad += z_sea_ad;
         z_sea_ad = 0.0;
      },
      afieldsetAD["geopotential_height"],
      afieldsetAD["height_above_mean_sea_level"]);

    oops::Log::trace() << "leaving HeightAboveMeanSeaLevel_A::executeAD function"
      << std::endl;
}

}  // namespace vader
