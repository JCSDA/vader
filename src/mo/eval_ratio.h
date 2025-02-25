/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"

namespace mo {

/// Helper functions for calculating specific quantities
void eval_ratio_to_second(atlas::FieldSet & fields,
                          const std::vector<std::string> & vars);

/// Helper functions for calculating specific quantities
void eval_ratio_to_second_tl(atlas::FieldSet & incFields,
                             const atlas::FieldSet & fields,
                             const std::vector<std::string> & vars);

/// Helper functions for calculating specific quantities
void eval_ratio_to_second_ad(atlas::FieldSet & hatFields,
                             const atlas::FieldSet & fields,
                             const std::vector<std::string> & vars);

}  // namespace mo

