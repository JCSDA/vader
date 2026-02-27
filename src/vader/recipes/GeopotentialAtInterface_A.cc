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
#include "vader/recipes/GeopotentialAtInterface.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char GeopotentialAtInterface_A::Name[] = "GeopotentialAtInterface_A";
const oops::Variables GeopotentialAtInterface_A::Ingredients{
    std::vector<std::string>{
    "geopotential",
    "virtual_temperature",
    "ln_air_pressure",
    "ln_air_pressure_at_interface"}};

// Register the maker
static RecipeMaker<GeopotentialAtInterface_A> makerGeopotentialAtInterface_A_(
    GeopotentialAtInterface_A::Name);

GeopotentialAtInterface_A::GeopotentialAtInterface_A(const Parameters_ & params,
                                                     const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "GeopotentialAtInterface_A::GeopotentialAtInterface_A(params)"
        << std::endl;
}

std::string GeopotentialAtInterface_A::name() const
{
    return GeopotentialAtInterface_A::Name;
}

oops::Variable GeopotentialAtInterface_A::product() const
{
    return oops::Variable{"geopotential_levels"};
}

oops::Variables GeopotentialAtInterface_A::ingredients() const
{
    return GeopotentialAtInterface_A::Ingredients;
}

oops::Variables GeopotentialAtInterface_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"ln_air_pressure",
                                                    "ln_air_pressure_at_interface"}};
}

size_t GeopotentialAtInterface_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("ln_air_pressure_at_interface").shape(1);
}
atlas::FunctionSpace GeopotentialAtInterface_A::productFunctionSpace(const atlas::FieldSet &
                                                                                    afieldset) const
{
    return afieldset.field("geopotential").functionspace();
}

// -------------------------------------------------------------------------------------------------

void GeopotentialAtInterface_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering GeopotentialAtInterface_A::executeNL function" << std::endl;

    const double rdry = configVariables_.getDouble("gas_constant_of_dry_air");

    atlas::Field phi = afieldset.field("geopotential");
    atlas::Field tv = afieldset.field("virtual_temperature");
    atlas::Field ln_p = afieldset.field("ln_air_pressure");
    atlas::Field ln_p_int = afieldset.field("ln_air_pressure_at_interface");
    atlas::Field phi_int = afieldset.field("geopotential_levels");

    const int nlev = phi.shape(1);
    const int nint = ln_p_int.shape(1);

    if (nlev < 2 || nint < 2) {
        oops::Log::error() << "GeopotentialAtInterface_A::executeNL: need at least 2 full levels "
               "and 2 interfaces" << std::endl;
        ABORT("GeopotentialAtInterface_A::executeNL: need at least 2 full levels and 2 interfaces");
    }

    if (nint != nlev + 1) {
        oops::Log::error() << "GeopotentialAtInterface_A::executeTL: number of interfaces "
               "must be one more than the number of full levels" << std::endl;
        ABORT("GeopotentialAtInterface_A::executeTL: number of interfaces "
              "must be one more than the number of full levels");
    }

    util::for_each_column(
    [&](const auto phi_col,
        const auto tv_col,
        const auto ln_p_col,
        const auto ln_p_int_col,
        auto phi_int_col) {
        double tv_layer_0 = 0.5 * (tv_col(0) + tv_col(1));
        double dln_top = ln_p_col(0) - ln_p_int_col(0);
        phi_int_col(0) = phi_col(0) + rdry * tv_layer_0 * dln_top;

        for (int k = 0; k < nint - 1; ++k) {
            double tv_layer_k = 0.5 * (tv_col(k) + tv_col(k + 1));
            double dln = ln_p_int_col(k + 1) - ln_p_int_col(k);
            phi_int_col(k + 1) = phi_int_col(k) - rdry * tv_layer_k * dln;
        }
    },
    phi,
    tv,
    ln_p,
    ln_p_int,
    phi_int);

    oops::Log::trace() << "leaving GeopotentialAtInterface_A::executeNL function" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void GeopotentialAtInterface_A::executeTL(atlas::FieldSet & afieldsetTL,
                                    const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "entering GeopotentialAtInterface_A::executeTL function" << std::endl;

    const double rdry = configVariables_.getDouble("gas_constant_of_dry_air");

    atlas::Field phi_tl = afieldsetTL.field("geopotential");
    atlas::Field tv_tl = afieldsetTL.field("virtual_temperature");
    atlas::Field ln_p = afieldsetTraj.field("ln_air_pressure");
    atlas::Field ln_p_int = afieldsetTraj.field("ln_air_pressure_at_interface");
    atlas::Field phi_int_tl = afieldsetTL.field("geopotential_levels");

    const int nlev = phi_tl.shape(1);
    const int nint = ln_p_int.shape(1);

    if (nlev < 2 || nint < 2) {
        oops::Log::error() << "GeopotentialAtInterface_A::executeTL: need at least 2 full levels "
               "and 2 interfaces" << std::endl;
        ABORT("GeopotentialAtInterface_A::executeTL: need at least 2 full levels and 2 interfaces");
    }

    util::for_each_column(
    [&](const auto phi_tl_col,
        const auto tv_tl_col,
        const auto ln_p_col,
        const auto ln_p_int_col,
        auto phi_int_tl_col) {
        double dln_top = ln_p_col(0) - ln_p_int_col(0);
        double tv_layer_tl_0 = 0.5 * (tv_tl_col(0) + tv_tl_col(1));
        phi_int_tl_col(0) = phi_tl_col(0) + rdry * tv_layer_tl_0 * dln_top;

        for (int k = 0; k < nint - 1; ++k) {
            double dln = ln_p_int_col(k + 1) - ln_p_int_col(k);
            double tv_layer_tl_k = 0.5 * (tv_tl_col(k) + tv_tl_col(k + 1));
            phi_int_tl_col(k + 1) = phi_int_tl_col(k) - rdry * tv_layer_tl_k * dln;
        }
    },
    phi_tl,
    tv_tl,
    ln_p,
    ln_p_int,
    phi_int_tl);

    oops::Log::trace() << "leaving GeopotentialAtInterface_A::executeTL function" << std::endl;
}

