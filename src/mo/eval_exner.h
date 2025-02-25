/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field.h"

namespace mo {

/// TO BE RETIRED AFTER HpToHexner.cc is removed from saber (old B)
/// \details This calculates the hydrostatic exner field from the hydrostatic pressure
void eval_hydrostatic_exner_levels_tl(atlas::FieldSet & incFlds,
                                      const atlas::FieldSet & augStateFlds);

/// TO BE RETIRED AFTER HpToHexner.cc is removed from saber (old B)
/// \details This is the adjoint of the calculation of hydrostatic exner increment fields
void eval_hydrostatic_exner_levels_ad(atlas::FieldSet & hatFlds,
                                      const atlas::FieldSet & augStateFlds);

/// TO BE RETIRED AFTER HpToHexner.cc is removed from saber (old B)
/// \details This is the inverse of eval_hydrostatic_exner_levels_tl
void eval_hydrostatic_exner_levels_tl_inv(atlas::FieldSet & incFlds,
                                          const atlas::FieldSet & augStateFlds);

}  // namespace mo
