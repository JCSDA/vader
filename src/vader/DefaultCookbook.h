/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "vader/vader.h"
// Recipe headers
#include "recipes/AirDensityLevelsMinusOne.h"
#include "recipes/AirPotentialTemperature.h"
#include "recipes/AirPressureThickness.h"
#include "recipes/AirTemperature.h"
#include "recipes/AirVirtualTemperature.h"
#include "recipes/CloudIceMixingRatio.h"
#include "recipes/CloudLiquidMixingRatio.h"
#include "recipes/DryAirDensity.h"
#include "recipes/DryAirDensityLevelsMinusOne.h"
#include "recipes/EastwardWindAt10m.h"
#include "recipes/HydrostaticExnerLevels.h"
#include "recipes/NorthwardWindAt10m.h"
#include "recipes/ParticulateMatter2p5.h"
#include "recipes/RainMixingRatio.h"
#include "recipes/TotalMixingRatio.h"
#include "recipes/TotalWater.h"
#include "recipes/VirtualPotentialTemperature.h"
#include "recipes/WaterVaporMixingRatioWrtMoistAirAndCondensedWater.h"
#include "recipes/WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m.h"

namespace vader {

const cookbookConfigType Vader::defaultCookbookDefinition = {
        // Default VADER cookbook definition
        {oops::Variable{"air_temperature"},
                                     {AirTemperature_A::Name, AirTemperature_B::Name}},
        {oops::Variable{"cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"},
                                     {CloudIceMixingRatio_A::Name}},
        {oops::Variable{"cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water"},
                                     {CloudLiquidMixingRatio_A::Name}},
        {oops::Variable{"dry_air_density"},
                                     {DryAirDensity_A::Name}},
        {oops::Variable{"dry_air_density_levels_minus_one"},
                                     {DryAirDensityLevelsMinusOne_A::Name}},
        {oops::Variable{"air_density_levels_minus_one"},
                                     {AirDensityLevelsMinusOne_A::Name}},
        {oops::Variable{"hydrostatic_exner_levels"},
                                     {HydrostaticExnerLevels_A::Name}},
        {oops::Variable{"air_potential_temperature"},
                                     {AirPotentialTemperature_A::Name}},
        {oops::Variable{"qrain"},    {RainMixingRatio_A::Name}},
        {oops::Variable{"total_water_mixing_ratio_wrt_dry_air"},
                                     {TotalMixingRatio_A::Name}},
        {oops::Variable{"qt"},       {TotalWater_A::Name}},
        {oops::Variable{"water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water"},
                                     {WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A::Name}},
        {oops::Variable{"water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m"},
                                     {WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A::Name}},
        {oops::Variable{"eastward_wind_at_10m"},
                                     {uwind_at_10m_A::Name}},
        {oops::Variable{"virtual_potential_temperature"},
                                     {VirtualPotentialTemperature_B::Name,
                                      VirtualPotentialTemperature_A::Name}},
        {oops::Variable{"virtual_temperature"},
                                     {AirVirtualTemperature_A::Name}},
        {oops::Variable{"northward_wind_at_10m"},
                                     {vwind_at_10m_A::Name}},
        {oops::Variable{"mass_density_of_particulate_matter_2p5_in_air"},
                                     {ParticulateMatter2p5_A::Name,
                                      ParticulateMatter2p5_B::Name}}
    };

}  // namespace vader
