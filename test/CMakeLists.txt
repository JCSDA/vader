list( APPEND vader_testinput
testinput/empty.yaml
testinput/planvariable.yaml

testinput/recipe_AirPressureExtendedUpByOne_A.yaml
testinput/recipe_AirTemperature_A.yaml
testinput/recipe_CloudIceMixingRatioWrtWetAir_A.yaml
testinput/recipe_CloudLiquidWaterMixingRatioWrtWetAir_A.yaml
testinput/recipe_AirDensityLevelsMinusOne_A.yaml
testinput/recipe_DryAirDensityLevelsMinusOne_A.yaml
testinput/recipe_DustBin1MassConcentration_A.yaml
testinput/recipe_DustBin2MassConcentration_A.yaml
testinput/recipe_HydrostaticExnerLevels_A.yaml
testinput/recipe_HydrostaticPressureLevels_A.yaml
testinput/recipe_LogDerivativeSaturationVaporPressure_A.yaml
testinput/recipe_ParticulateMatter2p5_A.yaml
testinput/recipe_ParticulateMatter2p5_B.yaml
testinput/recipe_RainMixingRatio_A.yaml
testinput/recipe_RelativeHumidity_A.yaml
testinput/recipe_SaturationSpecificHumidity_A.yaml
testinput/recipe_SaturationVaporPressure_A.yaml
testinput/recipe_TotalRelativeHumidity_A.yaml
testinput/recipe_TotalWater_A.yaml
testinput/recipe_TotalWaterMixingRatioWrtDryAir_A.yaml
testinput/recipe_TotalWaterMixingRatioWrtWetAir_A.yaml
testinput/recipe_uwind_at_10m_A.yaml
testinput/recipe_VirtualPotentialTemperature_A.yaml
testinput/recipe_VirtualPotentialTemperature_B.yaml
testinput/recipe_vwind_at_10m_A.yaml
testinput/recipe_WaterVaporMixingRatioWrtDryAir_B.yaml
testinput/recipe_WaterVaporMixingRatioWrtWetAir_A.yaml
testinput/recipe_WaterVaporMixingRatioWrtWetAir2m_A.yaml

testinput/vader_AirTemperature_A.yaml
testinput/vader_AirVirtualTemperature_A.yaml
testinput/vader_CloudIceMixingRatioWrtDryAir_A.yaml
testinput/vader_CloudIceMixingRatioWrtDryAir_B.yaml
testinput/vader_CloudIceMixingRatioWrtWetAir_A.yaml
testinput/vader_CloudLiquidWaterMixingRatioWrtDryAir_A.yaml
testinput/vader_CloudLiquidWaterMixingRatioWrtDryAir_B.yaml
testinput/vader_CloudLiquidWaterMixingRatioWrtWetAir_A.yaml
testinput/vader_DryAirDensity_A.yaml
testinput/vader_DryAirDensityLevelsMinusOne_A.yaml
testinput/vader_GeopotentialAtInterface_A.yaml
testinput/vader_GeopotentialHeight_A.yaml
testinput/vader_GeopotentialHeightAtInterface_A.yaml
testinput/vader_GeopotentialHeightAtSurface_A.yaml
testinput/vader_HeightAboveMeanSeaLevelAtSurface_A.yaml
testinput/vader_HydrostaticExnerLevels_A.yaml
testinput/vader_LnAirPressure_A.yaml
testinput/vader_LnAirPressureAtInterface_A.yaml
testinput/vader_ParticulateMatter2p5_A.yaml
testinput/vader_ParticulateMatter2p5_B.yaml
testinput/vader_RainMixingRatio_A.yaml
testinput/vader_SurfaceAirPressure_A.yaml
testinput/vader_TotalWater_A.yaml
testinput/vader_uwind_at_10m_A.yaml
testinput/vader_VirtualPotentialTemperature_A.yaml
testinput/vader_VirtualPotentialTemperature_B.yaml
testinput/vader_vwind_at_10m_A.yaml
testinput/vader_WaterVaporMixingRatioWrtDryAir_A.yaml
testinput/vader_WaterVaporMixingRatioWrtDryAir_B.yaml
testinput/vader_WaterVaporMixingRatioWrtMoistAir_A.yaml
testinput/vader_WaterVaporMixingRatioWrtWetAir_A.yaml
testinput/vader_WaterVaporMixingRatioWrtWetAir2m_A.yaml
)

