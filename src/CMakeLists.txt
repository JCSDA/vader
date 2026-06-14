ecbuild_generate_config_headers( DESTINATION ${INSTALL_INCLUDE_DIR}/vader )

list( APPEND vader_src_files
vader/DefaultCookbook.h
vader/RecipeBase.cc
vader/RecipeBase.h
vader/recipes/AirPotentialTemperature_A.cc
vader/recipes/AirPotentialTemperature_B.cc
vader/recipes/AirPotentialTemperature.h
vader/recipes/AirPressureAtInterface_A.cc
vader/recipes/AirPressureAtInterface_B.cc
vader/recipes/AirPressureAtInterface_C.cc
vader/recipes/AirPressureAtInterface_D.cc
vader/recipes/AirPressureAtInterface.h
vader/recipes/WindReductionFactorAt10m.h
vader/recipes/WindReductionFactorAt10m_A.cc
vader/recipes/AirPressure.h
vader/recipes/AirPressure_A.cc
vader/recipes/AirPressure_B.cc
vader/recipes/AirPressureExtendedUpByOne_A.cc
vader/recipes/AirPressureExtendedUpByOne.h
vader/recipes/AirPressureThickness_A.cc
vader/recipes/AirPressureThickness.h
vader/recipes/AirPressureToKappa_A.cc
vader/recipes/AirPressureToKappa.h
vader/recipes/AirTemperature_A.cc
vader/recipes/AirTemperature_B.cc
vader/recipes/AirTemperature_C.cc
vader/recipes/AirTemperature.h
vader/recipes/AirVirtualTemperature_A.cc
vader/recipes/AirVirtualTemperature.h
vader/recipes/CloudIceMixingRatioWrtDryAir.h
vader/recipes/CloudIceMixingRatioWrtDryAir_A.cc
vader/recipes/CloudIceMixingRatioWrtDryAir_B.cc
vader/recipes/CloudIceMixingRatioWrtWetAir.h
vader/recipes/CloudIceMixingRatioWrtWetAir_A.cc
vader/recipes/CloudLiquidWaterMixingRatioWrtDryAir.h
vader/recipes/CloudLiquidWaterMixingRatioWrtDryAir_A.cc
vader/recipes/CloudLiquidWaterMixingRatioWrtDryAir_B.cc
vader/recipes/CloudLiquidWaterMixingRatioWrtWetAir.h
vader/recipes/CloudLiquidWaterMixingRatioWrtWetAir_A.cc
vader/recipes/AirDensityLevelsMinusOne.h
vader/recipes/AirDensityLevelsMinusOne_A.cc
vader/recipes/DryAirDensity.h
vader/recipes/DryAirDensity_A.cc
vader/recipes/DryAirDensityLevelsMinusOne.h
vader/recipes/DryAirDensityLevelsMinusOne_A.cc
mo/recipes/DustBin1MassConcentration.h
mo/recipes/DustBin1MassConcentration_A.cc
mo/recipes/DustBin2MassConcentration.h
mo/recipes/DustBin2MassConcentration_A.cc
vader/recipes/GeopotentialAtInterface_A.cc
vader/recipes/GeopotentialAtInterface.h
vader/recipes/GeopotentialHeight_A.cc
vader/recipes/GeopotentialHeight.h
vader/recipes/GeopotentialHeightAtInterface_A.cc
vader/recipes/GeopotentialHeightAtInterface.h
vader/recipes/GeopotentialHeightAtSurface_A.cc
vader/recipes/GeopotentialHeightAtSurface.h
vader/recipes/HeightAboveMeanSeaLevelAtSurface_A.cc
vader/recipes/HeightAboveMeanSeaLevelAtSurface.h
vader/recipes/HydrostaticExnerLevels.h
vader/recipes/HydrostaticExnerLevels_A.cc
vader/recipes/HydrostaticPressureLevels.h
vader/recipes/HydrostaticPressureLevels_A.cc
vader/recipes/LnAirPressure_A.cc
vader/recipes/LnAirPressure.h
vader/recipes/LnAirPressureAtInterface_A.cc
vader/recipes/LnAirPressureAtInterface.h
vader/recipes/LogDerivativeSaturationVaporPressure_A.cc
vader/recipes/LogDerivativeSaturationVaporPressure.h
vader/recipes/ParticulateMatter2p5.h
vader/recipes/ParticulateMatter2p5_A.cc
vader/recipes/ParticulateMatter2p5_B.cc
vader/recipes/RainMixingRatio.h
vader/recipes/RainMixingRatio_A.cc
vader/recipes/RelativeHumidity.h
vader/recipes/RelativeHumidity_A.cc
vader/recipes/SaturationSpecificHumidity.h
vader/recipes/SaturationSpecificHumidity_A.cc
vader/recipes/SaturationVaporPressure.h
vader/recipes/SaturationVaporPressure_A.cc
vader/recipes/SulfateMassFraction.h
vader/recipes/SulfateMassFraction_A.cc
vader/recipes/SurfaceAirPressure.h
vader/recipes/SurfaceAirPressure_A.cc
vader/recipes/TestRecipes.cc
vader/recipes/TestRecipes.h
vader/recipes/TotalRelativeHumidity.h
vader/recipes/TotalRelativeHumidity_A.cc
vader/recipes/TotalWater.h
vader/recipes/TotalWater_A.cc
vader/recipes/TotalWaterMixingRatioWrtDryAir.h
vader/recipes/TotalWaterMixingRatioWrtDryAir_A.cc
vader/recipes/TotalWaterMixingRatioWrtWetAir.h
vader/recipes/TotalWaterMixingRatioWrtWetAir_A.cc
vader/recipes/EastwardWindAt10m_A.cc
vader/recipes/EastwardWindAt10m.h
vader/recipes/VirtualPotentialTemperature_A.cc
vader/recipes/VirtualPotentialTemperature_B.cc
vader/recipes/VirtualPotentialTemperature.h
vader/recipes/NorthwardWindAt10m_A.cc
vader/recipes/NorthwardWindAt10m.h
vader/recipes/WaterVaporMixingRatioWrtDryAir.h
vader/recipes/WaterVaporMixingRatioWrtDryAir_A.cc
vader/recipes/WaterVaporMixingRatioWrtDryAir_B.cc
vader/recipes/WaterVaporMixingRatioWrtDryAir_C.cc
vader/recipes/WaterVaporMixingRatioWrtMoistAir.h
vader/recipes/WaterVaporMixingRatioWrtMoistAir_A.cc
vader/recipes/WaterVaporMixingRatioWrtWetAir.h
vader/recipes/WaterVaporMixingRatioWrtWetAir_A.cc
vader/recipes/WaterVaporMixingRatioWrtWetAir2m.h
vader/recipes/WaterVaporMixingRatioWrtWetAir2m_A.cc
vader/vader.cc
vader/vader.h
vader/VaderParameters.h
mo/common_varchange.cc
mo/common_varchange.h
mo/constants.h
mo/functions.h
mo/functions.cc
mo/control2analysis_varchange.h
mo/control2analysis_varchange.cc
mo/dlsvp_lookup.h
mo/dlsvpW_lookup.h
mo/eval_air_pressure.h
mo/eval_air_pressure.cc
mo/eval_air_pressure_levels.h
mo/eval_air_pressure_levels.cc
mo/eval_air_temperature.h
mo/eval_air_temperature.cc
mo/eval_cloud_ice_mixing_ratio.h
mo/eval_cloud_ice_mixing_ratio.cc
mo/eval_cloud_liquid_water_mixing_ratio.h
mo/eval_cloud_liquid_water_mixing_ratio.cc
mo/eval_air_density.h
mo/eval_air_density.cc
mo/eval_dry_air_density.h
mo/eval_dry_air_density.cc
mo/eval_dust_2bin_mass_concentration.h
mo/eval_dust_2bin_mass_concentration.cc
mo/eval_exner.h
mo/eval_exner.cc
mo/eval_geostrophic_to_hydrostatic_pressure.h
mo/eval_geostrophic_to_hydrostatic_pressure.cc
mo/eval_hydrostatic_balance.h
mo/eval_hydrostatic_balance.cc
mo/eval_mio_fields.h
mo/eval_mio_fields.cc
mo/eval_moisture_incrementing_operator.h
mo/eval_moisture_incrementing_operator.cc
mo/eval_rain_mixing_ratio.h
mo/eval_rain_mixing_ratio.cc
mo/eval_relative_humidity.h
mo/eval_relative_humidity.cc
mo/eval_sat_vapour_pressure.h
mo/eval_sat_vapour_pressure.cc
mo/eval_surface_pressure.h
mo/eval_surface_pressure.cc
mo/eval_surface_temperature.h
mo/eval_surface_temperature.cc
mo/eval_surface_wind.h
mo/eval_surface_wind.cc
mo/eval_total_water_mixing_ratio.h
mo/eval_total_water_mixing_ratio.cc
mo/eval_total_relative_humidity.h
mo/eval_total_relative_humidity.cc
mo/eval_virtual_potential_temperature.h
mo/eval_virtual_potential_temperature.cc
mo/eval_water_vapor_mixing_ratio.h
mo/eval_water_vapor_mixing_ratio.cc
mo/lookups.h
mo/model2geovals_varchange.h
mo/model2geovals_varchange.cc
mo/mio_interface.F90
mo/svp_lookup.h
mo/svpW_lookup.h
)

