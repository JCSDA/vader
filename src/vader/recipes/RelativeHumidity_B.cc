/*
 * (C) Copyright 2025 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "oops/util/for_each.h"
#include "oops/util/Logger.h"
#include "vader/recipes/RelativeHumidity.h"

namespace vader {

// Static attribute initialization
const char RelativeHumidity_B::Name[] = "RelativeHumidity_B";
const oops::Variables RelativeHumidity_B::Ingredients{std::vector<std::string>{
                                    "water_vapor_mixing_ratio_wrt_moist_air",
                                    "water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"}};

// Register the maker
static RecipeMaker<RelativeHumidity_B> makerRelativeHumidity_B_(RelativeHumidity_B::Name);

RelativeHumidity_B::RelativeHumidity_B(const Parameters_ & params,
                                       const VaderConfigVars & configVariables) :
    configVariables_{configVariables} {
    oops::Log::trace() << "RelativeHumidity_B::RelativeHumidity_B" << std::endl;
}

std::string RelativeHumidity_B::name() const {
    return RelativeHumidity_B::Name;
}

oops::Variable RelativeHumidity_B::product() const {
    return oops::Variable{"relative_humidity"};
}

oops::Variables RelativeHumidity_B::ingredients() const {
    return RelativeHumidity_B::Ingredients;
}

oops::Variables RelativeHumidity_B::trajectoryVars() const {
  return oops::Variables{std::vector<std::string>{
    "water_vapor_mixing_ratio_wrt_moist_air",
    "water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation",
    "relative_humidity"}};
}
size_t RelativeHumidity_B::productLevels(const atlas::FieldSet & afieldset) const {
    return afieldset.field("water_vapor_mixing_ratio_wrt_moist_air").shape(1);
}

atlas::FunctionSpace RelativeHumidity_B::productFunctionSpace(
                                              const atlas::FieldSet & afieldset) const {
    return afieldset.field("water_vapor_mixing_ratio_wrt_moist_air").functionspace();
}

// -------------------------------------------------------------------------------------------------

void RelativeHumidity_B::executeNL(atlas::FieldSet & afieldset) {
    oops::Log::trace() << "RelativeHumidity_B::executeNL Starting" << std::endl;

    util::for_each_value(
        [](const double q,
           const double qsat,
           double& rh) {
           if (qsat > 1.0e-12) {
               rh = q / qsat;
           } else {
               rh = 0.0;
           }
           rh = std::min(1.0, std::max(0.0, rh));
        },
        afieldset["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldset["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"],
        afieldset["relative_humidity"]);

    oops::Log::trace() << "RelativeHumidity_B::executeNL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void RelativeHumidity_B::executeTL(atlas::FieldSet & afieldsetTL,
                                    const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "RelativeHumidity_B::executeTL Starting" << std::endl;

    util::for_each_value(
        [](const double q,
           const double qsat,
           const double rh,
           const double q_tl,
           const double qsat_tl,
           double& rh_tl) {
           if (qsat > 1.0e-12 && rh >= 0.0 && rh < 1.0) {
              const double drh_dq = 1.0 / qsat;
              const double drh_dqsat = -1.0 * q / (qsat * qsat);

              rh_tl = drh_dq * q_tl + drh_dqsat * qsat_tl;
           } else {
              rh_tl = 0.0;
           }
        },
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"],
        afieldsetTraj["relative_humidity"],
        afieldsetTL["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetTL["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"],
        afieldsetTL["relative_humidity"]);

    oops::Log::trace() << "RelativeHumidity_B::executeTL Done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void RelativeHumidity_B::executeAD(atlas::FieldSet & afieldsetAD,
                                    const atlas::FieldSet & afieldsetTraj) {
    oops::Log::trace() << "RelativeHumidity_B::executeAD Starting" << std::endl;

    util::for_each_value(
        [](const double q,
           const double qsat,
           const double rh,
           double& q_ad,
           double& qsat_ad,
           double& rh_ad) {
            if (qsat > 1.0e-12 && rh >= 0.0 && rh < 1.0) {
                const double drh_dq = 1.0 / qsat;
                const double drh_dqsat = -1.0 * q / (qsat * qsat);
                q_ad += drh_dq * rh_ad;
                qsat_ad += drh_dqsat * rh_ad;
            }
            rh_ad = 0.0;
          },
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetTraj["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"],
        afieldsetTraj["relative_humidity"],
        afieldsetAD["water_vapor_mixing_ratio_wrt_moist_air"],
        afieldsetAD["water_vapor_mixing_ratio_wrt_moist_air_assuming_saturation"],
        afieldsetAD["relative_humidity"]);

    oops::Log::trace() << "RelativeHumidity_B::executeAD Done" << std::endl;
}

}  // namespace vader
