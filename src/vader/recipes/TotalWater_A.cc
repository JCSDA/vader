/*
 * (C) Crown Copyright 2023 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/field.h"
// #include "atlas/util/Metadata.h"
#include "mo/eval_moisture_incrementing_operator.h"
#include "oops/util/Logger.h"
#include "vader/recipes/TotalWater.h"

namespace {
  const char specific_humidity_mo[] = "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water";
}  // namespace

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char TotalWater_A::Name[] = "TotalWater_A";
const oops::Variables TotalWater_A::
  Ingredients{std::vector<std::string>{specific_humidity_mo,
              "cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water",
              "cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water"}};

// Register the maker
static RecipeMaker<TotalWater_A> makerTotalWater_A_(TotalWater_A::Name);

TotalWater_A::TotalWater_A(const Parameters_ & params,
                           const VaderConfigVars & configVariables)
{
  oops::Log::trace() << "TotalWater_A::TotalWater_A(params, configVariables)" << std::endl;
}

std::string TotalWater_A::name() const
{
  return TotalWater_A::Name;
}

oops::Variable TotalWater_A::product() const
{
  return oops::Variable{"qt"};
}

oops::Variables TotalWater_A::ingredients() const
{
  return TotalWater_A::Ingredients;
}

oops::Variables TotalWater_A::trajectoryVars() const
{
    return oops::Variables{};
}

size_t TotalWater_A::productLevels(const atlas::FieldSet & afieldset) const
{
  return (afieldset[specific_humidity_mo].shape(1));
}

atlas::FunctionSpace TotalWater_A::productFunctionSpace(const atlas::FieldSet
                                                        & afieldset) const
{
  return afieldset[specific_humidity_mo].functionspace();
}

void TotalWater_A::executeNL(atlas::FieldSet & afieldset)
{
  oops::Log::trace() << "entering TotalWater_A::executeNL function" << std::endl;
  // There is no non-linear version of this, the full version
  // is given by the tangent linear.
  mo::eval_total_water_tl(afieldset, afieldset);
  oops::Log::trace() << "leaving TotalWater_A::executeNL function" << std::endl;
}

void TotalWater_A::executeTL(atlas::FieldSet & afieldsetTL,
                             const atlas::FieldSet & afieldsetTraj)
{
  oops::Log::trace() << "entering TotalWater_A::executeTL function" << std::endl;
  mo::eval_total_water_tl(afieldsetTL, afieldsetTraj);
  oops::Log::trace() << "leaving TotalWater_A::executeTL function" << std::endl;
}

void TotalWater_A::executeAD(atlas::FieldSet & afieldsetAD,
                             const atlas::FieldSet & afieldsetTraj)
{
  oops::Log::trace() << "entering TotalWater::executeAD function" << std::endl;
  mo::eval_total_water_ad(afieldsetAD, afieldsetTraj);
  oops::Log::trace() << "leaving TotalWater_A::executeAD function" << std::endl;
}

}  // namespace vader
