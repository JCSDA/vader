/*
 * (C) Copyright 2023-2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/LnAirPressureAtInterface.h"

namespace vader {


// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char LnAirPressureAtInterface_A::Name[] = "LnAirPressureAtInterface_A";
const oops::Variables LnAirPressureAtInterface_A::Ingredients{
      std::vector<std::string>{"air_pressure_levels"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<LnAirPressureAtInterface_A>
       makerLnAirPressureAtInterface_A_(LnAirPressureAtInterface_A::Name);

// -------------------------------------------------------------------------------------------------

LnAirPressureAtInterface_A::LnAirPressureAtInterface_A(const
                                                     LnAirPressureAtInterface_AParameters & params,
                                                     const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "LnAirPressureAtInterface_A::LnAirPressureAtInterface_A" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string LnAirPressureAtInterface_A::name() const {
    return LnAirPressureAtInterface_A::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable LnAirPressureAtInterface_A::product() const {
    return oops::Variable{"ln_air_pressure_at_interface"};
}

// -------------------------------------------------------------------------------------------------

oops::Variables LnAirPressureAtInterface_A::ingredients() const {
    return LnAirPressureAtInterface_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

oops::Variables LnAirPressureAtInterface_A::trajectoryVars() const {
    return oops::Variables{std::vector<std::string>{"air_pressure_levels"}};
}

// -------------------------------------------------------------------------------------------------

size_t LnAirPressureAtInterface_A::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_levels").shape(1);
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace LnAirPressureAtInterface_A::productFunctionSpace(const
                                                                atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure_levels").functionspace();
}

// -------------------------------------------------------------------------------------------------

void LnAirPressureAtInterface_A::executeNL(atlas::FieldSet & afieldset) {
    //
    oops::Log::trace() << "LnAirPressureAtInterface_A::executeNL Starting" << std::endl;

    util::for_each_value(
      [&](const double p_int, double& ln_p_int) {
          ln_p_int = log(p_int);
      },
      afieldset["air_pressure_levels"],
      afieldset["ln_air_pressure_at_interface"]);

    oops::Log::trace() << "LnAirPressureAtInterface_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void LnAirPressureAtInterface_A::executeTL(atlas::FieldSet & afieldsetTL,
                                     const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "LnAirPressureAtInterface_A::executeTL Starting" << std::endl;

    util::for_each_value(
      [&](const double p_int, const double p_int_tl, double& ln_p_int_tl) {
          ln_p_int_tl = p_int_tl / p_int;
      },
      afieldsetTraj["air_pressure_levels"],
      afieldsetTL["air_pressure_levels"],
      afieldsetTL["ln_air_pressure_at_interface"]);

    oops::Log::trace() << "LnAirPressureAtInterface_A::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void LnAirPressureAtInterface_A::executeAD(atlas::FieldSet & afieldsetAD,
                                     const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "LnAirPressureAtInterface_A::executeAD Starting" << std::endl;

    util::for_each_value(
      [&](const double p_int, double& p_int_ad, double& ln_p_int_ad) {
          if (ln_p_int_ad != 0.0) {
            p_int_ad += ln_p_int_ad / p_int;
            ln_p_int_ad = 0.0;
          }
      },
      afieldsetTraj["air_pressure_levels"],
      afieldsetAD["air_pressure_levels"],
      afieldsetAD["ln_air_pressure_at_interface"]);

    oops::Log::trace() << "LnAirPressureAtInterface_A::executeAD Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
