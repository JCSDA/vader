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
#include "vader/recipes/OzoneMassMixingRatio.h"

namespace vader
{
// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char OzoneMassMixingRatio_A::Name[] = "OzoneMassMixingRatio_A";
const oops::Variables OzoneMassMixingRatio_A::Ingredients{std::vector<std::string>{
                                                   "mole_fraction_of_ozone_in_air"}};

// Register the maker
static RecipeMaker<OzoneMassMixingRatio_A> makerOzoneMassMixingRatio_A_(
                                                                  OzoneMassMixingRatio_A::Name);

OzoneMassMixingRatio_A::OzoneMassMixingRatio_A(const Parameters_ & /*params*/,
                                               const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "OzoneMassMixingRatio_A::OzoneMassMixingRatio_A(params)"
      << std::endl;
}

std::string OzoneMassMixingRatio_A::name() const
{
    return OzoneMassMixingRatio_A::Name;
}

oops::Variable OzoneMassMixingRatio_A::product() const
{
    return oops::Variable{"ozone_mass_mixing_ratio"};
}

oops::Variables OzoneMassMixingRatio_A::ingredients() const
{
    return OzoneMassMixingRatio_A::Ingredients;
}

// The transform is a constant scaling and so does not mathematically need a trajectory.
// It is declared anyway because Vader::executePlanAD allocates each ingredient field using
// the trajectory as a shape template, so an ingredient absent from the trajectory cannot be
// sized. Declaring it here makes Vader plan mole_fraction_of_ozone_in_air into the trajectory.
oops::Variables OzoneMassMixingRatio_A::trajectoryVars() const
{
    return oops::Variables{std::vector<std::string>{"mole_fraction_of_ozone_in_air"}};
}

size_t OzoneMassMixingRatio_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("mole_fraction_of_ozone_in_air").shape(1);
}

atlas::FunctionSpace OzoneMassMixingRatio_A::productFunctionSpace(const atlas::FieldSet &
                                                                                afieldset) const
{
    return afieldset.field("mole_fraction_of_ozone_in_air").functionspace();
}

// -------------------------------------------------------------------------------------------------

void OzoneMassMixingRatio_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering OzoneMassMixingRatio_A::executeNL" << std::endl;

    // Extract values from client config
    const double ozone_mass_mixing_ratio_to_mole_fraction_ppm =
                     configVariables_.getDouble("constoz");

    util::for_each_value(
      [ozone_mass_mixing_ratio_to_mole_fraction_ppm](const double ozone_mole_fraction,
                    double & ozone_mass_mixing_ratio) {
          ozone_mass_mixing_ratio =
              ozone_mole_fraction / ozone_mass_mixing_ratio_to_mole_fraction_ppm;
      },
      afieldset["mole_fraction_of_ozone_in_air"],
      afieldset["ozone_mass_mixing_ratio"]);

    oops::Log::trace() << "leaving OzoneMassMixingRatio_A::executeNL" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void OzoneMassMixingRatio_A::executeTL(atlas::FieldSet & afieldsetTL,
                                       const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering OzoneMassMixingRatio_A::executeTL" << std::endl;

    // Extract values from client config
    const double ozone_mass_mixing_ratio_to_mole_fraction_ppm =
                     configVariables_.getDouble("constoz");

    util::for_each_value(
      [ozone_mass_mixing_ratio_to_mole_fraction_ppm](const double ozone_mole_fraction_tl,
                    double & ozone_mass_mixing_ratio_tl) {
          ozone_mass_mixing_ratio_tl =
              ozone_mole_fraction_tl / ozone_mass_mixing_ratio_to_mole_fraction_ppm;
      },
      afieldsetTL["mole_fraction_of_ozone_in_air"],
      afieldsetTL["ozone_mass_mixing_ratio"]);

    oops::Log::trace() << "leaving OzoneMassMixingRatio_A::executeTL" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void OzoneMassMixingRatio_A::executeAD(
    atlas::FieldSet & afieldsetAD,
    const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering OzoneMassMixingRatio_A::executeAD" << std::endl;

    // Extract values from client config
    const double ozone_mass_mixing_ratio_to_mole_fraction_ppm =
                     configVariables_.getDouble("constoz");

    util::for_each_value(
      [ozone_mass_mixing_ratio_to_mole_fraction_ppm](double & ozone_mole_fraction_ad,
                    double & ozone_mass_mixing_ratio_ad) {
          ozone_mole_fraction_ad +=
              ozone_mass_mixing_ratio_ad / ozone_mass_mixing_ratio_to_mole_fraction_ppm;
          ozone_mass_mixing_ratio_ad = 0.0;
      },
      afieldsetAD["mole_fraction_of_ozone_in_air"],
      afieldsetAD["ozone_mass_mixing_ratio"]);

    oops::Log::trace() << "leaving OzoneMassMixingRatio_A::executeAD" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
