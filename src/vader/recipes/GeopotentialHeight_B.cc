/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "mo/constants.h"
#include "oops/util/abor1_cpp.h"
#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/GeopotentialHeight.h"

namespace vader {
namespace {

// Compressibility-factor constants from Davis et al.
constexpr double kCpfA0 = 1.58123e-6;
constexpr double kCpfA1 = -2.9331e-8;
constexpr double kCpfA2 = 1.1043e-10;
constexpr double kCpfB0 = 5.707e-6;
constexpr double kCpfB1 = -2.051e-8;
constexpr double kCpfC0 = 1.9898e-4;
constexpr double kCpfC1 = -2.376e-6;
constexpr double kCpfD = 1.83e-11;
constexpr double kCpfE = -0.765e-8;

// Saturation-vapor-pressure constants.
constexpr double kPsvA = 1.2378847e-5;
constexpr double kPsvB = -1.9121316e-2;
constexpr double kPsvC = 33.93711047;
constexpr double kPsvD = -6.3431645e3;

// Enhancement-factor constants.
constexpr double kEnhancementAlpha = 1.00062;
constexpr double kEnhancementBeta = 3.14e-8;
constexpr double kEnhancementGamma = 5.6e-7;

// ------------------------------------------------------------------------------------------------

double compressibilityFactor(const double pressure, const double temperature,
                             const double mixingRatio) {
  const double temperatureCelsius = temperature - mo::constants::ttp;

  const double saturationVaporPressure =
      std::exp(kPsvA * temperature * temperature + kPsvB * temperature + kPsvC +
               kPsvD / temperature);

  const double enhancementFactor =
      kEnhancementAlpha + kEnhancementBeta * pressure +
      kEnhancementGamma * temperatureCelsius * temperatureCelsius;

  const double vaporPressure =
      mixingRatio * pressure / (1.0 + mixingRatio * mo::constants::rd_over_rv);

  // Keep the same operation ordering as fv3-jedi.
  const double waterVaporMoleFraction = vaporPressure / saturationVaporPressure *
                                        enhancementFactor * saturationVaporPressure / pressure;

  const double pressureOverTemperature = pressure / temperature;

  return 1.0 -
         pressureOverTemperature *
             (kCpfA0 + kCpfA1 * temperatureCelsius +
              kCpfA2 * temperatureCelsius * temperatureCelsius +
              (kCpfB0 + kCpfB1 * temperatureCelsius) * waterVaporMoleFraction +
              (kCpfC0 + kCpfC1 * temperatureCelsius) * waterVaporMoleFraction *
                  waterVaporMoleFraction) +
         pressureOverTemperature * pressureOverTemperature *
             (kCpfD + kCpfE * waterVaporMoleFraction * waterVaporMoleFraction);
}

}  // namespace

// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char GeopotentialHeight_B::Name[] = "GeopotentialHeight_B";
const oops::Variables GeopotentialHeight_B::Ingredients{std::vector<std::string>{
                                                        "air_pressure",
                                                        "air_pressure_levels",
                                                        "air_temperature",
                                                        "water_vapor_mixing_ratio_wrt_dry_air",
                                                        "virtual_temperature",
                                                        "geopotential_height_at_surface"}};

// Register the maker
static RecipeMaker<GeopotentialHeight_B> makerGeopotentialHeight_B_(GeopotentialHeight_B::Name);

GeopotentialHeight_B::GeopotentialHeight_B(const Parameters_ & /*params*/,
                                           const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "GeopotentialHeight_B::GeopotentialHeight_B(params)" << std::endl;
}

std::string GeopotentialHeight_B::name() const
{
    return GeopotentialHeight_B::Name;
}

oops::Variable GeopotentialHeight_B::product() const
{
    return oops::Variable{"geopotential_height"};
}

oops::Variables GeopotentialHeight_B::ingredients() const
{
    return GeopotentialHeight_B::Ingredients;
}

size_t GeopotentialHeight_B::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_pressure").shape(1);
}

atlas::FunctionSpace GeopotentialHeight_B::productFunctionSpace(const atlas::FieldSet &
                                                                                    afieldset) const
{
    return afieldset.field("air_pressure").functionspace();
}

// ------------------------------------------------------------------------------------------------

