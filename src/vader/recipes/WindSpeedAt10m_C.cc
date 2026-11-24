/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/WindFromComponents.h"
#include "vader/recipes/WindSpeedAt10m.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WindSpeedAt10m_C::Name[] = "WindSpeedAt10m_C";
const oops::Variables WindSpeedAt10m_C::Ingredients{std::vector<std::string>{
                            "eastward_wind_at_10m",
                            "northward_wind_at_10m"}};

// Register the maker
static RecipeMaker<WindSpeedAt10m_C> makerWindSpeedAt10m_C_(
                                WindSpeedAt10m_C::Name);

WindSpeedAt10m_C::WindSpeedAt10m_C(const Parameters_ & params,
                   const VaderConfigVars & configVariables) {
  oops::Log::trace() << "WindSpeedAt10m_C::WindSpeedAt10m_C(params)"
                     << std::endl;
}

std::string WindSpeedAt10m_C::name() const {
  return WindSpeedAt10m_C::Name;
}

oops::Variable WindSpeedAt10m_C::product() const {
  return oops::Variable{"wind_speed_at_10m"};
}

oops::Variables WindSpeedAt10m_C::ingredients() const {
  return WindSpeedAt10m_C::Ingredients;
}

oops::Variables WindSpeedAt10m_C::trajectoryVars() const {
  return WindSpeedAt10m_C::Ingredients;
}

size_t WindSpeedAt10m_C::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace WindSpeedAt10m_C::productFunctionSpace(const atlas::FieldSet &
                                          afieldset) const {
  return afieldset.field("eastward_wind_at_10m").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAt10m_C::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "WindSpeedAt10m_C::executeNL starting" << std::endl;
  windSpeedNL(afieldset["eastward_wind_at_10m"],
              afieldset["northward_wind_at_10m"],
              afieldset["wind_speed_at_10m"]);
  oops::Log::trace() << "WindSpeedAt10m_C::executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAt10m_C::executeTL(atlas::FieldSet & afieldsetTL,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindSpeedAt10m_C::executeTL starting" << std::endl;
  windSpeedTL(afieldsetTraj["eastward_wind_at_10m"],
              afieldsetTraj["northward_wind_at_10m"],
              afieldsetTL["eastward_wind_at_10m"],
              afieldsetTL["northward_wind_at_10m"],
              afieldsetTL["wind_speed_at_10m"]);
  oops::Log::trace() << "WindSpeedAt10m_C::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindSpeedAt10m_C::executeAD(atlas::FieldSet & afieldsetAD,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindSpeedAt10m_C::executeAD starting" << std::endl;
  windSpeedAD(afieldsetTraj["eastward_wind_at_10m"],
              afieldsetTraj["northward_wind_at_10m"],
              afieldsetAD["eastward_wind_at_10m"],
              afieldsetAD["northward_wind_at_10m"],
              afieldsetAD["wind_speed_at_10m"]);
  oops::Log::trace() << "WindSpeedAt10m_C::executeAD done" << std::endl;
}

}  // namespace vader