if( gsw_FOUND )
  list( APPEND vader_src_files
  OceanConversions/OceanConversions.interface.F90
  OceanConversions/OceanConversions.interface.h
  vader/recipes/SeaWaterPotentialTemperature_A.cc
  vader/recipes/SeaWaterPotentialTemperature.h
  vader/recipes/SeaWaterTemperature_A.cc
  vader/recipes/SeaWaterTemperature.h
  )
endif( gsw_FOUND )

include(GNUInstallDirs)
ecbuild_add_library( TARGET ${PROJECT_NAME}
                     SOURCES ${${PROJECT_NAME}_src_files}
                     INSTALL_HEADERS LISTED
                     HEADER_DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
                     LINKER_LANGUAGE CXX )

# Mark Boost as a system library to silence compiler warnings from Boost headers
target_include_directories( ${PROJECT_NAME} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS} )

target_link_libraries( ${PROJECT_NAME} PUBLIC ${oops_LIBRARIES} ) #TODO: Change to "oops::oops" once oops adds namespace support

if( gsw_FOUND )
  target_link_libraries( ${PROJECT_NAME} PUBLIC gsw ${gsw_LIBRARIES} )
  target_compile_definitions( ${PROJECT_NAME} PUBLIC GSW_FOUND )
endif( gsw_FOUND )

#Configure include directory layout for build-tree to match install-tree
set(BUILD_DIR_INCLUDE_PATH ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/include)
add_custom_target(${PROJECT_NAME}_headers ALL
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_DIR_INCLUDE_PATH}
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR} ${BUILD_DIR_INCLUDE_PATH}/${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_headers)

## Include paths
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
                                      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
