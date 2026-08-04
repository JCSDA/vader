/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

  /// USED IN RECONFIGURE STATE GAUSS
  /// USED IN RECIPE TotalRelativeHumidity_A
  /// \brief function to evaluate the 'total relative humidity':
  ///   rh = (q+qcl+qci+qrain)/qsat
  /// where ...
  ///   q  = specific humidity
  ///   qcl = specific cloud water
  ///   qci = specific cloud ice
  ///   qrain = specific rain
  ///   qsat  = saturated specific humidity
  void eval_total_relative_humidity_nl(atlas::FieldSet & fields);

}  // namespace mo
