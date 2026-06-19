/*
 * (C) Crown Copyright 2024 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */


#include <iostream>
#include <vector>

#include "atlas/field/Field.h"
#include "mo/eval_total_relative_humidity.h"
#include "oops/util/Logger.h"
#include "vader/recipes/TotalRelativeHumidity.h"


namespace {
  const char specific_humidity_mo[] = "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water";
}  // namespace

namespace vader {


// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char TotalRelativeHumidity_A::Name[] = "TotalRelativeHumidity_A";

// Register the maker
static RecipeMaker<TotalRelativeHumidity_A>
  makerTotalRelativeHumidity_A_(TotalRelativeHumidity_A::Name);

// -------------------------------------------------------------------------------------------------

TotalRelativeHumidity_A::TotalRelativeHumidity_A(const Parameters_ & params,
  const VaderConfigVars & configVariables)
{
  oops::Log::trace() << "TotalRelativeHumidity_A::TotalRelativeHumidity_A"
                     << std::endl;
}

std::string TotalRelativeHumidity_A::name() const
{
  return TotalRelativeHumidity_A::Name;
}

oops::Variable TotalRelativeHumidity_A::product() const
{
  return oops::Variable{"rht"};
}

oops::Variables TotalRelativeHumidity_A::ingredients() const {
  return oops::Variables{std::vector<std::string>{specific_humidity_mo,
          "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
          "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water",
          "qrain",
          "qsat"}};
  // CCPP standard
  // return {"water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_interface",
  //        "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water_at_interface",
  //        "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water_at_interface",
  //        "rain_mixing_ratio_wrt_moist_air_and_condensed_water_at_interface",
  //        "qsat"};
}

size_t TotalRelativeHumidity_A::productLevels(const atlas::FieldSet & afieldset) const {
  return afieldset.field(specific_humidity_mo).shape(1);
}

atlas::FunctionSpace TotalRelativeHumidity_A::productFunctionSpace(
  const atlas::FieldSet & afieldset) const {
  return afieldset.field(specific_humidity_mo).functionspace();
}
// -------------------------------------------------------------------------------------------------

void TotalRelativeHumidity_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "TotalRelativeHumidity_A::executeNL starting ..." << std::endl;
  mo::eval_total_relative_humidity_nl(afieldset);

  oops::Log::trace() << "TotalRelativeHumidity_A::executeNL ... done" << std::endl;
}


// -------------------------------------------------------------------------------------------------

}  // namespace vader
