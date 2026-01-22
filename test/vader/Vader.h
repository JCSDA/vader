/*
 * (C) Copyright 2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <netcdf.h>

#include <string>
#include <vector>

#define ECKIT_TESTING_SELF_REGISTER_CASES 0

#include "atlas/field.h"
#include "atlas/functionspace.h"
#include "atlas/grid.h"
#include "atlas/meshgenerator.h"

#include "eckit/config/LocalConfiguration.h"

#include "oops/base/Variables.h"
#include "oops/runs/Test.h"
#include "oops/util/Logger.h"
#include "test/TestEnvironment.h"

#include "vader/vader.h"
#include "Utils.h"

namespace vader {
namespace test {

/// \brief Top-level options taken by the Vader test.
class VaderTestParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(VaderTestParameters, Parameters)

 public:
  oops::RequiredParameter<vader::VaderParameters> vader{
        "vader", "vader parameters", this};
  oops::RequiredParameter<eckit::LocalConfiguration> grid{
        "trajectory grid", "trajectory grid description", this};
  oops::RequiredParameter<std::string> filename{"trajectory filename",
        "filename of existing netcdf file with trajectory ingredients", this};
  oops::RequiredParameter<std::vector<std::string>> trajectoryVars{
        "trajectory variables",
        "trajectory variables required by recipe(s) being tested",
        this};
  oops::RequiredParameter<std::vector<std::string>> ingredients{
        "ingredients",
        "ingredient variables required by recipe(s) being tested",
        this};
  oops::RequiredParameter<std::vector<std::string>> products{"products",
        "target variables", this};
  oops::RequiredParameter<double> tolerance{"adjoint test tolerance",
        "adjoint test tolerance", this};
  oops::OptionalParameter<eckit::LocalConfiguration> cookbook{
    "cookbook", "custom cookbook to use for the test", this};
};

// -----------------------------------------------------------------------------
/// \brief Tests adjoint of the recipe.
void testVaderAdjoint() {
  VaderTestParameters params;
  params.validateAndDeserialize(::test::TestEnvironment::config());

  eckit::LocalConfiguration modelVarsConfig;
  modelVarsConfig.set("gas_constant_of_dry_air", 2.8705e2);
  eckit::LocalConfiguration vaderConfig;
  vaderConfig.set(vader::configModelVarsKey, modelVarsConfig);
  if (params.cookbook.value() != boost::none) {
    vaderConfig.set(vader::configCookbookKey, *params.cookbook.value());
  }

  vader::Vader vader(params.vader, vaderConfig);
  oops::Variables ingredientVars(params.ingredients);
  oops::Variables trajectoryVars(params.trajectoryVars);
  const oops::Variables productVars(params.products);
  oops::Log::info() << "Testing vader" << std::endl;
  oops::Log::info() << " Trajectory (x): " << trajectoryVars << std::endl;
  oops::Log::info() << " Ingredients (dx): " << ingredientVars << std::endl;
  oops::Log::info() << " Products (dy): " << productVars << std::endl;

  // set up grid and functionspace based on description in the yaml
  const atlas::StructuredGrid grid(params.grid.value());
  const atlas::functionspace::StructuredColumns fs(grid);

  // Open NetCDF file and read all the ingredients for the trajectory
  atlas::FieldSet traj;
  int ncid, retval;
  const std::string & filename = params.filename;
  oops::Log::info() << "Reading trajectory from file: " << filename << std::endl;
  if ((retval = nc_open(filename.c_str(), NC_NOWRITE, &ncid))) ERR(retval);
  std::vector<size_t> trajectoryVarsLevels(trajectoryVars.size(), 0);
  for (size_t jvar = 0; jvar < trajectoryVars.size(); ++jvar) {
    addFieldFromFile(traj, trajectoryVars[jvar].name(), fs, grid,
                     trajectoryVarsLevels[jvar], ncid);
  }
  oops::Log::info() << "Reading ingredients from file: " << filename << std::endl;
  // Also read ingredients from file, but only to populate ingredientVarsLevels (better way?)
  atlas::FieldSet tempFieldSet;
  std::vector<size_t> ingredientVarsLevels(ingredientVars.size(), 0);
  for (size_t jvar = 0; jvar < ingredientVars.size(); ++jvar) {
    addFieldFromFile(tempFieldSet, ingredientVars[jvar].name(), fs, grid,
                     ingredientVarsLevels[jvar], ncid);
  }
  if ((retval = nc_close(ncid))) ERR(retval);
  // run NL to set trajectory
  oops::Variables vars = productVars;
  // oops::Variables incrementVars(traj.field_names());
  vader.changeVarTraj(traj, vars);

  // Testing whether (dx, K^T dy) == (K dx, dy)
  // Allocating dxin to contain randomized dx (for the ingredient variables)
  atlas::FieldSet dxin;
  for (size_t ivar = 0; ivar < ingredientVars.size(); ++ivar) {
    addRandomField(dxin, ingredientVars[ivar].name(), fs, ingredientVarsLevels[ivar]);
  }
  // fill the product variable with Kdx
  oops::Variables varsProduced = vader.initTLAD(ingredientVars);
  vader.changeVarTL(dxin);

  // The test should be setup to do variable change for all variables, exit
  // if it's not
  vars -= varsProduced;
  if (vars.size() > 0) {
    oops::Log::info() << "Not all variables can be converted; no recipes found for "
                      << vars << std::endl;
  }
  EXPECT_EQUAL(vars.size(), 0);

  // Allocating dxout to contain randomized dy (for the product variable).
  // After calling recipe->executeAD, dy (the product variable) will be
  // zeroed out, so the copy is needed.
  atlas::FieldSet dxout, dy;
  for (const auto & productVar : productVars) {
    const std::string & name = productVar.name();
    addRandomField(dxout, name, fs, dxin.field(name).shape(1));
    atlas::Field field(name, dxout.field(name).datatype(),
                  dxout.field(name).shape());
    auto from_view = atlas::array::make_view<double, 2>(dxout.field(name));
    auto to_view = atlas::array::make_view<double, 2>(field);
    to_view.assign(from_view);
    dy.add(field);
  }
  // If an ingredient is not already in dxout, and is also not present in the trajectory, Vader
  // won't have enough information to create the field, so we must populate dxout with those
  // zeroed-out ingredient fields before calling changeVarAD (Problem?)
  for (size_t ivar = 0; ivar < ingredientVars.size(); ++ivar) {
    const std::string & name = ingredientVars[ivar].name();
    if (!(dxout.has(name) || traj.has(name))) {
      addZeroField(dxout, name, fs, ingredientVarsLevels[ivar]);
    }
  }

  // fill the ingredients variables with K^T dxout
  vader.changeVarAD(dxout);

  double zz1 = 0;
  // Compute (dx, K^T dy)
  for (const auto & ingredientVar : ingredientVars) {
    zz1 += dotProduct(dxin[ingredientVar.name()], dxout[ingredientVar.name()]);
  }
  // Compute (Kdx, dy)
  double zz2 = 0;
  for (const auto & productVar : productVars) {
    zz2 += dotProduct(dxin[productVar.name()], dy[productVar.name()]);
  }
  oops::Log::info() << "<dx,KTdy>=" << zz1 << std::endl;
  oops::Log::info() << "<Kdx,dy>=" << zz2 << std::endl;
  oops::Log::info() << "<dx,KTdy>-<Kdx,dy>/<dx,KTdy>="
                    << (zz1-zz2)/zz1 << std::endl;
  oops::Log::info() << "<dx,KTdy>-<Kdx,dy>/<Kdx,dy>="
                    << (zz1-zz2)/zz2 << std::endl;
  const double tol = params.tolerance;
  EXPECT(oops::is_close(zz1, zz2, tol));
}

// -----------------------------------------------------------------------------

class Vader : public oops::Test {
 public:
  Vader() {}
  virtual ~Vader() {}
 private:
  std::string testid() const override {return "vader::test::Vader";}

  void register_tests() const override {
    std::vector<eckit::testing::Test>& ts = eckit::testing::specification();

    ts.emplace_back(CASE("vader/Vader/testVaderAdjoint")
      { testVaderAdjoint(); });
  }

  void clear() const override {}
};

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace vader