list( APPEND vader_testdata
  AirTemperature_A.nc
  wind_at_10m_A.nc
  gauss_state_F12.nc
  air_density_levels_minus_one_A.nc
  air_density_levels_minus_one_B.nc
  dry_air_density_levels_minus_one_A.nc
  aero6_gauss_state_F12.nc
  rrfs_sd_gauss_state_F12.nc
  dust_gauss_state_F12_lfric2um.nc
)

# Create data directory for test input and symlink all files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testinput)
foreach(FILENAME ${vader_testinput})
    execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink
           ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
           ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME} )
endforeach(FILENAME)

# Path to the vader test files
set (jedi_model_data_tier_1_vader "${CMAKE_SOURCE_DIR}/jedi-model-data/testinput_tier_1/vader")

# Create data directory for test data and symlink all files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testdata)
foreach(FILENAME ${vader_testdata})
    execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink
           ${jedi_model_data_tier_1_vader}/${FILENAME}
           ${CMAKE_CURRENT_BINARY_DIR}/testdata/${FILENAME} )
endforeach(FILENAME)

# Create test executables
ecbuild_add_executable( TARGET  test_recipe.x
                        SOURCES vader/TestRecipe.cc vader/TestUtils.cc
                        LIBS    vader
                       )
ecbuild_add_executable( TARGET  test_vader.x
                        SOURCES vader/TestVader.cc vader/TestUtils.cc
                        LIBS    vader
                       )

# Test recipes
ecbuild_add_test( TARGET  vader_recipe_AirPressureExtendedUpByOne_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_AirPressureExtendedUpByOne_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_AirTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_AirTemperature_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_CloudIceMixingRatioWrtWetAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_CloudIceMixingRatioWrtWetAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_CloudLiquidWaterMixingRatioWrtWetAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_CloudLiquidWaterMixingRatioWrtWetAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_AirDensityLevelsMinusOne_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_AirDensityLevelsMinusOne_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_DryAirDensityLevelsMinusOne_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_DryAirDensityLevelsMinusOne_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_DustBin1MassConcentration_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_DustBin1MassConcentration_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_DustBin2MassConcentration_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_DustBin2MassConcentration_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_HydrostaticExnerLevels_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_HydrostaticExnerLevels_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_HydrostaticPressureLevels_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_HydrostaticPressureLevels_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_LogDerivativeSaturationVaporPressure_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_LogDerivativeSaturationVaporPressure_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_ParticulateMatter2p5_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_ParticulateMatter2p5_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_ParticulateMatter2p5_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_ParticulateMatter2p5_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_RainMixingRatio_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_RainMixingRatio_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_RelativeHumidity_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_RelativeHumidity_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_SaturationSpecificHumidity_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_SaturationSpecificHumidity_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_SaturationVaporPressure_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_SaturationVaporPressure_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_TotalWaterMixingRatioWrtDryAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
		  ARGS    "testinput/recipe_TotalWaterMixingRatioWrtDryAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_TotalWaterMixingRatioWrtWetAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
		  ARGS    "testinput/recipe_TotalWaterMixingRatioWrtWetAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_TotalRelativeHumidity_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_TotalRelativeHumidity_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_TotalWater_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_TotalWater_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_uwind_at_10m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_uwind_at_10m_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_VirtualPotentialTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_VirtualPotentialTemperature_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_VirtualPotentialTemperature_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_VirtualPotentialTemperature_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_vwind_at_10m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_vwind_at_10m_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_WaterVaporMixingRatioWrtDryAir_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_WaterVaporMixingRatioWrtDryAir_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_WaterVaporMixingRatioWrtWetAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_WaterVaporMixingRatioWrtWetAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_recipe_WaterVaporMixingRatioWrtWetAir2m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_WaterVaporMixingRatioWrtWetAir2m_A.yaml"
                  LIBS    vader )

