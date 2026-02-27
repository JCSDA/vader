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
#include "vader/recipes/ParticulateMatter2p5.h"


namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char ParticulateMatter2p5_B::Name[] = "ParticulateMatter2p5_B";

const oops::Variables ParticulateMatter2p5_B::Ingredients{std::vector<std::string>{
                                   "dry_air_density",
                                   "mixing_ratio_of_smoke_wrt_dry_air",
                                   "mixing_ratio_of_dust_wrt_dry_air"}};

// Register the maker
static RecipeMaker<ParticulateMatter2p5_B>
         makerParticulateMatter2p5_B_(ParticulateMatter2p5_B::Name);

ParticulateMatter2p5_B::ParticulateMatter2p5_B(const Parameters_ & params,
                                    const VaderConfigVars & configVariables):
                                            configVariables_{configVariables}
{
    oops::Log::trace() << "ParticulateMatter2p5_B::ParticulateMatter2p5_B(params)" << std::endl;
}

std::string ParticulateMatter2p5_B::name() const
{
    return ParticulateMatter2p5_B::Name;
}

oops::Variable ParticulateMatter2p5_B::product() const
{
    return oops::Variable{"mass_density_of_particulate_matter_2p5_in_air"};
}

oops::Variables ParticulateMatter2p5_B::ingredients() const
{
    return ParticulateMatter2p5_B::Ingredients;
}

oops::Variables ParticulateMatter2p5_B::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"dry_air_density"}};
}

size_t ParticulateMatter2p5_B::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("mixing_ratio_of_smoke_wrt_dry_air").shape(1);
}

atlas::FunctionSpace ParticulateMatter2p5_B::productFunctionSpace
                                                (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("mixing_ratio_of_smoke_wrt_dry_air").functionspace();
}

// -------------------------------------------------------------------------------------------------

void ParticulateMatter2p5_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_B::executeNL function" << std::endl;

    util::for_each_value(
      [](const double airdens,
         const double mixing_ratio_of_smoke_wrt_dry_air,
         const double mixing_ratio_of_dust_wrt_dry_air,
         double& mass_density_of_particulate_matter_2p5_in_air) {
         mass_density_of_particulate_matter_2p5_in_air = airdens *
           (mixing_ratio_of_smoke_wrt_dry_air + mixing_ratio_of_dust_wrt_dry_air);
      },
      afieldset["dry_air_density"],
      afieldset["mixing_ratio_of_smoke_wrt_dry_air"],
      afieldset["mixing_ratio_of_dust_wrt_dry_air"],
      afieldset["mass_density_of_particulate_matter_2p5_in_air"]);

    oops::Log::trace() << "leaving ParticulateMatter2p5_B::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void ParticulateMatter2p5_B::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_B::executeTL function" << std::endl;

    util::for_each_value(
      [](const double airdens,
         const double tl_mixing_ratio_of_smoke_wrt_dry_air,
         const double tl_mixing_ratio_of_dust_wrt_dry_air,
         double& tl_mass_density_of_particulate_matter_2p5_in_air) {
           tl_mass_density_of_particulate_matter_2p5_in_air = airdens *
             (tl_mixing_ratio_of_smoke_wrt_dry_air + tl_mixing_ratio_of_dust_wrt_dry_air);
  },
      afieldsetTraj["dry_air_density"],
      afieldsetTL["mixing_ratio_of_smoke_wrt_dry_air"],
      afieldsetTL["mixing_ratio_of_dust_wrt_dry_air"],
      afieldsetTL["mass_density_of_particulate_matter_2p5_in_air"]);

    oops::Log::trace() << "leaving ParticulateMatter2p5_B::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void ParticulateMatter2p5_B::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_B::executeAD function" << std::endl;

    util::for_each_value(
      [](const double airdens,
         double& ad_mass_density_of_particulate_matter_2p5_in_air,
         double& ad_mixing_ratio_of_smoke_wrt_dry_air,
         double& ad_mixing_ratio_of_dust_wrt_dry_air) {
         ad_mixing_ratio_of_smoke_wrt_dry_air += airdens *
                     ad_mass_density_of_particulate_matter_2p5_in_air;
         ad_mixing_ratio_of_dust_wrt_dry_air  += airdens *
                     ad_mass_density_of_particulate_matter_2p5_in_air;
         ad_mass_density_of_particulate_matter_2p5_in_air = 0.0f;
      },
      afieldsetTraj["dry_air_density"],
      afieldsetAD["mass_density_of_particulate_matter_2p5_in_air"],
      afieldsetAD["mixing_ratio_of_smoke_wrt_dry_air"],
      afieldsetAD["mixing_ratio_of_dust_wrt_dry_air"]);

    oops::Log::trace() << "leaving ParticulateMatter2p5_B::executeAD function" << std::endl;
}


}  // namespace vader
