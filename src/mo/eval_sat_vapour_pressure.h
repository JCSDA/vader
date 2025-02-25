/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>

#include "atlas/field/FieldSet.h"

namespace mo {

/// USED IN VARTRANSFORMS
/// USED IN RECONFIGURESTATEGAUSS
/// USED IN RECIPE SaturationVaporPressure_A
/// \brief function to evaluate the 'saturation water pressure (svp)'
void eval_sat_vapour_pressure_nl(atlas::FieldSet & fields);

/// USED IN VARTRANSFORMS
/// USED IN RECONFIGURESTATEGAUSS
/// USED IN RECIPE LogDerivativeSaturationVaporPressure_A
/// \brief function to evaluate the 'derivative of ln water vapor partial pressure
/// assuming saturation at interface wrt air temperature at interface (dlsvpdT)'
void eval_derivative_ln_svp_wrt_temperature_nl(atlas::FieldSet & fields);

}  // namespace mo
