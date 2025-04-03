/*
 * (C) Copyright 2024 UCAR.
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
using std::vector;
using std::string;

namespace vader
{
// ------------------------------------------------------------------------------------------------
// Static attribute initialization
const char ParticulateMatter2p5_A::Name[] = "ParticulateMatter2p5_A";
// PM2.5 calculaiton with CMAQ AERO6
// Please change ivar_start (here it is 4 for aso4i) and imodes if you change the INGREDIENTS list.
const std::vector<std::string> AERO6_INGREDIENTS{
           "pm2p5_fraction_of_aitken_mode",
           "pm2p5_fraction_of_accumulation_mode",
           "pm2p5_fraction_of_coarse_mode",
           "dry_air_density",
           "mixing_ratio_of_aso4i_wrt_dry_air",
           "mixing_ratio_of_ano3i_wrt_dry_air",
           "mixing_ratio_of_anh4i_wrt_dry_air",
           "mixing_ratio_of_anai_wrt_dry_air",
           "mixing_ratio_of_acli_wrt_dry_air",
           "mixing_ratio_of_aeci_wrt_dry_air",
           "mixing_ratio_of_aothri_wrt_dry_air",
           "mixing_ratio_of_alvpo1i_wrt_dry_air",
           "mixing_ratio_of_asvpo1i_wrt_dry_air",
           "mixing_ratio_of_asvpo2i_wrt_dry_air",
           "mixing_ratio_of_alvoo1i_wrt_dry_air",
           "mixing_ratio_of_alvoo2i_wrt_dry_air",
           "mixing_ratio_of_asvoo1i_wrt_dry_air",
           "mixing_ratio_of_asvoo2i_wrt_dry_air",
           "mixing_ratio_of_aso4j_wrt_dry_air",
           "mixing_ratio_of_ano3j_wrt_dry_air",
           "mixing_ratio_of_anh4j_wrt_dry_air",
           "mixing_ratio_of_anaj_wrt_dry_air",
           "mixing_ratio_of_aclj_wrt_dry_air",
           "mixing_ratio_of_aecj_wrt_dry_air",
           "mixing_ratio_of_aothrj_wrt_dry_air",
           "mixing_ratio_of_afej_wrt_dry_air",
           "mixing_ratio_of_asij_wrt_dry_air",
           "mixing_ratio_of_atij_wrt_dry_air",
           "mixing_ratio_of_acaj_wrt_dry_air",
           "mixing_ratio_of_amgj_wrt_dry_air",
           "mixing_ratio_of_amnj_wrt_dry_air",
           "mixing_ratio_of_aalj_wrt_dry_air",
           "mixing_ratio_of_akj_wrt_dry_air",
           "mixing_ratio_of_alvpo1j_wrt_dry_air",
           "mixing_ratio_of_asvpo1j_wrt_dry_air",
           "mixing_ratio_of_asvpo2j_wrt_dry_air",
           "mixing_ratio_of_asvpo3j_wrt_dry_air",
           "mixing_ratio_of_aivpo1j_wrt_dry_air",
           "mixing_ratio_of_axyl1j_wrt_dry_air",
           "mixing_ratio_of_axyl2j_wrt_dry_air",
           "mixing_ratio_of_axyl3j_wrt_dry_air",
           "mixing_ratio_of_atol1j_wrt_dry_air",
           "mixing_ratio_of_atol2j_wrt_dry_air",
           "mixing_ratio_of_atol3j_wrt_dry_air",
           "mixing_ratio_of_abnz1j_wrt_dry_air",
           "mixing_ratio_of_abnz2j_wrt_dry_air",
           "mixing_ratio_of_abnz3j_wrt_dry_air",
           "mixing_ratio_of_aiso1j_wrt_dry_air",
           "mixing_ratio_of_aiso2j_wrt_dry_air",
           "mixing_ratio_of_aiso3j_wrt_dry_air",
           "mixing_ratio_of_atrp1j_wrt_dry_air",
           "mixing_ratio_of_atrp2j_wrt_dry_air",
           "mixing_ratio_of_asqtj_wrt_dry_air",
           "mixing_ratio_of_aalk1j_wrt_dry_air",
           "mixing_ratio_of_aalk2j_wrt_dry_air",
           "mixing_ratio_of_apah1j_wrt_dry_air",
           "mixing_ratio_of_apah2j_wrt_dry_air",
           "mixing_ratio_of_apah3j_wrt_dry_air",
           "mixing_ratio_of_aorgcj_wrt_dry_air",
           "mixing_ratio_of_aolgbj_wrt_dry_air",
           "mixing_ratio_of_aolgaj_wrt_dry_air",
           "mixing_ratio_of_alvoo1j_wrt_dry_air",
           "mixing_ratio_of_alvoo2j_wrt_dry_air",
           "mixing_ratio_of_asvoo1j_wrt_dry_air",
           "mixing_ratio_of_asvoo2j_wrt_dry_air",
           "mixing_ratio_of_asvoo3j_wrt_dry_air",
           "mixing_ratio_of_apcsoj_wrt_dry_air",
           "mixing_ratio_of_aso4k_wrt_dry_air",
           "mixing_ratio_of_asoil_wrt_dry_air",
           "mixing_ratio_of_acors_wrt_dry_air",
           "mixing_ratio_of_aseacat_wrt_dry_air",
           "mixing_ratio_of_aclk_wrt_dry_air",
           "mixing_ratio_of_ano3k_wrt_dry_air",
           "mixing_ratio_of_anh4k_wrt_dry_air"};
const oops::Variables ParticulateMatter2p5_A::Ingredients{AERO6_INGREDIENTS};
vector<int>  imodes = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                       2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                       2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3};
// Register the maker
static RecipeMaker<ParticulateMatter2p5_A>
         makerParticulateMatter2p5_A_(ParticulateMatter2p5_A::Name);

ParticulateMatter2p5_A::ParticulateMatter2p5_A(const Parameters_ & params,
                                    const VaderConfigVars & configVariables):
                                            configVariables_{configVariables}
{
    oops::Log::trace() << "ParticulateMatter2p5_A::ParticulateMatter2p5_A(params)"
        << std::endl;
}

std::string ParticulateMatter2p5_A::name() const
{
    return ParticulateMatter2p5_A::Name;
}

oops::Variable ParticulateMatter2p5_A::product() const
{
    return oops::Variable{"mass_density_of_particulate_matter_2p5_in_air"};
}

oops::Variables ParticulateMatter2p5_A::ingredients() const
{
    return ParticulateMatter2p5_A::Ingredients;
}

oops::Variables ParticulateMatter2p5_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"pm2p5_fraction_of_aitken_mode",
           "pm2p5_fraction_of_accumulation_mode",
           "pm2p5_fraction_of_coarse_mode",
           "dry_air_density"
    }};
}

size_t ParticulateMatter2p5_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field(AERO6_INGREDIENTS[4]).shape(1);
}

atlas::FunctionSpace ParticulateMatter2p5_A::productFunctionSpace
                                                (const atlas::FieldSet & afieldset) const
{
    return afieldset.field(AERO6_INGREDIENTS[4]).functionspace();
}

void ParticulateMatter2p5_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_A::executeNL function"
        << std::endl;

    atlas::Field pm25at = afieldset.field(AERO6_INGREDIENTS[0]);
    atlas::Field pm25ac = afieldset.field(AERO6_INGREDIENTS[1]);
    atlas::Field pm25co = afieldset.field(AERO6_INGREDIENTS[2]);
    atlas::Field airdens = afieldset.field(AERO6_INGREDIENTS[3]);
    atlas::Field mass_density_of_particulate_matter_2p5_in_air =
             afieldset.field("mass_density_of_particulate_matter_2p5_in_air");

    auto pm25at_view = atlas::array::make_view<double, 2>(pm25at);
    auto pm25ac_view = atlas::array::make_view<double, 2>(pm25ac);
    auto pm25co_view = atlas::array::make_view<double, 2>(pm25co);
    auto airdens_view = atlas::array::make_view<double, 2>(airdens);

    auto mass_density_of_particulate_matter_2p5_in_air_view =
           atlas::array::make_view<double, 2>(mass_density_of_particulate_matter_2p5_in_air);

    int nlevels = pm25at.shape(1);
    size_t grid_size = pm25at.size()/nlevels;
    int nvars = AERO6_INGREDIENTS.size();
//  Assign array position where the first aerosal variable starts
    int ivar_start = 4;

//  Initilaze returned output variable
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) = 0.0;
      }
    }

    for (int ivar = ivar_start; ivar < nvars; ++ivar) {
      atlas::Field tmp = afieldset.field(AERO6_INGREDIENTS[ivar]);
      auto tmp_view = atlas::array::make_view<double, 2>(tmp);
      for (int level = 0; level < nlevels; ++level) {
        for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
          switch (imodes[ivar]) {
          case 1:
            mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) +=
              tmp_view(jnode, level)*pm25at_view(jnode, level)*airdens_view(jnode, level);
            break;
          case 2:
            mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) +=
              tmp_view(jnode, level)*pm25ac_view(jnode, level)*airdens_view(jnode, level);
            break;
          case 3:
            mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) +=
              tmp_view(jnode, level)*pm25co_view(jnode, level)*airdens_view(jnode, level);
            break;
          default:
            oops::Log::trace() << "ParticulateMatter2p5_A::execute: imodes must be [1, 2, 3]!"
                               << (imodes[ivar]) << std::endl;
            throw eckit::Exception(
                         "ParticulateMatter2p5_A:: imodes value is invalid. STOP!!!", Here());
          }
        }
      }
    }
    oops::Log::trace() << "leaving ParticulateMatter2p5_A::executeNL function" << std::endl;
}

void ParticulateMatter2p5_A::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_A::executeTL function"
        << std::endl;

    atlas::Field pm25at = afieldsetTraj.field(AERO6_INGREDIENTS[0]);
    atlas::Field pm25ac = afieldsetTraj.field(AERO6_INGREDIENTS[1]);
    atlas::Field pm25co = afieldsetTraj.field(AERO6_INGREDIENTS[2]);
    atlas::Field airdens = afieldsetTraj.field(AERO6_INGREDIENTS[3]);

    atlas::Field tl_mass_density_of_particulate_matter_2p5_in_air =
      afieldsetTL.field("mass_density_of_particulate_matter_2p5_in_air");

    auto pm25at_view = atlas::array::make_view<double, 2>(pm25at);
    auto pm25ac_view = atlas::array::make_view<double, 2>(pm25ac);
    auto pm25co_view = atlas::array::make_view<double, 2>(pm25co);
    auto airdens_view = atlas::array::make_view<double, 2>(airdens);

    auto tl_mass_density_of_particulate_matter_2p5_in_air_view =
      atlas::array::make_view<double, 2>(tl_mass_density_of_particulate_matter_2p5_in_air);


    int nlevels = pm25at.shape(1);
    size_t grid_size = pm25at.size()/nlevels;
    int nvars = AERO6_INGREDIENTS.size();
//  Assign array position where the first aerosal variable starts
    int ivar_start = 4;

    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        tl_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) = 0.0;
      }
    }

    for (int ivar = ivar_start; ivar < nvars; ++ivar) {
      atlas::Field tmp = afieldsetTL.field(AERO6_INGREDIENTS[ivar]);
      auto tmp_view = atlas::array::make_view<double, 2>(tmp);
      for (int level = 0; level < nlevels; ++level) {
        for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
          switch (imodes[ivar]) {
          case 1:
            tl_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) +=
              tmp_view(jnode, level)*pm25at_view(jnode, level)*airdens_view(jnode, level);
            break;
          case 2:
            tl_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) +=
              tmp_view(jnode, level)*pm25ac_view(jnode, level)*airdens_view(jnode, level);
            break;
          case 3:
            tl_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) +=
              tmp_view(jnode, level)*pm25co_view(jnode, level)*airdens_view(jnode, level);
            break;
          default:
            oops::Log::trace() << "ParticulateMatter2p5_A::execute: imodes must be [1, 2 ,3]!"
                               << (imodes[ivar]) << std::endl;
            throw eckit::Exception(
                         "ParticulateMatter2p5_A:: imodes value is invalid. STOP!!!", Here());
          }
        }
      }
    }
    oops::Log::trace() << "leaving ParticulateMatter2p5_A::executeTL function" << std::endl;
}


void ParticulateMatter2p5_A::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_A::executeAD function"
        << std::endl;

    atlas::Field pm25at = afieldsetTraj.field(AERO6_INGREDIENTS[0]);
    atlas::Field pm25ac = afieldsetTraj.field(AERO6_INGREDIENTS[1]);
    atlas::Field pm25co = afieldsetTraj.field(AERO6_INGREDIENTS[2]);
    atlas::Field airdens = afieldsetTraj.field(AERO6_INGREDIENTS[3]);

    atlas::Field ad_mass_density_of_particulate_matter_2p5_in_air =
                   afieldsetAD.field("mass_density_of_particulate_matter_2p5_in_air");


    auto pm25at_view = atlas::array::make_view<double, 2>(pm25at);
    auto pm25ac_view = atlas::array::make_view<double, 2>(pm25ac);
    auto pm25co_view = atlas::array::make_view<double, 2>(pm25co);
    auto airdens_view = atlas::array::make_view<double, 2>(airdens);

    auto ad_mass_density_of_particulate_matter_2p5_in_air_view =
           atlas::array::make_view<double, 2>(ad_mass_density_of_particulate_matter_2p5_in_air);

    int nlevels = pm25at.shape(1);
    size_t grid_size = pm25at.size()/nlevels;
    int nvars = AERO6_INGREDIENTS.size();
//  Assign array position where the first aerosal variable starts
    int ivar_start = 4;

    for (int ivar = ivar_start; ivar < nvars; ++ivar) {
      atlas::Field tmp = afieldsetAD.field(AERO6_INGREDIENTS[ivar]);
      auto tmp_view = atlas::array::make_view<double, 2>(tmp);
      for (int level = 0; level < nlevels; ++level) {
        for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
          switch (imodes[ivar]) {
          case 1:
            tmp_view(jnode, level) +=
              ad_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level)*
              pm25at_view(jnode, level)*airdens_view(jnode, level);
            break;
          case 2:
            tmp_view(jnode, level) +=
              ad_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level)*
              pm25ac_view(jnode, level)*airdens_view(jnode, level);
            break;
          case 3:
            tmp_view(jnode, level) +=
              ad_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level)*
              pm25co_view(jnode, level)*airdens_view(jnode, level);
            break;
          default:
            oops::Log::trace() << "ParticulateMatter2p5_A::execute: imodes must be [1, 2, 3]!"
                               << (imodes[ivar]) << std::endl;
            throw eckit::Exception(
                         "ParticulateMatter2p5_A:: imodes value is invalid. STOP!!!", Here());
          }
        }
      }
    }

    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        ad_mass_density_of_particulate_matter_2p5_in_air_view(jnode, level) = 0.0f;
      }
    }
    oops::Log::trace() << "leaving ParticulateMatter2p5_A::executeAD function" << std::endl;
}

}  // namespace vader
