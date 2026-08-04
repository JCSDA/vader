/*
 * (C) Crown Copyright 2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "mo/eval_relative_humidity.h"
#include "oops/util/Logger.h"
#include "vader/recipes/RelativeHumidityAt2m.h"

namespace vader {

// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char RelativeHumidityAt2m_A::Name[] =
    "RelativeHumidityAt2m_A";
const oops::Variables RelativeHumidityAt2m_A::Ingredients{
    std::vector<std::string>{"relative_humidity_at_2m_percentage"}};

// Register the maker
static RecipeMaker<RelativeHumidityAt2m_A>
    makerRelativeHumidityAt2m_A_(
        RelativeHumidityAt2m_A::Name);

// ------------------------------------------------------------------------------------------------

RelativeHumidityAt2m_A::RelativeHumidityAt2m_A(
    const Parameters_ & params,
    const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace()
      << "RelativeHumidityAt2m_A::RelativeHumidityAt2m_A()"
      << std::endl;
}

// ------------------------------------------------------------------------------------------------

std::string RelativeHumidityAt2m_A::name() const
{
    return RelativeHumidityAt2m_A::Name;
}

// ------------------------------------------------------------------------------------------------

oops::Variable RelativeHumidityAt2m_A::product() const
{
    return oops::Variable{"relative_humidity_at_2m"};
}

// ------------------------------------------------------------------------------------------------

oops::Variables RelativeHumidityAt2m_A::ingredients() const
{
    return RelativeHumidityAt2m_A::Ingredients;
}

// ------------------------------------------------------------------------------------------------

oops::Variables RelativeHumidityAt2m_A::trajectoryVars() const
{
    return oops::Variables(
        std::vector<std::string>{"relative_humidity_at_2m_percentage"});
}

// ------------------------------------------------------------------------------------------------

size_t RelativeHumidityAt2m_A::productLevels(
    const atlas::FieldSet & afieldset) const
{
    return afieldset.field("relative_humidity_at_2m_percentage").shape(1);
}

// ------------------------------------------------------------------------------------------------

atlas::FunctionSpace RelativeHumidityAt2m_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const
{
    return afieldset.field("relative_humidity_at_2m_percentage").functionspace();
}

// ------------------------------------------------------------------------------------------------

void RelativeHumidityAt2m_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace()
        << "entering RelativeHumidityAt2m_A::executeNL" << std::endl;

    mo::eval_relative_humidity_at_2m_percentage_to_fraction_nl(afieldset);

    oops::Log::trace()
        << "leaving RelativeHumidityAt2m_A::executeNL" << std::endl;
}

// ------------------------------------------------------------------------------------------------

void RelativeHumidityAt2m_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering RelativeHumidityAt2m_A::executeTL" << std::endl;

    mo::eval_relative_humidity_at_2m_percentage_to_fraction_tl(afieldsetTL);

    oops::Log::trace()
        << "leaving RelativeHumidityAt2m_A::executeTL" << std::endl;
}

// ------------------------------------------------------------------------------------------------

void RelativeHumidityAt2m_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                        const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace()
        << "entering RelativeHumidityAt2m_A::executeAD" << std::endl;

    mo::eval_relative_humidity_at_2m_percentage_to_fraction_ad(afieldsetAD);

    oops::Log::trace()
        << "leaving RelativeHumidityAt2m_A::executeAD" << std::endl;
}

}  // namespace vader