# Test vader calling recipes

ecbuild_add_test( TARGET  vader_vader_AirTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_AirTemperature_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_AirVirtualTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_AirVirtualTemperature_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_CloudLiquidWaterMixingRatioWrtDryAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_CloudLiquidWaterMixingRatioWrtDryAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_CloudLiquidWaterMixingRatioWrtDryAir_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_CloudLiquidWaterMixingRatioWrtDryAir_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_CloudLiquidWaterMixingRatioWrtWetAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_CloudLiquidWaterMixingRatioWrtWetAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_CloudIceMixingRatioWrtDryAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_CloudIceMixingRatioWrtDryAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_CloudIceMixingRatioWrtDryAir_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_CloudIceMixingRatioWrtDryAir_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_CloudIceMixingRatioWrtWetAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_CloudIceMixingRatioWrtWetAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_DryAirDensity_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_DryAirDensity_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_DryAirDensityLevelsMinusOne_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_DryAirDensityLevelsMinusOne_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_GeopotentialAtInterface_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_GeopotentialAtInterface_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_GeopotentialHeight_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_GeopotentialHeight_A.yaml"
                  LIBS    vader )


ecbuild_add_test( TARGET  vader_vader_GeopotentialHeightAtInterface_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_GeopotentialHeightAtInterface_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_GeopotentialHeightAtSurface_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_GeopotentialHeightAtSurface_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_HeightAboveMeanSeaLevelAtSurface_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_HeightAboveMeanSeaLevelAtSurface_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_HydrostaticExnerLevels_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_HydrostaticExnerLevels_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_LnAirPressure_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_LnAirPressure_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_LnAirPressureAtInterface_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_LnAirPressureAtInterface_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_ParticulateMatter2p5_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_ParticulateMatter2p5_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_ParticulateMatter2p5_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_ParticulateMatter2p5_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_RainMixingRatio_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_RainMixingRatio_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_SurfaceAirPressure_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_SurfaceAirPressure_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_TotalWater_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_TotalWater_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_uwind_at_10m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_uwind_at_10m_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_VirtualPotentialTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_VirtualPotentialTemperature_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_VirtualPotentialTemperature_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_VirtualPotentialTemperature_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_vwind_at_10m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_vwind_at_10m_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_WaterVaporMixingRatioWrtDryAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_WaterVaporMixingRatioWrtDryAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_WaterVaporMixingRatioWrtDryAir_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_WaterVaporMixingRatioWrtDryAir_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_WaterVaporMixingRatioWrtMoistAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_WaterVaporMixingRatioWrtMoistAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_WaterVaporMixingRatioWrtWetAir_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_WaterVaporMixingRatioWrtWetAir_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_vader_WaterVaporMixingRatioWrtWetAir2m_A
          COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
          ARGS    "testinput/vader_WaterVaporMixingRatioWrtWetAir2m_A.yaml"
          LIBS    vader )

# Test planVariable algorithm
ecbuild_add_test( TARGET  vader_planvariable
                  SOURCES vader/TestPlanVariable.cc
                  ARGS    "testinput/planvariable.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  vader_mo_lookuptables
                  SOURCES vader/mo/TestCheckLookupTables.cc
                  ARGS    "testinput/empty.yaml"
                  LIBS    vader )

# Test print (Prints default Vader cookbook, in yaml form, to log)
ecbuild_add_test( TARGET  vader_print_cookbook
                  SOURCES vader/TestPrintVader.cc
                  ARGS    "testinput/empty.yaml"
                  LIBS    vader )
