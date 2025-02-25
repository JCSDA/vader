/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>
#include <vector>

#include "atlas/field/Field.h"
#include "atlas/field/FieldSet.h"

namespace mo {

/// USED IN RECONFIGURE STATE GAUSS
/// \details eval_mio_fields_nl returns the effective cloud fractions
///          for the moisture incrementing operator (MIO)
void eval_mio_fields_nl(const std::string & mio_file, atlas::FieldSet & fields);

}  // namespace mo
