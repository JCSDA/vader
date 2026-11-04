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
#include "vader/recipes/DryAirDensity.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DryAirDensity_A::Name[] = "DryAirDensity_A";
// Note that water_vapor_mixing_ratio_wrt_moist_air is referred to as "specific humidity"
// by some groups.
const oops::Variables DryAirDensity_A::
    Ingredients{std::vector<std::string>{"air_temperature", "air_pressure",
                                         "water_vapor_mixing_ratio_wrt_moist_air"}};

// Register the maker
static RecipeMaker<DryAirDensity_A>
    makerDryAirDensity_A_(DryAirDensity_A::Name);

DryAirDensity_A::DryAirDensity_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables) :
        configVariables_{configVariables}

{
    oops::Log::trace() <<
        "DryAirDensity_A::DryAirDensity_A(params)" << std::endl;
}

std::string DryAirDensity_A::name() const
{
    return DryAirDensity_A::Name;
}

oops::Variable DryAirDensity_A::product() const
{
    return oops::Variable("dry_air_density");
}

oops::Variables DryAirDensity_A::ingredients() const
{
    return DryAirDensity_A::Ingredients;
}

oops::Variables DryAirDensity_A::trajectoryVars() const
{
    return DryAirDensity_A::Ingredients;
}

size_t DryAirDensity_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["air_pressure"].shape(1);
}

atlas::FunctionSpace DryAirDensity_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset.field("air_pressure").functionspace();
}

// -------------------------------------------------------------------------------------------------

void DryAirDensity_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering DryAirDensity_A::executeNL function"
      << std::endl;

    const double rdry = configVariables_.getDouble("gas_constant_of_dry_air");
    // TODO(vahl) :"epsilon" should be changed to more CCPP
    //           "ratio_of_dry_air_gas_to_water_vapor_constants"
    const double epsilon_star = 1.0 / configVariables_.getDouble("epsilon") - 1.0;

    // The density of the dry air alone is the total (moist) air density scaled by the
    // dry mass fraction (1 - q). Using the definition of virtual temperature,
    // Tv = T * (1 + epsilon_star * q), this is
    //   rho_dry = p * (1 - q) / (Rd * T * (1 + epsilon_star * q)),
    // which is algebraically identical to (p - e) / (Rd * T).
    util::for_each_value(
        [&](const double air_p,
            const double air_t,
            const double spechum,
            double& air_dens) {
            air_dens = air_p*(1.0 - spechum)/(rdry*air_t*(1.0 + epsilon_star*spechum));
        },
        afieldset["air_pressure"],
        afieldset["air_temperature"],
        afieldset["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldset["dry_air_density"]);

    oops::Log::trace() << "leaving DryAirDensity_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void DryAirDensity_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensity_A::executeTL function"
        << std::endl;

    const double rdry = configVariables_.getDouble("gas_constant_of_dry_air");
    const double epsilon_star = 1.0 / configVariables_.getDouble("epsilon") - 1.0;

    util::for_each_value(
        [&](const double air_p,
            const double air_t,
            const double spechum,
            const double tl_air_p,
            const double tl_air_t,
            const double tl_spechum,
            double& tl_air_dens) {
            const double air_dens =
                air_p*(1.0 - spechum)/(rdry*air_t*(1.0 + epsilon_star*spechum));
            const double drho_dq =
                -air_dens*(1.0/(1.0 - spechum) + epsilon_star/(1.0 + epsilon_star*spechum));
            tl_air_dens = tl_air_p * (air_dens/air_p)
                        - tl_air_t * (air_dens/air_t)
                        + tl_spechum * drho_dq;
        },
        afieldsetTraj["air_pressure"],
        afieldsetTraj["air_temperature"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetTL["air_pressure"],
        afieldsetTL["air_temperature"],
        afieldsetTL["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetTL["dry_air_density"]);

    oops::Log::trace() << "leaving DryAirDensity_A::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void DryAirDensity_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensity::executeAD function"
        << std::endl;

    const double rdry = configVariables_.getDouble("gas_constant_of_dry_air");
    const double epsilon_star = 1.0 / configVariables_.getDouble("epsilon") - 1.0;

    util::for_each_value(
        [&](const double air_p,
            const double air_t,
            const double spechum,
            double& ad_air_dens,
            double& ad_air_p,
            double& ad_air_t,
            double& ad_spechum) {
            const double air_dens =
                air_p*(1.0 - spechum)/(rdry*air_t*(1.0 + epsilon_star*spechum));
            const double drho_dq =
                -air_dens*(1.0/(1.0 - spechum) + epsilon_star/(1.0 + epsilon_star*spechum));
            ad_air_p += ad_air_dens * (air_dens/air_p);
            ad_air_t += -ad_air_dens * (air_dens/air_t);
            ad_spechum += ad_air_dens * drho_dq;
            ad_air_dens = 0.0;
        },
        afieldsetTraj["air_pressure"],
        afieldsetTraj["air_temperature"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetAD["dry_air_density"],
        afieldsetAD["air_pressure"],
        afieldsetAD["air_temperature"],
        afieldsetAD["water_vapor_mixing_ratio_wrt_moist_air"]);

    oops::Log::trace() << "leaving DryAirDensity_A::executeAD function" << std::endl;
}

}  // namespace vader
