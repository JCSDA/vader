/*
 * (C) Crown Copyright 2024 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */


#include <iostream>
#include <vector>

#include "atlas/field/Field.h"
#include "mo/common_varchange.h"
#include "oops/util/Logger.h"
#include "vader/recipes/SaturationSpecificHumidity.h"


namespace vader {


// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char SaturationSpecificHumidity_A::Name[] = "SaturationSpecificHumidity_A";

// Register the maker
static RecipeMaker<SaturationSpecificHumidity_A>
  makerSaturationSpecificHumidity_A_(SaturationSpecificHumidity_A::Name);

// -------------------------------------------------------------------------------------------------

SaturationSpecificHumidity_A::SaturationSpecificHumidity_A(const Parameters_ & params,
  const VaderConfigVars & configVariables) {
  oops::Log::trace() << "SaturationSpecificHumidity_A::SaturationSpecificHumidity_A"
                     << std::endl;
}

std::string SaturationSpecificHumidity_A::name() const {
  return SaturationSpecificHumidity_A::Name;
}

oops::Variable SaturationSpecificHumidity_A::product() const {
  return oops::Variable{"qsat"};
}

oops::Variables SaturationSpecificHumidity_A::ingredients() const {
  return oops::Variables{std::vector<std::string>{"air_pressure",
          "svp",
          "air_temperature"}};
}

size_t SaturationSpecificHumidity_A::productLevels(const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_pressure").shape(1);
}

atlas::FunctionSpace SaturationSpecificHumidity_A::productFunctionSpace(
  const atlas::FieldSet & afieldset) const {
  return afieldset.field("air_pressure").functionspace();
}
// -------------------------------------------------------------------------------------------------

void SaturationSpecificHumidity_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "SaturationSpecificHumidity_A::executeNL starting ..." << std::endl;
  mo::evalSatSpecificHumidity(afieldset);
  oops::Log::trace() << "SaturationSpecificHumidity_A::executeNL ... done" << std::endl;
}


// -------------------------------------------------------------------------------------------------

}  // namespace vader
