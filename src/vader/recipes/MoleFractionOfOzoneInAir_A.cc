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
#include "vader/recipes/MoleFractionOfOzoneInAir.h"

namespace vader
{
// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char MoleFractionOfOzoneInAir_A::Name[] = "MoleFractionOfOzoneInAir_A";
const oops::Variables MoleFractionOfOzoneInAir_A::Ingredients{std::vector<std::string>{
                                                   "ozone_mass_mixing_ratio"}};

// Register the maker
static RecipeMaker<MoleFractionOfOzoneInAir_A> makerMoleFractionOfOzoneInAir_A_(
                                                                  MoleFractionOfOzoneInAir_A::Name);

MoleFractionOfOzoneInAir_A::MoleFractionOfOzoneInAir_A(const Parameters_ & /*params*/,
                                                       const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "MoleFractionOfOzoneInAir_A::MoleFractionOfOzoneInAir_A(params)"
      << std::endl;
}

std::string MoleFractionOfOzoneInAir_A::name() const
{
    return MoleFractionOfOzoneInAir_A::Name;
}

oops::Variable MoleFractionOfOzoneInAir_A::product() const
{
    return oops::Variable{"mole_fraction_of_ozone_in_air"};
}

oops::Variables MoleFractionOfOzoneInAir_A::ingredients() const
{
    return MoleFractionOfOzoneInAir_A::Ingredients;
}

size_t MoleFractionOfOzoneInAir_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("ozone_mass_mixing_ratio").shape(1);
}

atlas::FunctionSpace MoleFractionOfOzoneInAir_A::productFunctionSpace(const atlas::FieldSet &
                                                                                    afieldset) const
{
    return afieldset.field("ozone_mass_mixing_ratio").functionspace();
}

// -------------------------------------------------------------------------------------------------

void MoleFractionOfOzoneInAir_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering MoleFractionOfOzoneInAir_A::executeNL" << std::endl;

    // Extract values from client config
    const double ozone_mass_mixing_ratio_to_mole_fraction_ppm =
                     configVariables_.getDouble("constoz");

    util::for_each_value(
      [ozone_mass_mixing_ratio_to_mole_fraction_ppm](const double ozone_mass_mixing_ratio,
                    double & ozone_mole_fraction) {
          ozone_mole_fraction =
              ozone_mass_mixing_ratio * ozone_mass_mixing_ratio_to_mole_fraction_ppm;
      },
      afieldset["ozone_mass_mixing_ratio"],
      afieldset["mole_fraction_of_ozone_in_air"]);

    oops::Log::trace() << "leaving MoleFractionOfOzoneInAir_A::executeNL" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MoleFractionOfOzoneInAir_A::executeTL(atlas::FieldSet & afieldsetTL,
                                     const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering MoleFractionOfOzoneInAir_A::executeTL" << std::endl;

    // Extract values from client config
    const double ozone_mass_mixing_ratio_to_mole_fraction_ppm =
                     configVariables_.getDouble("constoz");

    util::for_each_value(
      [ozone_mass_mixing_ratio_to_mole_fraction_ppm](const double ozone_mass_mixing_ratio_tl,
                    double & ozone_mole_fraction_tl) {
          ozone_mole_fraction_tl =
              ozone_mass_mixing_ratio_tl * ozone_mass_mixing_ratio_to_mole_fraction_ppm;
      },
      afieldsetTL["ozone_mass_mixing_ratio"],
      afieldsetTL["mole_fraction_of_ozone_in_air"]);

    oops::Log::trace() << "leaving MoleFractionOfOzoneInAir_A::executeTL" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MoleFractionOfOzoneInAir_A::executeAD(
    atlas::FieldSet & afieldsetAD,
    const atlas::FieldSet & /*afieldsetTraj*/)
{
    oops::Log::trace() << "entering MoleFractionOfOzoneInAir_A::executeAD" << std::endl;

    // Extract values from client config
    const double ozone_mass_mixing_ratio_to_mole_fraction_ppm =
                     configVariables_.getDouble("constoz");

    util::for_each_value(
      [ozone_mass_mixing_ratio_to_mole_fraction_ppm](double & ozone_mass_mixing_ratio_ad,
                    double & ozone_mole_fraction_ad) {
          ozone_mass_mixing_ratio_ad +=
              ozone_mole_fraction_ad * ozone_mass_mixing_ratio_to_mole_fraction_ppm;
          ozone_mole_fraction_ad = 0.0;
      },
      afieldsetAD["ozone_mass_mixing_ratio"],
      afieldsetAD["mole_fraction_of_ozone_in_air"]);

    oops::Log::trace() << "leaving MoleFractionOfOzoneInAir_A::executeAD" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
