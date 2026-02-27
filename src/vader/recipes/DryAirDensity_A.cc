/*
 * (C) Copyright 2024 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/DryAirDensity.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DryAirDensity_A::Name[] = "DryAirDensity_A";
const oops::Variables DryAirDensity_A::
    Ingredients{std::vector<std::string>{"air_temperature", "air_pressure"}};

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

    const double rdgas = 287.58f;
    util::for_each_value(
        [&](const double air_p,
            const double air_t,
            double& air_dens) {
            air_dens = air_p/(rdgas*air_t);
        },
        afieldset["air_pressure"],
        afieldset["air_temperature"],
        afieldset["dry_air_density"]);

    oops::Log::trace() << "leaving DryAirDensity_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void DryAirDensity_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensity_A::executeTL function"
        << std::endl;

    const double rdgas = 287.58f;
    util::for_each_value(
        [&](const double air_p,
            const double air_t,
            const double tl_air_p,
            const double tl_air_t,
            double& tl_air_dens) {
            tl_air_dens = tl_air_p /(rdgas*air_t) - tl_air_t * (air_p/rdgas) / (air_t*air_t);
        },
        afieldsetTraj["air_pressure"],
        afieldsetTraj["air_temperature"],
        afieldsetTL["air_pressure"],
        afieldsetTL["air_temperature"],
        afieldsetTL["dry_air_density"]);

    oops::Log::trace() << "leaving DryAirDensity_A::executeTL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void DryAirDensity_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensity::executeAD function"
        << std::endl;

    const double rdgas = 287.58f;
    util::for_each_value(
        [&](const double air_p,
            const double air_t,
            double& ad_air_dens,
            double& ad_air_p,
            double& ad_air_t) {
            ad_air_p += ad_air_dens/(rdgas*air_t);
            ad_air_t += -ad_air_dens * (air_p/rdgas) / (air_t*air_t);
            ad_air_dens = 0.0;
        },
        afieldsetTraj["air_pressure"],
        afieldsetTraj["air_temperature"],
        afieldsetAD["dry_air_density"],
        afieldsetAD["air_pressure"],
        afieldsetAD["air_temperature"]);

    oops::Log::trace() << "leaving DryAirDensity_A::executeAD function" << std::endl;
}

}  // namespace vader
