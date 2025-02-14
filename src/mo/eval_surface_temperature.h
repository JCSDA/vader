/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field.h"

namespace mo {
/// \details This calculates the surface temperature from the atmospheric
///          temperature at the lowest level  (TO BE RETIRED)
void eval_surface_temperature_nl(atlas::FieldSet & stateFlds);

/// \details This calculates the surface temperature increment from the atmospheric
///          temperature increment at the lowest level (TO BE RETIRED)
void eval_surface_temperature_tl(atlas::FieldSet & incFlds);

/// \details This calculates the adjoint of the atmospheric temperature
///          at the lowest level from the adjoint of the surface temperature
///          (TO BE RETIRED)
void eval_surface_temperature_ad(atlas::FieldSet & hatFlds);


/// \details This calculates the surface temperature from the atmospheric
///          exner and potential temperature at the lowest level
void eval_surface_temperature_from_exner_nl(atlas::FieldSet & stateFlds);

/// \details This calculates the surface temperature increment from the atmospheric
///           exner and potential temperature  at the lowest level
void eval_surface_temperature_from_exner_tl(atlas::FieldSet & incFlds,
                                            const atlas::FieldSet & stateFlds);

/// \details This calculates the adjoint of the tangent linear of calculation of the
///          surface temperature from the exner and potential temperature.
///          at the lowest level from the adjoint of the surface temperature
void eval_surface_temperature_from_exner_ad(atlas::FieldSet & hatFlds,
                                            const atlas::FieldSet & stateFlds);



}  // namespace mo
