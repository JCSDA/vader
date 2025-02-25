/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */


#include <string>
#include <vector>

#include "atlas/field/for_each.h"

#include "mo/constants.h"
#include "mo/eval_sat_vapour_pressure.h"
#include "mo/functions.h"
#include "mo/lookups.h"

#include "oops/util/Logger.h"


namespace mo {

namespace {

/// \brief function to evaluate the several types of 'saturation water pressure'
///
///  input: Atlas data structure (FieldSet) containing 'air temperature'
/// output: 'svp' or 'dlsvpdT'
///
void eval_sat_vapour_pressure_variant(const std::string field_out,
                                      atlas::FieldSet & stateFlds)
{
  // normalised T lambda function
  // which enforces upper and lower bounds on T
  auto normalisedT = [](const double tVal) {
    double t = (tVal - constants::TLoBound)/constants::Tinc;
    double t1 = (t < 0.0 ? 0.0 : t);
    double t2 = (t1 >= static_cast<double>(constants::svpLookUpLength - 1)) ?
                       static_cast<double>(constants::svpLookUpLength - 1) : t1;
    return t2;
  };

  // lambda function returning the index of a given temperature value;
  // avoids returning last index in table
  auto index = [](const double normalisedTVal) {
    std::size_t i = static_cast<std::size_t>(normalisedTVal);
    return (i == constants::svpLookUpLength - 1 ? constants::svpLookUpLength - 2 : i);
  };

  // weight lamba function
  auto weight = [index](const double normalisedTVal) {
    std::size_t i = index(normalisedTVal);
    return ( normalisedTVal -  static_cast<double>(i) );
  };

  // lambda function to interpolate values from table to given temperature
  auto interp = [](const double weight, const double v1, const double v2) {
     return (weight * v2 + (1 - weight) * v1);
  };

  // ID to identify a type of 'saturation water pressure'
  int ival;
  if (field_out == "svp") {
    ival = 0;
  } else if (field_out == "dlsvpdT") {
    ival = 1;
  }

  // weight
  double w;
  // index
  std::size_t indx;

  const auto& lookup = lookups::allSvpLookupTable[ival];

  const auto & ghost = stateFlds[field_out].functionspace().ghost();
  atlas::field::for_each_value_masked(
    ghost,
    stateFlds["air_temperature"],
    stateFlds[field_out],
    [&](const double air_temp, double& svp) {
      indx = index(normalisedT(air_temp));
      w = weight(normalisedT(air_temp));
      svp = interp(w, lookup.at(indx), lookup.at(indx+1));
    });

  stateFlds[field_out].set_dirty();
}

}  // namespace

void eval_sat_vapour_pressure_nl(atlas::FieldSet & stateFlds)
{
  oops::Log::trace() << "[eval_sat_vapour_pressure_svp_nl()] starting ..." << std::endl;

  eval_sat_vapour_pressure_variant("svp", stateFlds);

  oops::Log::trace() << "[eval_sat_vapour_pressure_svp_nl()] ... done" << std::endl;
}


void eval_derivative_ln_svp_wrt_temperature_nl(atlas::FieldSet & stateFlds)
{
  oops::Log::trace() << "[eval_sat_vapour_pressure_dlsvpdT_nl()] starting ..." << std::endl;

  eval_sat_vapour_pressure_variant("dlsvpdT", stateFlds);

  oops::Log::trace() << "[eval_sat_vapour_pressure_dlsvpdT_nl()] ... done" << std::endl;
}

}  // namespace mo

