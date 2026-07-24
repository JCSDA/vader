/*
 * (C) Copyright 2021-2023  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/field/Field.h"

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirPotentialTemperature.h"


namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPotentialTemperature_A::Name[] = "AirPotentialTemperature_A";
const oops::Variables AirPotentialTemperature_A::Ingredients{
      std::vector<std::string>{"air_temperature", "air_pressure_at_surface"}};

// Register the maker
static RecipeMaker<AirPotentialTemperature_A> makerTempToPTemp_(AirPotentialTemperature_A::Name);

AirPotentialTemperature_A::AirPotentialTemperature_A(const Parameters_ & params,
                                    const VaderConfigVars & configVariables):
                                            configVariables_{configVariables}
{
    oops::Log::trace() << "AirPotentialTemperature_A::AirPotentialTemperature_A(params)"
        << std::endl;
}

std::string AirPotentialTemperature_A::name() const
{
    return AirPotentialTemperature_A::Name;
}

oops::Variable AirPotentialTemperature_A::product() const
{
    return oops::Variable("air_potential_temperature");
}

oops::Variables AirPotentialTemperature_A::ingredients() const
{
    return AirPotentialTemperature_A::Ingredients;
}

size_t AirPotentialTemperature_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace AirPotentialTemperature_A::productFunctionSpace
                                                (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").functionspace();
}

// -------------------------------------------------------------------------------------------------

void AirPotentialTemperature_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering AirPotentialTemperature_A::executeNL function"
        << std::endl;

    // Extract values from client config
    const double p0 = configVariables_.getDouble("reference_pressure");
    const double kappa = configVariables_.getDouble("kappa");  // Need better name

    // Get the fields
    atlas::Field temp  = afieldset.field("air_temperature");
    atlas::Field ps    = afieldset.field("air_pressure_at_surface");
    atlas::Field ptemp = afieldset.field("air_potential_temperature");

    std::string t_units, ps_units;

    afieldset.field("air_temperature").metadata().get("units", t_units);
    ASSERT_MSG(t_units.empty() || t_units == "K", "AirPotentialTemperature_A::executeNL: "
                            "Incorrect units for air_temperature");
    afieldset.field("air_pressure_at_surface").metadata().get("units", ps_units);
    ASSERT_MSG(ps_units.empty() || ps_units == "Pa", "AirPotentialTemperature_A::executeNL: "
                            "Incorrect units for air_pressure_at_surface");
    oops::Log::debug() << "AirPotentialTemperature_A::execute: p0 value: " << p0
        << std::endl;
    oops::Log::debug() << "AirPotentialTemperature_A::execute: kappa value: " << kappa
        << std::endl;

    util::for_each_column(
      [&](const auto t_col,
          const auto ps_col,
          auto ptemp_col) {
          const int nLevels = t_col.shape(0);
          for (int level = 0; level < nLevels; ++level) {
              ptemp_col(level) = t_col(level) * std::pow(p0 / ps_col(0), kappa);
          }
      },
      temp,
      ps,
      ptemp);

    oops::Log::trace() << "leaving AirPotentialTemperature_A::executeNL function" << std::endl;
}

}  // namespace vader
