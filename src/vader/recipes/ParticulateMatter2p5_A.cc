/*
 * (C) Copyright 2024 UCAR.
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

// -------------------------------------------------------------------------------------------------

void ParticulateMatter2p5_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_A::executeNL function"
        << std::endl;

    //  Number of aerosol variables
    const int nvars = AERO6_INGREDIENTS.size();
    //  Assign index where the first aerosol variable starts
    const int ivar_start = 4;

    // Initialize the output variable at 0.0
    util::for_each_value(
      [](double& mass_density_of_particulate_matter_2p5_in_air) {
         mass_density_of_particulate_matter_2p5_in_air = 0.0;
      },
      afieldset["mass_density_of_particulate_matter_2p5_in_air"]);

    for (int ivar = ivar_start; ivar < nvars; ++ivar) {
      util::for_each_value(
        [&](const double ingredient_var,
            const double pm25at,
            const double pm25ac,
            const double pm25co,
            const double airdens,
            double& mass_density_of_particulate_matter_2p5_in_air) {
            switch (imodes[ivar]) {
            case 1:
              mass_density_of_particulate_matter_2p5_in_air +=
                ingredient_var * pm25at * airdens;
              break;
            case 2:
              mass_density_of_particulate_matter_2p5_in_air +=
                ingredient_var * pm25ac * airdens;
              break;
            case 3:
              mass_density_of_particulate_matter_2p5_in_air +=
                ingredient_var * pm25co * airdens;
              break;
            default:
              oops::Log::trace() << "ParticulateMatter2p5_A::execute: imodes must be [1, 2, 3]!"
                                << (imodes[ivar]) << std::endl;
              throw eckit::Exception(
                          "ParticulateMatter2p5_A:: imodes value is invalid. STOP!!!", Here());
            }
            },
        afieldset[AERO6_INGREDIENTS[ivar]],
        afieldset[AERO6_INGREDIENTS[0]],
        afieldset[AERO6_INGREDIENTS[1]],
        afieldset[AERO6_INGREDIENTS[2]],
        afieldset[AERO6_INGREDIENTS[3]],
        afieldset["mass_density_of_particulate_matter_2p5_in_air"]);
    }
    oops::Log::trace() << "leaving ParticulateMatter2p5_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void ParticulateMatter2p5_A::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_A::executeTL function"
        << std::endl;

    //  Number of aerosol variables
    const int nvars = AERO6_INGREDIENTS.size();
    //  Assign array position where the first aerosol variable starts
    const int ivar_start = 4;

    // Initialize the output variable at 0.0
    util::for_each_value(
      [](double& tl_mass_density_of_particulate_matter_2p5_in_air) {
         tl_mass_density_of_particulate_matter_2p5_in_air = 0.0;
      },
      afieldsetTL["mass_density_of_particulate_matter_2p5_in_air"]);

    for (int ivar = ivar_start; ivar < nvars; ++ivar) {
      util::for_each_value(
        [&](const double pm25at,
            const double pm25ac,
            const double pm25co,
            const double airdens,
            const double tl_ingredient_var,
            double& tl_mass_density_of_particulate_matter_2p5_in_air) {
            switch (imodes[ivar]) {
            case 1:
              tl_mass_density_of_particulate_matter_2p5_in_air +=
                tl_ingredient_var * pm25at * airdens;
              break;
            case 2:
              tl_mass_density_of_particulate_matter_2p5_in_air +=
                tl_ingredient_var * pm25ac * airdens;
              break;
            case 3:
              tl_mass_density_of_particulate_matter_2p5_in_air +=
                tl_ingredient_var * pm25co * airdens;
              break;
            default:
              oops::Log::trace() << "ParticulateMatter2p5_A::execute: imodes must be [1, 2 ,3]!"
                                << (imodes[ivar]) << std::endl;
              throw eckit::Exception(
                          "ParticulateMatter2p5_A:: imodes value is invalid. STOP!!!", Here());
          }
        },
        afieldsetTraj[AERO6_INGREDIENTS[0]],
        afieldsetTraj[AERO6_INGREDIENTS[1]],
        afieldsetTraj[AERO6_INGREDIENTS[2]],
        afieldsetTraj[AERO6_INGREDIENTS[3]],
        afieldsetTL[AERO6_INGREDIENTS[ivar]],
        afieldsetTL["mass_density_of_particulate_matter_2p5_in_air"]);
    }

    oops::Log::trace() << "leaving ParticulateMatter2p5_A::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void ParticulateMatter2p5_A::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering ParticulateMatter2p5_A::executeAD function"
        << std::endl;

    //  Number of aerosol variables
    const int nvars = AERO6_INGREDIENTS.size();
    //  Assign array position where the first aerosol variable starts
    const int ivar_start = 4;

    for (int ivar = ivar_start; ivar < nvars; ++ivar) {
      oops::Log::info() << "entering ParticulateMatter2p5_A::executeAD function for variable "
                         << AERO6_INGREDIENTS[ivar] << std::endl;
      util::for_each_value(
        [&](const double pm25at,
            const double pm25ac,
            const double pm25co,
            const double airdens,
            const double ad_mass_density_of_particulate_matter_2p5_in_air,
            double& ad_ingredient_var) {
            switch (imodes[ivar]) {
            case 1:
              ad_ingredient_var +=
                ad_mass_density_of_particulate_matter_2p5_in_air * pm25at * airdens;
              break;
            case 2:
              ad_ingredient_var +=
                ad_mass_density_of_particulate_matter_2p5_in_air * pm25ac * airdens;
              break;
            case 3:
              ad_ingredient_var +=
                ad_mass_density_of_particulate_matter_2p5_in_air * pm25co * airdens;
              break;
            default:
              oops::Log::trace() << "ParticulateMatter2p5_A::execute: imodes must be [1, 2, 3]!"
                                  << (imodes[ivar]) << std::endl;
              throw eckit::Exception(
                            "ParticulateMatter2p5_A:: imodes value is invalid. STOP!!!", Here());
            }
        },
        afieldsetTraj[AERO6_INGREDIENTS[0]],
        afieldsetTraj[AERO6_INGREDIENTS[1]],
        afieldsetTraj[AERO6_INGREDIENTS[2]],
        afieldsetTraj[AERO6_INGREDIENTS[3]],
        afieldsetAD["mass_density_of_particulate_matter_2p5_in_air"],
        afieldsetAD[AERO6_INGREDIENTS[ivar]]);
    }

    util::for_each_value(
      [](double& ad_mass_density_of_particulate_matter_2p5_in_air) {
         ad_mass_density_of_particulate_matter_2p5_in_air = 0.0;
      },
      afieldsetAD["mass_density_of_particulate_matter_2p5_in_air"]);


    oops::Log::trace() << "leaving ParticulateMatter2p5_A::executeAD function" << std::endl;
}

}  // namespace vader
