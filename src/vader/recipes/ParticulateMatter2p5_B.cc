/*
 * (C) Copyright 2024  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
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

void ParticulateMatter2p5_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_B::executeNL function" << std::endl;

    atlas::Field airdens = afieldset.field("dry_air_density");
    atlas::Field mixing_ratio_of_smoke_wrt_dry_air =
                   afieldset.field("mixing_ratio_of_smoke_wrt_dry_air");
    atlas::Field mixing_ratio_of_dust_wrt_dry_air =
                   afieldset.field("mixing_ratio_of_dust_wrt_dry_air");
    atlas::Field mass_density_of_particulate_matter_2p5_in_air =
                   afieldset.field("mass_density_of_particulate_matter_2p5_in_air");

    auto airdens_view = atlas::array::make_view<double, 2>(airdens);
    auto mixing_ratio_of_smoke_wrt_dry_air_view =
           atlas::array::make_view<double, 2>(mixing_ratio_of_smoke_wrt_dry_air);
    auto mixing_ratio_of_dust_wrt_dry_air_view  =
           atlas::array::make_view<double, 2>(mixing_ratio_of_dust_wrt_dry_air);
    auto mass_density_of_particulate_matter_2p5_in_air_view =
           atlas::array::make_view<double, 2>(mass_density_of_particulate_matter_2p5_in_air);

    int nlevels = mixing_ratio_of_smoke_wrt_dry_air.shape(1);
    size_t grid_size = mixing_ratio_of_smoke_wrt_dry_air.size()/nlevels;

    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) =
                            airdens_view(jnode, level)*
                            (mixing_ratio_of_smoke_wrt_dry_air_view(jnode, level)
                            + mixing_ratio_of_dust_wrt_dry_air_view(jnode, level));
      }
    }
    oops::Log::trace() << "leaving ParticulateMatter2p5_B::executeNL function" << std::endl;
}

void ParticulateMatter2p5_B::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_B::executeTL function" << std::endl;

    atlas::Field airdens = afieldsetTraj.field("dry_air_density");
    auto airdens_view = atlas::array::make_view<double, 2>(airdens);

    atlas::Field tl_mixing_ratio_of_smoke_wrt_dry_air =
             afieldsetTL.field("mixing_ratio_of_smoke_wrt_dry_air");
    auto tl_mixing_ratio_of_smoke_wrt_dry_air_view =
           atlas::array::make_view<double, 2>(tl_mixing_ratio_of_smoke_wrt_dry_air);

    atlas::Field tl_mixing_ratio_of_dust_wrt_dry_air =
             afieldsetTL.field("mixing_ratio_of_dust_wrt_dry_air");
    auto tl_mixing_ratio_of_dust_wrt_dry_air_view =
           atlas::array::make_view<double, 2>(tl_mixing_ratio_of_dust_wrt_dry_air);


    atlas::Field tl_mass_density_of_particulate_matter_2p5_in_air =
             afieldsetTL.field("mass_density_of_particulate_matter_2p5_in_air");
    auto tl_mass_density_of_particulate_matter_2p5_in_air_view =
           atlas::array::make_view<double, 2>(tl_mass_density_of_particulate_matter_2p5_in_air);

    int nlevels = tl_mixing_ratio_of_smoke_wrt_dry_air.shape(1);
    size_t grid_size = tl_mixing_ratio_of_smoke_wrt_dry_air.size()/nlevels;

    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        tl_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) =
                         airdens_view(jnode, level)*
                        (tl_mixing_ratio_of_smoke_wrt_dry_air_view(jnode, level)
                        + tl_mixing_ratio_of_dust_wrt_dry_air_view(jnode, level));
      }
    }
    oops::Log::trace() << "leaving ParticulateMatter2p5_B::executeTL function" << std::endl;
}


void ParticulateMatter2p5_B::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_B::executeAD function" << std::endl;

    atlas::Field airdens = afieldsetTraj.field("dry_air_density");
    auto airdens_view = atlas::array::make_view<double, 2>(airdens);

    atlas::Field ad_mixing_ratio_of_smoke_wrt_dry_air =
             afieldsetAD.field("mixing_ratio_of_smoke_wrt_dry_air");
    auto ad_mixing_ratio_of_smoke_wrt_dry_air_view =
           atlas::array::make_view<double, 2>(ad_mixing_ratio_of_smoke_wrt_dry_air);

    atlas::Field ad_mixing_ratio_of_dust_wrt_dry_air =
             afieldsetAD.field("mixing_ratio_of_dust_wrt_dry_air");
    auto ad_mixing_ratio_of_dust_wrt_dry_air_view =
           atlas::array::make_view<double, 2>(ad_mixing_ratio_of_dust_wrt_dry_air);

    atlas::Field ad_mass_density_of_particulate_matter_2p5_in_air =
             afieldsetAD.field("mass_density_of_particulate_matter_2p5_in_air");
    auto ad_mass_density_of_particulate_matter_2p5_in_air_view =
           atlas::array::make_view<double, 2>(ad_mass_density_of_particulate_matter_2p5_in_air);

    int nlevels = ad_mixing_ratio_of_smoke_wrt_dry_air.shape(1);
    size_t grid_size = ad_mixing_ratio_of_smoke_wrt_dry_air.size()/nlevels;

    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        ad_mixing_ratio_of_smoke_wrt_dry_air_view(jnode, level) += airdens_view(jnode, level)*
                     ad_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level);
        ad_mixing_ratio_of_dust_wrt_dry_air_view(jnode, level)  += airdens_view(jnode, level)*
                     ad_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level);
        ad_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) = 0.0f;
      }
    }
    oops::Log::trace() << "leaving ParticulateMatter2p5_B::executeAD function" << std::endl;
}


}  // namespace vader
