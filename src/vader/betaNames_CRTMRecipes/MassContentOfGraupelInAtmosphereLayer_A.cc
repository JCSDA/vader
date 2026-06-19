/*
 * (C) Copyright 2025 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/MassContentOfGraupelInAtmosphereLayer.h"

namespace vader {

// -------------------------------------------------------------------------------------------------
// Static attribute initialization
const char MassContentOfGraupelInAtmosphereLayer_A::Name[] =
    "MassContentOfGraupelInAtmosphereLayer_A";
const oops::Variables MassContentOfGraupelInAtmosphereLayer_A::Ingredients{
    std::vector<std::string>{
      "graupel", "air_pressure_thickness"}};

// Register the maker
static RecipeMaker<MassContentOfGraupelInAtmosphereLayer_A>
  makerMassContentOfGraupelInAtmosphereLayer_A_(
      MassContentOfGraupelInAtmosphereLayer_A::Name);

// -------------------------------------------------------------------------------------------------

MassContentOfGraupelInAtmosphereLayer_A::MassContentOfGraupelInAtmosphereLayer_A(
    const Parameters_ & params, const VaderConfigVars & configVariables):
    configVariables_{configVariables} {
  oops::Log::trace() << "MassContentOfGraupelInAtmosphereLayer_A::"
                     << "MassContentOfGraupelInAtmosphereLayer_A" << std::endl;
}

std::string MassContentOfGraupelInAtmosphereLayer_A::name() const {
  return MassContentOfGraupelInAtmosphereLayer_A::Name;
}

oops::Variable MassContentOfGraupelInAtmosphereLayer_A::product() const {
  return oops::Variable{"mass_content_of_graupel_in_atmosphere_layer"};
}

oops::Variables MassContentOfGraupelInAtmosphereLayer_A::ingredients() const {
  return MassContentOfGraupelInAtmosphereLayer_A::Ingredients;
}

oops::Variables MassContentOfGraupelInAtmosphereLayer_A::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{"graupel", "air_pressure_thickness"}};
}

size_t MassContentOfGraupelInAtmosphereLayer_A::productLevels(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("graupel").shape(1);
}

atlas::FunctionSpace MassContentOfGraupelInAtmosphereLayer_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("graupel").functionspace();
}
// -------------------------------------------------------------------------------------------------

void MassContentOfGraupelInAtmosphereLayer_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "MassContentOfGraupelInAtmosphereLayer_A::executeNL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double graupel_mr,
          const double delta_p,
          double& graupel_wp) {
          graupel_wp = graupel_mr * delta_p / g;
      },
      afieldset.field("graupel"),
      afieldset.field("air_pressure_thickness"),
      afieldset.field("mass_content_of_graupel_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfGraupelInAtmosphereLayer_A::executeNL done"
                     << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfGraupelInAtmosphereLayer_A::executeTL(atlas::FieldSet & afieldsetTL,
                                                         const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfGraupelInAtmosphereLayer_A::executeTL starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          const double graupel_mr_tl,
          double& graupel_wp_tl) {
          graupel_wp_tl = graupel_mr_tl * delta_p / g;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetTL.field("graupel"),
      afieldsetTL.field("mass_content_of_graupel_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfGraupelInAtmosphereLayer_A::executeTL done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void MassContentOfGraupelInAtmosphereLayer_A::executeAD(atlas::FieldSet & afieldsetAD,
                                                         const atlas::FieldSet & afieldsetTraj) {
  oops::Log::trace() << "MassContentOfGraupelInAtmosphereLayer_A::executeAD starting"
                     << std::endl;

  const double g = configVariables_.getDouble("standard_gravitational_acceleration");

  util::for_each_value(
      [=](const double delta_p,
          double& graupel_mr_ad,
          double& graupel_wp_ad) {
          graupel_mr_ad += graupel_wp_ad * delta_p / g;
          graupel_wp_ad = 0.0;
      },
      afieldsetTraj.field("air_pressure_thickness"),
      afieldsetAD.field("graupel"),
      afieldsetAD.field("mass_content_of_graupel_in_atmosphere_layer"));

  oops::Log::trace() << "MassContentOfGraupelInAtmosphereLayer_A::executeAD done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader
