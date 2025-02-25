/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field.h"


/// \brief ++ Variable Tranforms Repository ++
///
/// \details This repository contains a set of functions associated with
/// 'transformations'.
/// The transformations are applied to Atlas data structures.
///
/// Here the term 'transformation' is used in a broad sense.
/// Examples of transformations include model-specific variable changes.
///
/// This is only a temporary repository. In the long term, the Variable Transforms will be
/// stored and processed by the VAriable DErivation Repository (VADER) system.
///


namespace mo {

/// USED IN VARTRANSFORMS
/// \brief function to evaluate 'surf_param_a' and 'surf_param_b';
/// 'surf_param_a' and 'surf_param_b' are  two 'derived' fields used to evaluate
/// the background pressure at the observation height
///
///          surf_param_a = f(height_above_mean_sea_level_levels[0], t_msh)
///          surf_param_b = f(t_msh, pressure_level_minus_one[0])
/// where ...
///     height_above_mean_sea_level_levels[0] = model surface height
///            t_msh = temperature at model surface height
///     pressure_level_minus_one[0] = pressure at model surface height
///
/// input data (fields) needed for the calculations:
///     height = height on theta model levels (not including surface)
///        (it includes metadata "boundary_layer_index" )
///     height_above_mean_sea_level_levels = height on rho model levels
///     pressure_levels_minus_one = pressure on rho model levels
///     water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water
///
void evalParamAParamB(atlas::FieldSet & fields);

}  // namespace mo
