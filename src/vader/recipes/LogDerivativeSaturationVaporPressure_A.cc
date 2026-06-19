/*
 * (C) Crown Copyright 2024 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */


#include <iostream>
#include <vector>

#include "atlas/field/Field.h"
#include "mo/eval_sat_vapour_pressure.h"
#include "oops/util/Logger.h"
#include "vader/recipes/LogDerivativeSaturationVaporPressure.h"


namespace vader {


// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char LogDerivativeSaturationVaporPressure_A::Name[] =
  "LogDerivativeSaturationVaporPressure_A";

// Register the maker
static RecipeMaker<LogDerivativeSaturationVaporPressure_A>
  makerLogDerivativeSaturationVaporPressure_A_(LogDerivativeSaturationVaporPressure_A::Name);

// -------------------------------------------------------------------------------------------------

LogDerivativeSaturationVaporPressure_A::LogDerivativeSaturationVaporPressure_A(
  const Parameters_ & params,
  const VaderConfigVars & configVariables) {
  oops::Log::trace() <<
    "LogDerivativeSaturationVaporPressure_A::LogDerivativeSaturationVaporPressure_A"
    << std::endl;
}

std::string LogDerivativeSaturationVaporPressure_A::name() const {
  return LogDerivativeSaturationVaporPressure_A::Name;
}

oops::Variable LogDerivativeSaturationVaporPressure_A::product() const {
  return oops::Variable{"dlsvpdT"};
}

oops::Variables LogDerivativeSaturationVaporPressure_A::ingredients() const {
  return oops::Variables{std::vector<std::string>{"air_temperature"}};
}

size_t LogDerivativeSaturationVaporPressure_A::productLevels(
  const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace LogDerivativeSaturationVaporPressure_A::productFunctionSpace(
  const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void LogDerivativeSaturationVaporPressure_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() <<
    "LogDerivativeSaturationVaporPressure_A::executeNL starting ..." << std::endl;
  mo::eval_derivative_ln_svp_wrt_temperature_nl(afieldset);

  oops::Log::trace() <<
    "LogDerivativeSaturationVaporPressure_A::executeNL ... done" << std::endl;
}


// -------------------------------------------------------------------------------------------------

}  // namespace vader
