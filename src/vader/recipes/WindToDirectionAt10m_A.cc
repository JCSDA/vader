/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/WindFromComponents.h"
#include "vader/recipes/WindToDirectionAt10m.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char WindToDirectionAt10m_A::Name[] = "WindToDirectionAt10m_A";
const oops::Variables WindToDirectionAt10m_A::Ingredients{std::vector<std::string>{
                            "eastward_wind_at_10m",
                            "northward_wind_at_10m"}};

// Register the maker
static RecipeMaker<WindToDirectionAt10m_A> makerWindToDirectionAt10m_A_(
                                WindToDirectionAt10m_A::Name);

WindToDirectionAt10m_A::WindToDirectionAt10m_A(const Parameters_ & params,
                   const VaderConfigVars & configVariables) {
  oops::Log::trace() << "WindToDirectionAt10m_A::WindToDirectionAt10m_A(params)"
                     << std::endl;
  radToDeg_ = configVariables.getDouble("radians_to_degrees");
}

std::string WindToDirectionAt10m_A::name() const {
  return WindToDirectionAt10m_A::Name;
}

oops::Variable WindToDirectionAt10m_A::product() const {
  return oops::Variable{"wind_to_direction_at_10m"};
}

oops::Variables WindToDirectionAt10m_A::ingredients() const {
  return WindToDirectionAt10m_A::Ingredients;
}

oops::Variables WindToDirectionAt10m_A::trajectoryVars() const {
  return WindToDirectionAt10m_A::Ingredients;
}

size_t WindToDirectionAt10m_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

atlas::FunctionSpace WindToDirectionAt10m_A::productFunctionSpace(const atlas::FieldSet &
                                          afieldset) const {
  return afieldset.field("eastward_wind_at_10m").functionspace();
}

// -------------------------------------------------------------------------------------------------

void WindToDirectionAt10m_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "WindToDirectionAt10m_A::executeNL starting" << std::endl;
  windToDirectionNL(afieldset["eastward_wind_at_10m"],
                    afieldset["northward_wind_at_10m"],
                    afieldset["wind_to_direction_at_10m"], radToDeg_);
  oops::Log::trace() << "WindToDirectionAt10m_A::executeNL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindToDirectionAt10m_A::executeTL(atlas::FieldSet & afieldsetTL,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindToDirectionAt10m_A::executeTL starting" << std::endl;
  windToDirectionTL(afieldsetTraj["eastward_wind_at_10m"],
                    afieldsetTraj["northward_wind_at_10m"],
                    afieldsetTL["eastward_wind_at_10m"],
                    afieldsetTL["northward_wind_at_10m"],
                    afieldsetTL["wind_to_direction_at_10m"], radToDeg_);
  oops::Log::trace() << "WindToDirectionAt10m_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void WindToDirectionAt10m_A::executeAD(atlas::FieldSet & afieldsetAD,
                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "WindToDirectionAt10m_A::executeAD starting" << std::endl;
  windToDirectionAD(afieldsetTraj["eastward_wind_at_10m"],
                    afieldsetTraj["northward_wind_at_10m"],
                    afieldsetAD["eastward_wind_at_10m"],
                    afieldsetAD["northward_wind_at_10m"],
                    afieldsetAD["wind_to_direction_at_10m"], radToDeg_);
  oops::Log::trace() << "WindToDirectionAt10m_A::executeAD done" << std::endl;
}

}  // namespace vader
