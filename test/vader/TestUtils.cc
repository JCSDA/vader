/*
 * (C) Copyright 2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "Utils.h"

#include <netcdf.h>

#include <string>
#include <vector>

#include "atlas/field.h"
#include "atlas/functionspace.h"
#include "atlas/grid.h"

#include "oops/util/Random.h"

namespace vader {
namespace test {

void addFieldFromFile(atlas::FieldSet & fieldset,
                      const std::string & name,
                      const atlas::FunctionSpace & fs,
                      const atlas::StructuredGrid & grid,
                      size_t & levels, int ncid) {
  int var_id, dim_id, retval;
  if ((retval = nc_inq_varid(ncid, name.c_str(), &var_id))) ERR(retval);

  // Get sizes (nx and ny from the grid definition; nz from the file)
  atlas::idx_t nx = grid.nxmax();
  atlas::idx_t ny = grid.ny();

  const std::string nz_field("nz_" + name);
  if ((retval = nc_inq_dimid(ncid, "nz", &dim_id))) {
    if ((retval = nc_inq_dimid(ncid, nz_field.c_str(), &dim_id))) ERR(retval);
  }
  if ((retval = nc_inq_dimlen(ncid, dim_id, &levels))) ERR(retval);

  // Read data
  double zvar[levels][ny][nx];
  if ((retval = nc_get_var_double(ncid, var_id, &zvar[0][0][0]))) ERR(retval);

  // Copy data
  atlas::Field field = fs.createField<double>(
        atlas::option::name(name) | atlas::option::levels(levels));
  auto varView = atlas::array::make_view<double, 2>(field);
  for (size_t k = 0; k < levels; ++k) {
    for (atlas::idx_t j = 0; j < ny; ++j) {
      for (atlas::idx_t i = 0; i < grid.nx(ny-1-j); ++i) {
        atlas::gidx_t gidx = grid.index(i, ny-1-j);
        varView(gidx, k) = zvar[k][j][i];
      }
    }
  }
  fieldset.add(field);
}

void addRandomField(atlas::FieldSet & fieldset,
                    const std::string & name,
                    const atlas::FunctionSpace & fs,
                    size_t levels) {
  atlas::Field field = fs.createField<double>(
        atlas::option::name(name) | atlas::option::levels(levels));
  size_t n = field.shape(0)*field.shape(1);
  util::NormalDistribution<double> rand_vec(n, 0.0, 1.0, 1);
  auto view = atlas::array::make_view<double, 2>(field);
  for (atlas::idx_t jnode = 0; jnode < field.shape(0); ++jnode) {
    for (atlas::idx_t jlevel = 0; jlevel < field.shape(1); ++jlevel) {
      view(jnode, jlevel) = rand_vec[jnode*field.shape(1)+jlevel];
    }
  }
  fieldset.add(field);
}

void addZeroField(atlas::FieldSet & fieldset,
                  const std::string & name,
                  const atlas::FunctionSpace & fs,
                  size_t levels) {
  atlas::Field field = fs.createField<double>(
        atlas::option::name(name) | atlas::option::levels(levels));
  size_t n = field.shape(0)*field.shape(1);
  util::NormalDistribution<double> rand_vec(n, 0.0, 1.0, 1);
  auto view = atlas::array::make_view<double, 2>(field);
  for (atlas::idx_t jnode = 0; jnode < field.shape(0); ++jnode) {
    for (atlas::idx_t jlevel = 0; jlevel < field.shape(1); ++jlevel) {
      view(jnode, jlevel) = 0.0;
    }
  }
  fieldset.add(field);
}

void addFieldFromColumn(atlas::FieldSet & fieldset,
                        const std::string & name,
                        const std::vector<double> & column,
                        const atlas::FunctionSpace & fs) {
  const size_t levels = column.size();
  atlas::Field field = fs.createField<double>(
        atlas::option::name(name) | atlas::option::levels(levels));
  auto view = atlas::array::make_view<double, 2>(field);
  for (atlas::idx_t jnode = 0; jnode < field.shape(0); ++jnode) {
    for (size_t k = 0; k < levels; ++k) {
      view(jnode, k) = column[k];
    }
  }
  fieldset.add(field);
}

double dotProduct(const atlas::Field & f1, const atlas::Field & f2) {
  double zz = 0.0;
  auto view1 = atlas::array::make_view<double, 2>(f1);
  auto view2 = atlas::array::make_view<double, 2>(f2);
  for (atlas::idx_t jnode = 0; jnode < f1.shape(0); ++jnode) {
    for (atlas::idx_t jlevel = 0; jlevel < f1.shape(1); ++jlevel) {
      zz += view1(jnode, jlevel)*view2(jnode, jlevel);
    }
  }
  return zz;
}

}  // namespace test
}  // namespace vader
