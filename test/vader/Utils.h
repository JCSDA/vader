/*
 * (C) Copyright 2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>
#include <vector>

#include "atlas/field.h"
#include "atlas/functionspace.h"
#include "atlas/grid.h"

#include "oops/util/abor1_cpp.h"

#define ERR(e) {ABORT(nc_strerror(e));}

namespace vader {
namespace test {

/// \brief Helper function adding a new field to fieldset, and
///        initializing it with values read from netcdf file. Initializes
///        levels with the number of levels for this variable in the file.
void addFieldFromFile(atlas::FieldSet & fieldset,
                      const std::string & name,
                      const atlas::FunctionSpace & fs,
                      const atlas::StructuredGrid & grid,
                      size_t & levels, int ncid);

/// \brief Helper function adding a new field to fieldset, and
///        initializing it with random perturbations.
void addRandomField(atlas::FieldSet & fieldset,
                    const std::string & name,
                    const atlas::FunctionSpace & fs,
                    size_t levels);

/// \brief Helper function adding a new field to fieldset, and
///        initializing it with zeroes.
void addZeroField(atlas::FieldSet & fieldset,
                    const std::string & name,
                    const atlas::FunctionSpace & fs,
                    size_t levels);

/// \brief Helper function adding a new field to fieldset, initializing every
///        horizontal node with the same vertical column of values (typically
///        read from the test yaml), in order to create a legitimate Field
///        while only requiring the test author to provide one column of values.
void addFieldFromColumn(atlas::FieldSet & fieldset,
                        const std::string & name,
                        const std::vector<double> & column,
                        const atlas::FunctionSpace & fs);

/// \brief Helper function returning dot product between two fields
double dotProduct(const atlas::Field & f1, const atlas::Field & f2);

}  // namespace test
}  // namespace vader