void GeopotentialHeight_B::executeNL(atlas::FieldSet & afieldset)
{
  oops::Log::trace() << "entering GeopotentialHeight_B::executeNL" << std::endl;

  // Extract values from client config
  const bool levelsAreTopDown = configVariables_.getBool("levels_are_top_down");
  const double grav = configVariables_.getDouble("standard_gravitational_acceleration");
  const double dryAirGasConstantOverGravity = mo::constants::rd / grav;

  atlas::Field pressureField = afieldset.field("air_pressure");
  atlas::Field pressureLevelsField = afieldset.field("air_pressure_levels");
  atlas::Field temperatureField = afieldset.field("air_temperature");
  atlas::Field mixingRatioField = afieldset.field("water_vapor_mixing_ratio_wrt_dry_air");
  atlas::Field virtualTemperatureField = afieldset.field("virtual_temperature");
  atlas::Field surfaceHeightField = afieldset.field("geopotential_height_at_surface");
  atlas::Field heightField = afieldset.field("geopotential_height");

  const int numberOfLevels = pressureField.shape(1);

  ASSERT_MSG(pressureLevelsField.shape(1) == numberOfLevels + 1,
             "GeopotentialHeight_B requires air_pressure_levels to have one more level than "
             "air_pressure");
  ASSERT_MSG(temperatureField.shape(1) == numberOfLevels,
             "GeopotentialHeight_B received inconsistent air_temperature levels");
  ASSERT_MSG(mixingRatioField.shape(1) == numberOfLevels,
             "GeopotentialHeight_B received inconsistent dry-air water-vapor mixing-ratio "
             "levels");
  ASSERT_MSG(virtualTemperatureField.shape(1) == numberOfLevels,
             "GeopotentialHeight_B received inconsistent virtual_temperature levels");
  ASSERT_MSG(heightField.shape(1) == numberOfLevels,
             "GeopotentialHeight_B received an incorrect number of product levels");
  ASSERT_MSG(surfaceHeightField.shape(1) == 1,
             "GeopotentialHeight_B requires geopotential_height_at_surface to have one level");

  // Integrate outward from the surface. The two ordering conventions differ only in
  // iteration direction and in which air_pressure_levels index bounds the bottom
  // layer (the top end for top-down storage, the bottom end for bottom-up).
  const int step = levelsAreTopDown ? -1 : 1;
  const int firstLevel = levelsAreTopDown ? numberOfLevels - 1 : 0;
  const int surfacePressureLevelOffset = levelsAreTopDown ? 1 : 0;

  util::for_each_column(
    [&](const auto pressure, const auto pressureLevels, const auto temperature,
        const auto mixingRatio, const auto virtualTemperature, const auto surfaceHeight,
        auto height) {
      for (int n = 0; n < numberOfLevels; ++n) {
        const int level = firstLevel + n * step;
        const int previousLevel = level - step;  // only valid when n > 0
        const bool atSurface = (n == 0);

        double layerTemperature;
        double layerVirtualTemperature;
        double representativePressure;
        double pressureRatio;

        if (atSurface) {
          const double surfacePressure = pressureLevels(level + surfacePressureLevelOffset);
          layerTemperature = temperature(level);
          layerVirtualTemperature = virtualTemperature(level);
          representativePressure =
              std::exp(0.5 * (std::log(surfacePressure * 0.01) +
                              std::log(pressure(level) * 0.01)));
          pressureRatio = surfacePressure / pressure(level);
        } else {
          layerTemperature = 0.5 * (temperature(previousLevel) + temperature(level));
          layerVirtualTemperature =
              0.5 * (virtualTemperature(previousLevel) + virtualTemperature(level));
          representativePressure =
              std::exp(0.5 * (std::log(pressure(previousLevel) * 0.01) +
                              std::log(pressure(level) * 0.01)));
          pressureRatio = pressure(previousLevel) / pressure(level);
        }

        const double compressibility =
            compressibilityFactor(representativePressure, layerTemperature, mixingRatio(level));
        const double heightIncrement = dryAirGasConstantOverGravity * layerVirtualTemperature *
                                       compressibility * std::log(pressureRatio);

        height(level) =
            (atSurface ? surfaceHeight(0) : height(previousLevel)) + heightIncrement;
      }
    },
    pressureField, pressureLevelsField, temperatureField, mixingRatioField,
    virtualTemperatureField, surfaceHeightField, heightField);

  oops::Log::trace() << "leaving GeopotentialHeight_B::executeNL" << std::endl;
}

// ------------------------------------------------------------------------------------------------

}  // namespace vader

