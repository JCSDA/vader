/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirVirtualTemperature.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirVirtualTemperature_A::Name[] = "AirVirtualTemperature_A";
// Note that water_vapor_mixing_ratio_wrt_moist_air is referred to as "specific humidity"
// by some groups.
const oops::Variables AirVirtualTemperature_A::Ingredients{
      std::vector<std::string>{"air_temperature", "water_vapor_mixing_ratio_wrt_moist_air"}};

// Register the maker
static RecipeMaker<AirVirtualTemperature_A> makerTempToVTemp_(AirVirtualTemperature_A::Name);

AirVirtualTemperature_A::AirVirtualTemperature_A(const Parameters_ & params,
                                    const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "AirVirtualTemperature_A::AirVirtualTemperature_A(params)" << std::endl;
}

std::string AirVirtualTemperature_A::name() const
{
    return AirVirtualTemperature_A::Name;
}

oops::Variable AirVirtualTemperature_A::product() const
{
    return oops::Variable{"virtual_temperature"};
}

oops::Variables AirVirtualTemperature_A::ingredients() const
{
    return AirVirtualTemperature_A::Ingredients;
}

oops::Variables AirVirtualTemperature_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"air_temperature",
                                                    "water_vapor_mixing_ratio_wrt_moist_air"}};
}

size_t AirVirtualTemperature_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace AirVirtualTemperature_A::productFunctionSpace
                                              (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").functionspace();
}

void AirVirtualTemperature_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering AirVirtualTemperature_A::executeNL function"
        << std::endl;

    // TODO(vahl) :"epsilon" should be changed to more CCPP
    //           "ratio_of_dry_air_gas_to_water_vapor_constants"
    const double epsilon = configVariables_.getDouble("epsilon");

    atlas::Field temperature = afieldset.field("air_temperature");
    atlas::Field specific_humidity = afieldset.field("water_vapor_mixing_ratio_wrt_moist_air");
    atlas::Field virtual_temperature = afieldset.field("virtual_temperature");

    auto temperature_view = atlas::array::make_view<double, 2>(temperature);
    auto specific_humidity_view = atlas::array::make_view<double, 2>(specific_humidity);
    auto virtual_temperature_view = atlas::array::make_view<double, 2>(virtual_temperature);

    size_t grid_size = temperature.shape(0);

    int nlevels = temperature.shape(1);
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        virtual_temperature_view(jnode, level) =
            temperature_view(jnode, level) *
            (1.0 + epsilon * specific_humidity_view(jnode, level));
      }
    }
    oops::Log::trace() << "leaving AirVirtualTemperature_A::executeNL function" << std::endl;
}

void AirVirtualTemperature_A::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering AirVirtualTemperature_A::executeTL function"
        << std::endl;

    double epsilon = configVariables_.getDouble("epsilon");

    atlas::Field traj_temperature = afieldsetTraj.field("air_temperature");
    atlas::Field traj_specific_humidity =
                                      afieldsetTraj.field("water_vapor_mixing_ratio_wrt_moist_air");
    atlas::Field tl_temperature = afieldsetTL.field("air_temperature");
    atlas::Field tl_specific_humidity = afieldsetTL.field("water_vapor_mixing_ratio_wrt_moist_air");
    atlas::Field tl_virtual_temperature = afieldsetTL.field("virtual_temperature");

    auto tl_temperature_view = atlas::array::make_view<double, 2>(tl_temperature);
    auto traj_temperature_view = atlas::array::make_view<double, 2>(traj_temperature);
    auto traj_specific_humidity_view = atlas::array::make_view<double, 2>(traj_specific_humidity);
    auto tl_specific_humidity_view = atlas::array::make_view<double, 2>(tl_specific_humidity);
    auto tl_virtual_temperature_view = atlas::array::make_view<double, 2>(tl_virtual_temperature);

    size_t grid_size = tl_specific_humidity.shape(0);

    int nlevels = tl_temperature.shape(1);
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        tl_virtual_temperature_view(jnode, level) =
            tl_temperature_view(jnode, level) *
            (1.0 + epsilon * traj_specific_humidity_view(jnode, level)) +
            (traj_temperature_view(jnode, level) * epsilon *
             tl_specific_humidity_view(jnode, level));
      }
    }
    oops::Log::trace() << "leaving AirVirtualTemperature_A::executeTL function" << std::endl;
}

void AirVirtualTemperature_A::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering AirVirtualTemperature_A::executeAD function"
        << std::endl;

    double epsilon = configVariables_.getDouble("epsilon");

    atlas::Field traj_temperature = afieldsetTraj.field("air_temperature");
    atlas::Field traj_specific_humidity =
                                      afieldsetTraj.field("water_vapor_mixing_ratio_wrt_moist_air");
    atlas::Field ad_temperature = afieldsetAD.field("air_temperature");
    atlas::Field ad_specific_humidity = afieldsetAD.field("water_vapor_mixing_ratio_wrt_moist_air");
    atlas::Field ad_virtual_temperature = afieldsetAD.field("virtual_temperature");

    auto ad_temperature_view = atlas::array::make_view<double, 2>(ad_temperature);
    auto traj_temperature_view = atlas::array::make_view<double, 2>(traj_temperature);
    auto traj_specific_humidity_view = atlas::array::make_view<double, 2>(traj_specific_humidity);
    auto ad_specific_humidity_view = atlas::array::make_view<double, 2>(ad_specific_humidity);
    auto ad_virtual_temperature_view = atlas::array::make_view<double, 2>(ad_virtual_temperature);

    size_t grid_size = ad_specific_humidity.shape(0);

    int nlevels = ad_temperature.shape(1);
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        ad_temperature_view(jnode, level) += ad_virtual_temperature_view(jnode, level) *
            (1.0 + epsilon * traj_specific_humidity_view(jnode, level));

        ad_specific_humidity_view(jnode, level) +=
            ad_virtual_temperature_view(jnode, level) * epsilon *
            traj_temperature_view(jnode, level);

        ad_virtual_temperature_view(jnode, level) = 0.0;
      }
    }

    oops::Log::trace() << "leaving AirVirtualTemperature_A::executeAD function" << std::endl;
}

}  // namespace vader
