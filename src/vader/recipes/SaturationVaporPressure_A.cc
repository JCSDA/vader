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
#include "vader/recipes/SaturationVaporPressure.h"


namespace vader {


// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char SaturationVaporPressure_A::Name[] = "SaturationVaporPressure_A";

// Register the maker
static RecipeMaker<SaturationVaporPressure_A>
  makerSaturationVaporPressure_A_(SaturationVaporPressure_A::Name);

// -------------------------------------------------------------------------------------------------

SaturationVaporPressure_A::SaturationVaporPressure_A(const Parameters_ & params,
  const VaderConfigVars & configVariables) {
  oops::Log::trace() << "SaturationVaporPressure_A::SaturationVaporPressure_A"
                     << std::endl;
}

std::string SaturationVaporPressure_A::name() const {
  return SaturationVaporPressure_A::Name;
}

oops::Variable SaturationVaporPressure_A::product() const {
  return oops::Variable{"svp"};
}

oops::Variables SaturationVaporPressure_A::ingredients() const {
  return oops::Variables{std::vector<std::string>{"air_temperature"}};
}

size_t SaturationVaporPressure_A::productLevels(const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace SaturationVaporPressure_A::productFunctionSpace(
  const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_temperature").functionspace();
}
// -------------------------------------------------------------------------------------------------

void SaturationVaporPressure_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SatVaporPressure_A::executeNL starting ..." << std::endl;
  mo::eval_sat_vapour_pressure_nl(afieldset);

  oops::Log::trace() << "SaturationVaporPressure_A::executeNL ... done" << std::endl;
}


// -------------------------------------------------------------------------------------------------

}  // namespace vader
