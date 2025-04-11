/*
 * (C) Crown Copyright 2023 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "mo/eval_relative_humidity.h"
#include "oops/util/Logger.h"
#include "vader/recipes/RelativeHumidity.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char RelativeHumidity_A::Name[] = "RelativeHumidity_A";
const oops::Variables RelativeHumidity_A::Ingredients{std::vector<std::string>{"air_temperature",
                                    "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water",
                                    "qsat"}};

// Register the maker
static RecipeMaker<RelativeHumidity_A> makerRelativeHumidity_(RelativeHumidity_A::Name);

RelativeHumidity_A::RelativeHumidity_A(const Parameters_ & params,
                                       const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "RelativeHumidity_A::RelativeHumidity_A(params)" << std::endl;
}

std::string RelativeHumidity_A::name() const
{
    return RelativeHumidity_A::Name;
}

oops::Variable RelativeHumidity_A::product() const
{
    return oops::Variable{"relative_humidity"};
}

oops::Variables RelativeHumidity_A::ingredients() const
{
    return RelativeHumidity_A::Ingredients;
}

oops::Variables RelativeHumidity_A::trajectoryVars() const
{
    return oops::Variables({"dlsvpdT",
                            "water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water",
                            "qsat"});
}

size_t RelativeHumidity_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").shape(1);
}

atlas::FunctionSpace RelativeHumidity_A::productFunctionSpace
                                              (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").functionspace();
}

void RelativeHumidity_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering RelativeHumidity_A::executeNL function" << std::endl;

    // Actually requiring water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water and qsat only
    mo::eval_relative_humidity_nl(afieldset);

    oops::Log::trace() << "leaving RelativeHumidity_A::executeNL function" << std::endl;
}

void RelativeHumidity_A::executeTL(atlas::FieldSet & afieldsetTL,
                                   const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering RelativeHumidity_A::executeTL function" << std::endl;

    // Requiring air_temperature and water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water in
    // perturbation fieldSet, but water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water,
    // qsat and dlsvpdt in trajectory fieldSet.
    mo::eval_relative_humidity_tl(afieldsetTL, afieldsetTraj);

    oops::Log::trace() << "leaving RelativeHumidity_A::executeTL function" << std::endl;
}

void RelativeHumidity_A::executeAD(atlas::FieldSet & afieldsetAD,
                                   const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering RelativeHumidity_A::executeAD function" << std::endl;

    mo::eval_relative_humidity_ad(afieldsetAD, afieldsetTraj);

    oops::Log::trace() << "leaving RelativeHumidity_A::executeAD function" << std::endl;
}

}  // namespace vader
