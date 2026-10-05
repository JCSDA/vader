/*
 * (C) Copyright 2025- UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/SeaWaterTemperature.h"

using atlas::array::make_datatype;
using atlas::array::make_shape;

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char SeaWaterTemperature_B::Name[] = "SeaWaterTemperature_B";
const oops::Variables SeaWaterTemperature_B::Ingredients{
                                   std::vector<std::string>{"sea_water_potential_temperature",
                                                            // CF standard name would be
                                                            // "sea_water_absolute_salinity".
                                                            "sea_water_salinity"}};

const oops::Variables SeaWaterTemperature_B::TrajectoryVars{
                                   std::vector<std::string>{"sea_water_potential_temperature",
                                                            // CF standard name would be
                                                            // "sea_water_absolute_salinity".
                                                            "sea_water_salinity",
                                                            "latitude",
                                                            "longitude",
                                                            "sea_water_depth",
                                                            "sea_area_fraction",
                                                            "sea_water_temperature"}};

// Register the maker
static RecipeMaker<SeaWaterTemperature_B> makerSWPTempToSWTemp_(SeaWaterTemperature_B::Name);

SeaWaterTemperature_B::SeaWaterTemperature_B(const Parameters_ & params,
                                             const VaderConfigVars & configVariables)
  : configVariables_{configVariables}, haveJac_(false), jacT_(), jacS_()
{
    oops::Log::trace() << "SeaWaterTemperature_B::SeaWaterTemperature_B(params)"
        << std::endl;
}

std::string SeaWaterTemperature_B::name() const
{
    return SeaWaterTemperature_B::Name;
}

oops::Variable SeaWaterTemperature_B::product() const
{
    return oops::Variable{"sea_water_temperature"};
}

oops::Variables SeaWaterTemperature_B::ingredients() const
{
    return SeaWaterTemperature_B::Ingredients;
}

oops::Variables SeaWaterTemperature_B::trajectoryVars() const
{
    return SeaWaterTemperature_B::TrajectoryVars;
}

size_t SeaWaterTemperature_B::productLevels(const atlas::FieldSet & afieldset) const
{
    // CF standard name would be "sea_water_absolute_salinity".
    return afieldset.field("sea_water_salinity").shape(1);
}

atlas::FunctionSpace SeaWaterTemperature_B::productFunctionSpace
                                                (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("sea_water_potential_temperature").functionspace();
}

void SeaWaterTemperature_B::computeJac(const atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering SeaWaterTemperature_B::computeJac function"
        << std::endl;

    // Get fields
    atlas::Field potential_temperature = afieldset.field("sea_water_potential_temperature");
    // CF standard name would be "sea_water_absolute_salinity".
    atlas::Field salinity = afieldset.field("sea_water_salinity");
    atlas::Field depth = afieldset.field("sea_water_depth");
    atlas::Field latitude = afieldset.field("latitude");
    atlas::Field longitude = afieldset.field("longitude");
    atlas::Field insitu_temperature = afieldset.field("sea_water_temperature");
    atlas::Field sea_area_fraction = afieldset.field("sea_area_fraction");

    // Grid dimensions
    size_t grid_size = salinity.shape(0);
    int nlevels = potential_temperature.levels();

    // Allocate the Atlas fields for the shared_ptr Jacobian
    jacS_.reset(new atlas::Field("JacobianSalinity", make_datatype<double>(),
                                 make_shape(grid_size, nlevels)));
    jacT_.reset(new atlas::Field("JacobianTemperature", make_datatype<double>(),
                                 make_shape(grid_size, nlevels)));

    // Finite-difference perturbation for the Jacobian. The gsw conversions are
    // evaluated in single precision (float), so the step must stay well above
    // float roundoff (~1e-7 relative) to produce a non-zero difference. A step
    // of 1e-2 (degC / g kg-1) is resolvable while keeping truncation error
    // negligible for these near-linear conversions.
    const double t_p = 1e-2;
    const double s_p = t_p;

    // Make views for jacS_ and jacT_ so they can be filled in
    util::for_each_column(
        [&](const auto potential_temperature_col,
            const auto salinity_col,
            const auto depth_col,
            const auto latitude_col,
            const auto longitude_col,
            const auto sea_area_fraction_col,
            auto insitu_temperature_col,
            auto jacS_col,
            auto jacT_col) {
            // sea_area_fraction is a 2D (surface) mask; if the column is land,
            // the whole column is land.
            const bool is_land = (sea_area_fraction_col(0) == 0.0);
            for (int level = 0; level < nlevels; ++level) {
              // Set the Jacobian to 0 on land columns and on below-bottom /
              // masked levels, where the practical salinity is filled with 0.
              // Passing salinity == 0 into the gsw conversions triggers a
              // divide-by-zero inside gsw_pt_from_ct.
              if (is_land || salinity_col(level) <= 0.0) {
                jacT_col(level) = 0.0;
                jacS_col(level) = 0.0;
                continue;
              }

              // Obtain pressure from depth
              double pressure = gsw_p_from_z_f90(-depth_col(level), latitude_col(0));

              // Base state
              double salinity0 = salinity_col(level);
              double absolute_salinity0 = gsw_sa_from_sp_f90(salinity0, pressure,
                                         longitude_col(0), latitude_col(0));
              double conservative_temperature0 = gsw_ct_from_pt_f90(absolute_salinity0,
                                         potential_temperature_col(level));
              double insitu_temperature0 = gsw_t_from_ct_f90(absolute_salinity0,
                                         conservative_temperature0, pressure);

              // Temperature derivative: perturb potential temperature and keep the
              // salinity state unchanged.
              double absolute_salinity_t = gsw_sa_from_sp_f90(salinity0, pressure,
                                         longitude_col(0), latitude_col(0));
              double conservative_temperature_t = gsw_ct_from_pt_f90(absolute_salinity_t,
                                         potential_temperature_col(level) + t_p);
              double insitu_temp_dt = gsw_t_from_ct_f90(absolute_salinity_t,
                                      conservative_temperature_t, pressure);

              // Salinity derivative: perturb practical salinity and recompute the
              // full thermodynamic chain to remain consistent with the chain
              // salinity -> absolute salinity -> conservative temperature -> in-situ
              // temperature.
              double salinity_s = salinity0 + s_p;
              double absolute_salinity_s = gsw_sa_from_sp_f90(salinity_s, pressure,
                                         longitude_col(0), latitude_col(0));
              double conservative_temperature_s = gsw_ct_from_pt_f90(absolute_salinity_s,
                                         potential_temperature_col(level));
              double insitu_temp_ds = gsw_t_from_ct_f90(absolute_salinity_s,
                                      conservative_temperature_s, pressure);

              jacT_col(level) = (insitu_temp_dt - insitu_temperature0) / t_p;
              jacS_col(level) = (insitu_temp_ds - insitu_temperature0) / s_p;
              insitu_temperature_col(level) = insitu_temperature0;
            }
            },
        potential_temperature,
        salinity,
        depth,
        latitude,
        longitude,
        sea_area_fraction,
        insitu_temperature,
        *jacS_,
        *jacT_);

    // Set flag that Jacobian has been computed
    haveJac_ = true;
}

void SeaWaterTemperature_B::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering SeaWaterTemperature_B::executeTL function"
        << std::endl;

    // Make sure the trajectory is set
    if (!haveJac_) {
      this->computeJac(afieldsetTraj);
    }

    // Get fields
    atlas::Field tl_potential_temp = afieldsetTL.field("sea_water_potential_temperature");
    // CF standard name would be "sea_water_absolute_salinity".
    atlas::Field tl_salinity = afieldsetTL.field("sea_water_salinity");
    atlas::Field tl_insitu_temperature = afieldsetTL.field("sea_water_temperature");

    // Grid dimensions
    int nlevels = tl_potential_temp.levels();

    util::for_each_column(
        [&](const auto tl_potential_temp_col,
            const auto tl_salinity_col,
            auto tl_insitu_temp_col,
            const auto jacT_col,
            const auto jacS_col) {
            for (int level = 0; level < nlevels; ++level) {
              // Calculate Sea Water Temperature TL
              tl_insitu_temp_col(level) =
                          jacT_col(level) * tl_potential_temp_col(level) +
                          jacS_col(level) * tl_salinity_col(level);
            }
            },
        tl_potential_temp,
        tl_salinity,
        tl_insitu_temperature,
        *jacT_,
        *jacS_);

    oops::Log::trace() << "leaving SeaWaterTemperature_B::executeTL function" << std::endl;
}

void SeaWaterTemperature_B::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering SeaWaterTemperature_B::executeAD function"
        << std::endl;

    // Make sure the trajectory is set
    if (!haveJac_) {
      this->computeJac(afieldsetTraj);
    }

    // Get fields
    atlas::Field ad_potential_temp = afieldsetAD.field("sea_water_potential_temperature");
    // CF standard name would be "sea_water_absolute_salinity".
    atlas::Field ad_salinity = afieldsetAD.field("sea_water_salinity");
    atlas::Field ad_insitu_temperature = afieldsetAD.field("sea_water_temperature");

    // Grid dimensions
    int nlevels = ad_potential_temp.levels();

    util::for_each_column(
        [&](const auto ad_insitu_temp_col,
            const auto jacT_col,
            const auto jacS_col,
            auto ad_potential_temp_col,
            auto ad_salinity_col) {
            for (int level = 0; level < nlevels; ++level) {
              // Calculate Sea Water Temperature AD
              ad_potential_temp_col(level) = ad_potential_temp_col(level) +
                                              jacT_col(level) * ad_insitu_temp_col(level);
              ad_salinity_col(level) = ad_salinity_col(level) +
                                              jacS_col(level) * ad_insitu_temp_col(level);
            }
            },
        ad_insitu_temperature,
        *jacT_,
        *jacS_,
        ad_potential_temp,
        ad_salinity);

    oops::Log::trace() << "leaving SeaWaterTemperature_B::executeAD function" << std::endl;
}

}  // namespace vader