// ------------------------------------------------------------------------------------------------

void GeopotentialAtInterface_A::executeAD(atlas::FieldSet & afieldsetAD,
                                    const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "entering GeopotentialAtInterface_A::executeAD function" << std::endl;

    const double rdry = configVariables_.getDouble("gas_constant_of_dry_air");

    atlas::Field phi_ad = afieldsetAD.field("geopotential");
    atlas::Field tv_ad = afieldsetAD.field("virtual_temperature");
    atlas::Field ln_p = afieldsetTraj.field("ln_air_pressure");
    atlas::Field ln_p_int = afieldsetTraj.field("ln_air_pressure_at_interface");
    atlas::Field phi_int_ad = afieldsetAD.field("geopotential_levels");

    const int nlev = phi_ad.shape(1);
    const int nint = ln_p_int.shape(1);

    if (nlev < 2 || nint < 2) {
        oops::Log::error() << "GeopotentialAtInterface_A::executeAD: need at least 2 full levels "
               "and 2 interfaces" << std::endl;
        ABORT("GeopotentialAtInterface_A::executeAD: need at least 2 full levels and 2 interfaces");
    }

    util::for_each_column(
        [&](const auto ln_p_col,
            const auto ln_p_int_col,
            auto phi_ad_col,
            auto tv_ad_col,
            auto phi_int_ad_col) {
            std::vector<double> tv_layer_ad(nlev - 1, 0.0);
            for (int k = nlev - 2; k >= 0; --k) {
                double dln = ln_p_int_col(k + 1) - ln_p_int_col(k);
                tv_layer_ad[k] -= rdry * phi_int_ad_col(k + 1) * dln;
                phi_int_ad_col(k) += phi_int_ad_col(k + 1);
                phi_int_ad_col(k + 1) = 0.0;
            }
            double dln_top = ln_p_col(0) - ln_p_int_col(0);
            tv_layer_ad[0] += rdry * phi_int_ad_col(0) * dln_top;
            phi_ad_col(0) += phi_int_ad_col(0);
            phi_int_ad_col(0) = 0.0;
            for (int k = 0; k < nlev - 1; ++k) {
                tv_ad_col(k) += 0.5 * tv_layer_ad[k];
                tv_ad_col(k + 1) += 0.5 * tv_layer_ad[k];
            }
        },
        ln_p,
        ln_p_int,
        phi_ad,
        tv_ad,
        phi_int_ad);

    oops::Log::trace() << "leaving GeopotentialAtInterface_A::executeAD function" << std::endl;
}

// ------------------------------------------------------------------------------------------------

}  // namespace vader
