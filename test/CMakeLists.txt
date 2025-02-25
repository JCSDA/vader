list( APPEND vader_testinput
testinput/empty.yaml
testinput/planvariable.yaml
testinput/recipe_AirPressureExtendedUpByOne_A.yaml
testinput/recipe_AirTemperature_A.yaml
testinput/recipe_CloudIceMixingRatio_A.yaml
testinput/recipe_CloudLiquidMixingRatio_A.yaml
testinput/recipe_AirDensityLevelsMinusOne_A.yaml
testinput/recipe_DryAirDensityLevelsMinusOne_A.yaml
testinput/recipe_HydrostaticExnerLevels_A.yaml
testinput/recipe_HydrostaticPressureLevels_A.yaml
testinput/recipe_LogDerivativeSaturationVaporPressure_A.yaml
testinput/recipe_ParticulateMatter2p5_A.yaml
testinput/recipe_ParticulateMatter2p5_B.yaml
testinput/recipe_RainMixingRatio_A.yaml
testinput/recipe_RelativeHumidity_A.yaml
testinput/recipe_SaturationSpecificHumidity_A.yaml
testinput/recipe_SaturationVaporPressure_A.yaml
testinput/recipe_TotalMixingRatio_A.yaml
testinput/recipe_TotalRelativeHumidity_A.yaml
testinput/recipe_TotalWater_A.yaml
testinput/recipe_uwind_at_10m_A.yaml
testinput/recipe_VirtualPotentialTemperature_A.yaml
testinput/recipe_VirtualPotentialTemperature_B.yaml
testinput/recipe_vwind_at_10m_A.yaml
testinput/recipe_WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A.yaml
testinput/recipe_WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A.yaml
testinput/vader_AirTemperature_A.yaml
testinput/vader_CloudIceMixingRatio_A.yaml
testinput/vader_CloudLiquidMixingRatio_A.yaml
testinput/vader_DryAirDensityLevelsMinusOne_A.yaml
testinput/vader_HydrostaticExnerLevels_A.yaml
testinput/vader_ParticulateMatter2p5_A.yaml
testinput/vader_ParticulateMatter2p5_B.yaml
testinput/vader_RainMixingRatio_A.yaml
testinput/vader_TotalMixingRatio_A.yaml
testinput/vader_TotalWater_A.yaml
testinput/vader_uwind_at_10m_A.yaml
testinput/vader_VirtualPotentialTemperature_A.yaml
testinput/vader_VirtualPotentialTemperature_B.yaml
testinput/vader_vwind_at_10m_A.yaml
testinput/vader_WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A.yaml
testinput/vader_WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A.yaml
)

list( APPEND vader_testdata
  testdata/AirTemperature_A.nc
  testdata/wind_at_10m_A.nc
  testdata/gauss_state_F12.nc
  testdata/air_density_levels_minus_one_A.nc
  testdata/air_density_levels_minus_one_B.nc
  testdata/dry_air_density_levels_minus_one_A.nc
  testdata/aero6_gauss_state_F12.nc
  testdata/rrfs_sd_gauss_state_F12.nc
)

# Create data directory for test input and symlink all files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testinput)
foreach(FILENAME ${vader_testinput})
    execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink
           ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
           ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME} )
endforeach(FILENAME)
# Create data directory for test data and symlink all files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testdata)
foreach(FILENAME ${vader_testdata})
    execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink
           ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
           ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME} )
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
ecbuild_add_test( TARGET  test_vader_recipe_AirPressureExtendedUpByOne_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_AirPressureExtendedUpByOne_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_AirTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_AirTemperature_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_CloudIceMixingRatio_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_CloudIceMixingRatio_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_CloudLiquidMixingRatio_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_CloudLiquidMixingRatio_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_AirDensityLevelsMinusOne_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_AirDensityLevelsMinusOne_A.yaml"
                  LIBS    vader )


ecbuild_add_test( TARGET  test_vader_recipe_DryAirDensityLevelsMinusOne_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_DryAirDensityLevelsMinusOne_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_HydrostaticExnerLevels_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_HydrostaticExnerLevels_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_HydrostaticPressureLevels_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_HydrostaticPressureLevels_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_LogDerivativeSaturationVaporPressure_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_LogDerivativeSaturationVaporPressure_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_ParticulateMatter2p5_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_ParticulateMatter2p5_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_ParticulateMatter2p5_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_ParticulateMatter2p5_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_RainMixingRatio_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_RainMixingRatio_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_RelativeHumidity_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_RelativeHumidity_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_SaturationSpecificHumidity_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_SaturationSpecificHumidity_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_SaturationVaporPressure_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_SaturationVaporPressure_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_TotalMixingRatio_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_TotalMixingRatio_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_TotalRelativeHumidity_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_TotalRelativeHumidity_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_TotalWater_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_TotalWater_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_uwind_at_10m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_uwind_at_10m_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_VirtualPotentialTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_VirtualPotentialTemperature_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_VirtualPotentialTemperature_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_VirtualPotentialTemperature_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_vwind_at_10m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_vwind_at_10m_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_recipe_WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A.yaml"
                  LIBS    vader )

# Test vader calling recipes

ecbuild_add_test( TARGET  test_vader_vader_AirTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_AirTemperature_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_CloudIceMixingRatio_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_CloudIceMixingRatio_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_DryAirDensityLevelsMinusOne_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_DryAirDensityLevelsMinusOne_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_HydrostaticExnerLevels_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_HydrostaticExnerLevels_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_ParticulateMatter2p5_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_ParticulateMatter2p5_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_ParticulateMatter2p5_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_ParticulateMatter2p5_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_RainMixingRatio_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_RainMixingRatio_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_WaterVaporMixingRatioWrtMoistAirAndCondensedWater_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A
          COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
          ARGS    "testinput/vader_WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A.yaml"
          LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_TotalMixingRatio_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_TotalMixingRatio_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_TotalWater_A
                 COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                 ARGS    "testinput/vader_TotalWater_A.yaml"
                 LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_uwind_at_10m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_uwind_at_10m_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_VirtualPotentialTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_VirtualPotentialTemperature_A.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_VirtualPotentialTemperature_B
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_VirtualPotentialTemperature_B.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_vader_vwind_at_10m_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_vwind_at_10m_A.yaml"
                  LIBS    vader )

# Test planVariable algorithm
ecbuild_add_test( TARGET  test_vader_planvariable
                  SOURCES vader/TestPlanVariable.cc
                  ARGS    "testinput/planvariable.yaml"
                  LIBS    vader )

ecbuild_add_test( TARGET  test_vader_mo_lookuptables
                  SOURCES vader/mo/TestCheckLookupTables.cc
                  ARGS    "testinput/empty.yaml"
                  LIBS    vader )

# Test print (Prints default Vader cookbook, in yaml form, to log)
ecbuild_add_test( TARGET  test_vader_print_cookbook
                  SOURCES vader/TestPrintVader.cc
                  ARGS    "testinput/empty.yaml"
                  LIBS    vader )
