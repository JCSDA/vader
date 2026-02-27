/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/SeaWaterPotentialTemperature.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char SeaWaterPotentialTemperature_A::Name[] = "SeaWaterPotentialTemperature_A";
const oops::Variables SeaWaterPotentialTemperature_A::Ingredients{std::vector<std::string>{
                                                                "sea_water_temperature",
                                                                "sea_water_salinity",
                                                                "latitude",
                                                                "longitude",
                                                                "sea_water_depth"}};

// Register the maker
static RecipeMaker<SeaWaterPotentialTemperature_A> makerSWTempToSWPTemp_(
                    SeaWaterPotentialTemperature_A::Name);

SeaWaterPotentialTemperature_A::SeaWaterPotentialTemperature_A(const Parameters_ & params,
                                    const VaderConfigVars & configVariables):
                                            configVariables_{configVariables}
{
    oops::Log::trace() << "SeaWaterPotentialTemperature_A::SeaWaterPotentialTemperature_A(params)"
        << std::endl;
}

std::string SeaWaterPotentialTemperature_A::name() const
{
    return SeaWaterPotentialTemperature_A::Name;
}

oops::Variable SeaWaterPotentialTemperature_A::product() const
{
    return oops::Variable{"sea_water_potential_temperature"};
}

oops::Variables SeaWaterPotentialTemperature_A::ingredients() const
{
    return SeaWaterPotentialTemperature_A::Ingredients;
}

size_t SeaWaterPotentialTemperature_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("sea_water_salinity").shape(1);
}

atlas::FunctionSpace SeaWaterPotentialTemperature_A::productFunctionSpace
                                                (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("sea_water_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void SeaWaterPotentialTemperature_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering SeaWaterPotentialTemperature_A::executeNL function"
        << std::endl;

    // Get fields
    atlas::Field potential_temperature = afieldset.field("sea_water_potential_temperature");
    atlas::Field salinity = afieldset.field("sea_water_salinity");
    atlas::Field latitude = afieldset.field("latitude");
    atlas::Field longitude = afieldset.field("longitude");
    atlas::Field depth = afieldset.field("sea_water_depth");
    atlas::Field insitu_temperature = afieldset.field("sea_water_temperature");

    // Number of levels
    int nlevels = potential_temperature.shape(1);

    util::for_each_column(
        [&](
            const auto salinity_col,
            const auto depth_col,
            const auto latitude_col,
            const auto longitude_col,
            const auto insitu_temperature_col,
            auto potential_temperature_col) {
            for (int level = 0; level < nlevels; ++level) {
                // Obtain pressure from depth
                double pressure = gsw_p_from_z_f90(-depth_col(level), latitude_col(0));

                // Convert practical salinity to absolute salinity
                double absolute_salinity = gsw_sa_from_sp_f90(salinity_col(level), pressure,
                                            longitude_col(0), latitude_col(0));

                // Calculate Sea Water Potential Temperature
                potential_temperature_col(level) = gsw_pt_from_t_f90(absolute_salinity,
                                                    insitu_temperature_col(level), pressure);
            }
        },
        salinity,
        depth,
        latitude,
        longitude,
        insitu_temperature,
        potential_temperature);

    oops::Log::trace() << "leaving SeaWaterPotentialTemperature_A::executeNL function" << std::endl;
}

}  // namespace vader
