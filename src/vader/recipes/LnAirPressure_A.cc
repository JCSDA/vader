/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/LnAirPressure.h"

namespace vader {


// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char LnAirPressure_A::Name[] = "LnAirPressure_A";
const oops::Variables LnAirPressure_A::Ingredients{
      std::vector<std::string>{"air_pressure"}};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<LnAirPressure_A>
       makerLnAirPressure_A_(LnAirPressure_A::Name);

// -------------------------------------------------------------------------------------------------

LnAirPressure_A::LnAirPressure_A(const LnAirPressure_AParameters & params,
                                 const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "LnAirPressure_A::LnAirPressure_A" << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string LnAirPressure_A::name() const {
    return LnAirPressure_A::Name;
}

// -------------------------------------------------------------------------------------------------

oops::Variable LnAirPressure_A::product() const {
    return oops::Variable{"ln_air_pressure"};
}

// -------------------------------------------------------------------------------------------------

oops::Variables LnAirPressure_A::ingredients() const {
    return LnAirPressure_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

oops::Variables LnAirPressure_A::trajectoryVars() const {
    return oops::Variables{std::vector<std::string>{"air_pressure"}};
}

// -------------------------------------------------------------------------------------------------

size_t LnAirPressure_A::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure").shape(1);
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace LnAirPressure_A::productFunctionSpace(const
                                                     atlas::FieldSet & afieldset) const {
    return afieldset.field("air_pressure").functionspace();
}

// -------------------------------------------------------------------------------------------------

void LnAirPressure_A::executeNL(atlas::FieldSet & afieldset) {
    //
    oops::Log::trace() << "LnAirPressure_A::executeNL Starting" << std::endl;

    util::for_each_value(
      [&](const double p, double& ln_p) {
        ln_p = std::log(p);
      },
      afieldset["air_pressure"],
      afieldset["ln_air_pressure"]);

    oops::Log::trace() << "LnAirPressure_A::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void LnAirPressure_A::executeTL(atlas::FieldSet & afieldsetTL,
                          const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "LnAirPressure_A::executeTL Starting" << std::endl;

    util::for_each_value(
        [&](const double p, const double p_tl, double& ln_p_tl) {
          ln_p_tl = p_tl / p;
        },
        afieldsetTraj["air_pressure"],
        afieldsetTL["air_pressure"],
        afieldsetTL["ln_air_pressure"]);

    oops::Log::trace() << "LnAirPressure_A::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void LnAirPressure_A::executeAD(atlas::FieldSet & afieldsetAD,
                          const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "LnAirPressure_A::executeAD Starting" << std::endl;

    util::for_each_value(
      [&](const double p, double& p_ad, double& ln_p_ad) {
        if (ln_p_ad != 0.0) {
          p_ad += ln_p_ad / p;
          ln_p_ad = 0.0;
        }
      },
      afieldsetTraj["air_pressure"],
      afieldsetAD["air_pressure"],
      afieldsetAD["ln_air_pressure"]);

    oops::Log::trace() << "LnAirPressure_A::executeAD Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
