/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// USED IN VARTRANSFORMS
/// USED IN RECONFIGURESTATEGAUSS
/// \brief function to evaluate the 'relative humidity':
///   rh = q/qsat*100
/// where ...
///   q  = specific humidity
///   qsat  = saturated specific humidity
///
/// Note that we have a bool metadata switch that we need here "cap_super_sat"
/// If it is not present then we assume that it is false (which is the default)
void eval_relative_humidity_nl(atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// USED IN RECIPE RelativeHumidity_A
/// \details This calculates the relative humidity increment from the air
///          temperature and specific humidity increment as:
///          rh' ~ (q'/qs - (q/qs) dlnesdT T')*100
///          This assumes air pressure increments can be neglected (usual approximation)
///          Only part of this is the tangent linear of eval_relative_humidity_nl.
///          This neglects supersaturation.
void eval_relative_humidity_tl(atlas::FieldSet & incFlds,
                               const atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// USED IN RECIPE RelativeHumidity_A
/// \brief Adjoint of eval_relative_humidity_tl
void eval_relative_humidity_ad(atlas::FieldSet & hatFlds,
                               const atlas::FieldSet & stateFlds);

/// USED IN RECONFIGURE STATE GAUSS
/// \details This calculates the relative humidity increment at 2 m from the atmospheric
///          temperature increment at the lowest level
void eval_relative_humidity_at_2m_nl(atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// \details This calculates the relative humidity increment at 2 m from the atmospheric
///          temperature increment at the lowest level
void eval_relative_humidity_at_2m_from_temp_tl(atlas::FieldSet & incFlds,
                                               const atlas::FieldSet & stateFlds);

/// USED IN VARTRANSFORMS
/// \details Adjoint of eval_relative_humidity_at_2m_tl
void eval_relative_humidity_at_2m_from_temp_ad(atlas::FieldSet & hatFlds,
                                               const atlas::FieldSet & stateFlds);

}  // namespace mo

