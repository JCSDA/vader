/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/IceAreaFraction.h"

namespace vader {

// Static attribute initialization
const char IceAreaFraction_A::Name[] = "IceAreaFraction_A";
const oops::Variables IceAreaFraction_A::Ingredients{
    std::vector<std::string>{"landmask", "seaice_fraction"}};

// Register the maker
static RecipeMaker<IceAreaFraction_A> makerIceAreaFraction_A_(
    IceAreaFraction_A::Name);

IceAreaFraction_A::IceAreaFraction_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables) {
  oops::Log::trace() << "IceAreaFraction_A::IceAreaFraction_A(params)"
                     << std::endl;
}

std::string IceAreaFraction_A::name() const {
  return IceAreaFraction_A::Name;
}

oops::Variable IceAreaFraction_A::product() const {
  return oops::Variable{"ice_area_fraction"};
}

oops::Variables IceAreaFraction_A::ingredients() const {
  return IceAreaFraction_A::Ingredients;
}

size_t IceAreaFraction_A::productLevels(const atlas::FieldSet & afieldset) const {
  return 1;
}

oops::Variables IceAreaFraction_A::trajectoryVars() const {
  return oops::Variables(std::vector<std::string>{"landmask", "seaice_fraction"});
}

atlas::FunctionSpace IceAreaFraction_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("landmask").functionspace();
}
// -------------------------------------------------------------------------------------------------

void IceAreaFraction_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "IceAreaFraction_A::executeNL starting" << std::endl;

  util::for_each_value(
      [](const double landmask,
         const double seaice,
         double& ice_frac) {
          ice_frac = (1.0 - landmask) * seaice;
      },
      afieldset.field("landmask"),
      afieldset.field("seaice_fraction"),
      afieldset.field("ice_area_fraction"));

  oops::Log::trace() << "IceAreaFraction_A::executeNL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void IceAreaFraction_A::executeTL(atlas::FieldSet & afieldsetTL,
                                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "IceAreaFraction_A::executeTL starting" << std::endl;

  util::for_each_value(
      [](const double landmask_traj,
         const double seaice_traj,
         const double landmask_tl,
         const double seaice_tl,
         double& ice_frac_tl) {
          ice_frac_tl = -seaice_traj * landmask_tl + (1.0 - landmask_traj) * seaice_tl;
      },
      afieldsetTraj.field("landmask"),
      afieldsetTraj.field("seaice_fraction"),
      afieldsetTL.field("landmask"),
      afieldsetTL.field("seaice_fraction"),
      afieldsetTL.field("ice_area_fraction"));

  oops::Log::trace() << "IceAreaFraction_A::executeTL done" << std::endl;
}
// -------------------------------------------------------------------------------------------------

void IceAreaFraction_A::executeAD(atlas::FieldSet & afieldsetAD,
                                    const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "IceAreaFraction_A::executeAD starting" << std::endl;

  util::for_each_value(
      [](const double landmask_traj,
         const double seaice_traj,
         double& landmask_ad,
         double& seaice_ad,
         double& ice_frac_ad) {
          landmask_ad += -seaice_traj * ice_frac_ad;
          seaice_ad += (1.0 - landmask_traj) * ice_frac_ad;
          ice_frac_ad = 0.0;
      },
      afieldsetTraj.field("landmask"),
      afieldsetTraj.field("seaice_fraction"),
      afieldsetAD.field("landmask"),
      afieldsetAD.field("seaice_fraction"),
      afieldsetAD.field("ice_area_fraction"));

  oops::Log::trace() << "IceAreaFraction_A::executeAD done" << std::endl;
}

}  // namespace vader
