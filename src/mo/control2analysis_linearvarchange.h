/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/Field.h"
#include "atlas/field/FieldSet.h"

namespace mo {

/// \details Tangent linear approximation to the
///          transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner (hydrostatic_exner_levels_minus_one)
void thetavP2HexnerTL(atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds);

/// \details Adjoint of the tangent linear approximation to the
///          transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner (hexner)
void thetavP2HexnerAD(atlas::FieldSet & hatFlds, const atlas::FieldSet & augStateFlds);

/// \details Tangent linear approximation to the
///          transformation from hydrostatically-balanced exner (hexner)
///          to virtual potential temperature (thetav)
void hexner2ThetavTL(atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds);

/// \details Adjoint of the tangent linear approximation to the
///          transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner (hexner)
///          Note:: zeroing of hexnerHatView not done.
void hexner2ThetavAD(atlas::FieldSet & hatFlds, const atlas::FieldSet & augStateFlds);

/// \details Tangent linear approximation to create the scaled dry
///          density using exner and the temperature
///
///          rho'_d = rho ( exner'       potential_temperature' )
///                       (-------   -   --------- )
///                       ( exner        potential_temperature )
///          potential_temperature and potential_temperature' needs to be vertically interpolated
///          onto the rho_grid.
void evalDryAirDensityTL(atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds);

/// \details Adjoint of Tangent linear approximation to create the scaled dry
///          density using exner and the temperature
///
///          rho'_d = rho ( exner'       potential_temperature' )
///                       (-------   -   --------- )
///                       ( exner        potential_temperature  )
///          potential_temperature and potential_temperature' needs to be vertically interpolated
///          onto the rho_grid.
void evalDryAirDensityAD(atlas::FieldSet & hatFlds, const atlas::FieldSet & augStateFlds);

/// \details This calculates air temperature increments from "exner" and "theta"
void evalAirTemperatureTL(atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds);

/// \details This calculates the adjoint of the transform to air temperature increments.
void evalAirTemperatureAD(atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds);

/// \details Calculate qT increment from the sum of q, qcl and qcf increments
void qqclqcf2qtTL(atlas::FieldSet & incFields, const atlas::FieldSet &);

/// \details This is the adjoint of qqclqcf2qtTL
void qqclqcf2qtAD(atlas::FieldSet & hatFields, const atlas::FieldSet &);

/// \details This is the moisture incrementing operator (MIO) that determines the cloud condensate
///          increments from increments in total water and temperature
void qtTemperature2qqclqcfTL(atlas::FieldSet & incFlds,
                             const atlas::FieldSet & augStateFlds);

/// \details This is the adjoint of the MIO
void qtTemperature2qqclqcfAD(atlas::FieldSet & hatFlds,
                             const atlas::FieldSet & augStateFlds);
}  // namespace mo
