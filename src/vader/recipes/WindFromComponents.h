/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/Field.h"

namespace vader {

/*! \brief Shared wind speed and direction calculations for single-level wind components.
 *
 *  \details Used by the recipes that compute speed or direction from eastward (u) and
 *           northward (v) components at one level, so that the 10 m and surface-adjacent-layer
 *           versions share one implementation. All fields have a single level. The TL and AD
 *           functions take the trajectory components as their first two arguments.
 */

/// speed = sqrt(u^2 + v^2)
void windSpeedNL(const atlas::Field & u, const atlas::Field & v, atlas::Field & speed);
void windSpeedTL(const atlas::Field & u, const atlas::Field & v,
                 const atlas::Field & u_tl, const atlas::Field & v_tl, atlas::Field & speed_tl);
void windSpeedAD(const atlas::Field & u, const atlas::Field & v,
                 atlas::Field & u_ad, atlas::Field & v_ad, atlas::Field & speed_ad);

/// Direction the wind blows toward, clockwise from north, in [0, 360):
/// direction = atan2(u, v) * radToDeg (the CRTM convention; 90 = blowing toward east).
void windToDirectionNL(const atlas::Field & u, const atlas::Field & v, atlas::Field & dir,
                       double radToDeg);
void windToDirectionTL(const atlas::Field & u, const atlas::Field & v,
                       const atlas::Field & u_tl, const atlas::Field & v_tl,
                       atlas::Field & dir_tl, double radToDeg);
void windToDirectionAD(const atlas::Field & u, const atlas::Field & v,
                       atlas::Field & u_ad, atlas::Field & v_ad,
                       atlas::Field & dir_ad, double radToDeg);

}  // namespace vader
