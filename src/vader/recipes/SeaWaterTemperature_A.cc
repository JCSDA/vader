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
#include "vader/recipes/SeaWaterTemperature.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char SeaWaterTemperature_A::Name[] = "SeaWaterTemperature_A";
const oops::Variables SeaWaterTemperature_A::Ingredients{
                                   std::vector<std::string>{"sea_water_potential_temperature",
                                                            "sea_water_salinity",
                                                            "latitude",
                                                            "longitude",
                                                            "sea_water_depth"}};

// Register the maker
static RecipeMaker<SeaWaterTemperature_A> makerSWPTempToSWTemp_(SeaWaterTemperature_A::Name);

SeaWaterTemperature_A::SeaWaterTemperature_A(const Parameters_ & params,
                                    const VaderConfigVars & configVariables):
                                            configVariables_{configVariables}
{
    oops::Log::trace() << "SeaWaterTemperature_A::SeaWaterTemperature_A(params)"
        << std::endl;
}

std::string SeaWaterTemperature_A::name() const
{
    return SeaWaterTemperature_A::Name;
}

oops::Variable SeaWaterTemperature_A::product() const
{
    return oops::Variable{"sea_water_temperature"};
}

oops::Variables SeaWaterTemperature_A::ingredients() const
{
    return SeaWaterTemperature_A::Ingredients;
}

size_t SeaWaterTemperature_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("sea_water_salinity").shape(1);
}

atlas::FunctionSpace SeaWaterTemperature_A::productFunctionSpace
                                                (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("sea_water_potential_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void SeaWaterTemperature_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering SeaWaterTemperature_A::executeNL function"
        << std::endl;

    // Get fields
    atlas::Field potential_temperature = afieldset.field("sea_water_potential_temperature");
    atlas::Field salinity = afieldset.field("sea_water_salinity");
    atlas::Field depth = afieldset.field("sea_water_depth");
    atlas::Field latitude = afieldset.field("latitude");
    atlas::Field longitude = afieldset.field("longitude");
    atlas::Field insitu_temperature = afieldset.field("sea_water_temperature");

    // Number of levels
    int nlevels = potential_temperature.shape(1);

    util::for_each_column(
        [&](const auto potential_temperature_col,
            const auto salinity_col,
            const auto depth_col,
            const auto latitude_col,
            const auto longitude_col,
            auto insitu_temperature_col) {
            for (int level = 0; level < nlevels; ++level) {
                // Obtain pressure from depth
                double pressure = gsw_p_from_z_f90(-depth_col(level), latitude_col(0));

                // Convert practical salinity to absolute salinity
                double absolute_salinity = gsw_sa_from_sp_f90(salinity_col(level), pressure,
                                            longitude_col(0), latitude_col(0));

                // Convert potential temperature to conservative temperature
                double conservative_temperature = gsw_ct_from_pt_f90(absolute_salinity,
                                                    potential_temperature_col(level));

                // Calculate Sea Water Temperature
                insitu_temperature_col(level) = gsw_t_from_ct_f90(absolute_salinity,
                                                    conservative_temperature, pressure);
            }
        },
        potential_temperature,
        salinity,
        depth,
        latitude,
        longitude,
        insitu_temperature);

    oops::Log::trace() << "leaving SeaWaterTemperature_A::executeNL function" << std::endl;
}

}  // namespace vader
