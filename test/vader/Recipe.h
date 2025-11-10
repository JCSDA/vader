/*
 * (C) Copyright 2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <netcdf.h>

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <boost/any.hpp>


#define ECKIT_TESTING_SELF_REGISTER_CASES 0

#include "atlas/field.h"
#include "atlas/functionspace.h"
#include "atlas/grid.h"
#include "atlas/meshgenerator.h"

#include "eckit/config/LocalConfiguration.h"
#include "eckit/mpi/Comm.h"

#include "oops/runs/Test.h"
#include "oops/util/FieldSetOperations.h"
#include "oops/util/Logger.h"
#include "oops/util/parameters/OptionalParameter.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "test/TestEnvironment.h"

#include "vader/RecipeBase.h"
#include "Utils.h"

namespace vader {
namespace test {

/// \brief Top-level options taken by the Recipe test.
class RecipeTestParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(RecipeTestParameters, Parameters)

 public:
  oops::RequiredParameter<vader::RecipeParametersWrapper> recipe{
        "recipe", "recipe parameters", this};
  oops::RequiredParameter<eckit::LocalConfiguration> grid{
        "trajectory grid", "trajectory grid description", this};
  oops::RequiredParameter<std::string> filename{"trajectory filename",
        "filename of existing netcdf file with trajectory ingredients", this};
  oops::Parameter<bool> runNLTest{"run nonlinear test", true, this};
  oops::Parameter<double> nlTolerance{"nonlinear test tolerance",
        "nonlinear test tolerance", 1e-12, this};
  oops::Parameter<bool> runADTest{"run adjoint test", true, this};
  oops::Parameter<double> adTolerance{"adjoint test tolerance",
        "adjoint test tolerance", 1e-12, this};
};

// -----------------------------------------------------------------------------
/// \brief Tests nonlinear recipe: computes norm of the difference between the
/// computed field and the field read from the file and compares to zero.
void testRecipeNonlinear() {
  RecipeTestParameters params;
  params.validateAndDeserialize(::test::TestEnvironment::config());

  if (!params.runNLTest) return;

  // create recipe
  const auto & recipeParams = params.recipe.value().recipeParams.value();
  std::unique_ptr<RecipeBase> recipe(RecipeFactory::create(
      recipeParams.name, recipeParams, eckit::LocalConfiguration()));
  const oops::Variables ingredientVars = recipe->ingredients();
  const oops::Variable productVar = recipe->product();
  oops::Log::info() << "Testing non-linear vader recipe: " << recipe->name()
                    << std::endl;
  oops::Log::info() << " Ingredients: " << ingredientVars << std::endl;
  oops::Log::info() << " Product: " << productVar << std::endl;

  // set up grid and functionspace based on description in the yaml
  const atlas::StructuredGrid grid(params.grid.value());
  const atlas::functionspace::StructuredColumns fs(grid);

  // Open NetCDF file and read all the ingredients
  atlas::FieldSet vader_computed;
  atlas::FieldSet reference;
  int ncid, retval;
  const std::string & filename = params.filename;
  oops::Log::info() << "Reading ingredients from file: " << filename << std::endl;
  if ((retval = nc_open(filename.c_str(), NC_NOWRITE, &ncid))) ERR(retval);
  std::vector<size_t> ingredientLevels(ingredientVars.size(), 0);
  for (size_t jvar = 0; jvar < ingredientVars.size(); ++jvar) {
    addFieldFromFile(vader_computed, ingredientVars[jvar].name(), fs, grid,
                     ingredientLevels[jvar], ncid);
  }
  oops::Log::info() << "Reading reference product from file: " << filename << std::endl;
  size_t productLevels = 0;
  addFieldFromFile(reference, productVar.name(), fs, grid, productLevels, ncid);

  // allocate field for the product (usually done in vader, but here the recipes are
  // tested outside of vader infrastructure)
  productLevels = recipe->productLevels(vader_computed);
  addZeroField(vader_computed, productVar.name(), fs, productLevels);
  // run NL recipe
  recipe->executeNL(vader_computed);

  // Debug prints
  const auto & field = vader_computed[productVar.name()];
  const auto & refField = reference[productVar.name()];
  const auto fieldView = atlas::array::make_view<const double, 2>(field);
  const auto refFieldView = atlas::array::make_view<const double, 2>(refField);
  oops::Log::debug() << "Comparing fields " << productVar << std::endl;
  oops::Log::debug() << "Level N : field norm ; reference norm ; difference norm"
                     << std::endl;
  const auto & comm = eckit::mpi::comm();
  for (size_t jlev = 0 ; jlev < field.shape(1); jlev++) {
    oops::Log::debug() << "Level " << jlev + 1 << " : ";
    double norm = 0;
    double refNorm = 0;
    double diffNorm = 0;
    for (size_t jnode = 0; jnode < field.shape(0); jnode++) {
      norm += std::pow(fieldView(jnode, jlev), 2);
      refNorm += std::pow(refFieldView(jnode, jlev), 2);
      diffNorm += std::pow(fieldView(jnode, jlev) - refFieldView(jnode, jlev), 2);
    }
    comm.allReduceInPlace(norm, eckit::mpi::sum());
    comm.allReduceInPlace(refNorm, eckit::mpi::sum());
    comm.allReduceInPlace(diffNorm, eckit::mpi::sum());
    oops::Log::debug() << std::sqrt(norm) << " ; "
                       << std::sqrt(refNorm) << " ; "
                       << std::sqrt(diffNorm) << std::endl;
  }

  // compute norm of the difference between the reference and the computed products
  util::subtractFieldSets(vader_computed, reference);
  const double norm = util::normField(vader_computed[productVar.name()], oops::mpi::world());
  const double refNorm = util::normField(reference[productVar.name()], oops::mpi::world());
  oops::Log::info() << "Norm of the difference between the reference and the computed field: "
                    << norm << std::endl;
  const double tol = params.nlTolerance;
  EXPECT(oops::is_close_absolute(norm / refNorm, 0.0, tol));
}


// -----------------------------------------------------------------------------
/// \brief Tests adjoint of the recipe.
void testRecipeAdjoint() {
  RecipeTestParameters params;
  params.validateAndDeserialize(::test::TestEnvironment::config());

  if (!params.runADTest) return;

  // create recipe
  const auto & recipeParams = params.recipe.value().recipeParams.value();
  std::unique_ptr<RecipeBase> recipe(RecipeFactory::create(
      recipeParams.name, recipeParams, eckit::LocalConfiguration()));
  const oops::Variables ingredientVars = recipe->ingredients();
  const oops::Variables trajectoryVars = recipe->trajectoryVars();
  const oops::Variable productVar = recipe->product();
  oops::Log::info() << "Testing vader recipe (K): " << recipe->name() << std::endl;
  oops::Log::info() << " Ingredients (dx): " << ingredientVars << std::endl;
  oops::Log::info() << " Trajectory Vars (x): " << trajectoryVars << std::endl;
  oops::Log::info() << " Product (dy): " << productVar << std::endl;

  // set up grid and functionspace based on description in the yaml
  const atlas::StructuredGrid grid(params.grid.value());
  const atlas::functionspace::StructuredColumns fs(grid);

  // Open NetCDF file and read all the fields for the trajectory
  atlas::FieldSet traj;
  int ncid, retval;
  const std::string & filename = params.filename;
  std::vector<size_t> trajLevels(trajectoryVars.size(), 0);
  oops::Log::info() << "Reading trajectory from file: " << filename << std::endl;
  if ((retval = nc_open(filename.c_str(), NC_NOWRITE, &ncid))) ERR(retval);
  for (size_t jvar = 0; jvar < trajectoryVars.size(); ++jvar) {
    addFieldFromFile(traj, trajectoryVars[jvar].name(), fs, grid,
                     trajLevels[jvar], ncid);
  }

  // We don't need to read ingredient fields from the file, but we do need to get their # of levels
  atlas::FieldSet ingredientsNL;
  std::vector<size_t> ingredientLevels(ingredientVars.size(), 0);
  oops::Log::info() << "Getting ingredient levels from file: " << filename << std::endl;
  if ((retval = nc_open(filename.c_str(), NC_NOWRITE, &ncid))) ERR(retval);
  for (size_t jvar = 0; jvar < ingredientVars.size(); ++jvar) {
    addFieldFromFile(ingredientsNL, ingredientVars[jvar].name(), fs, grid,
                     ingredientLevels[jvar], ncid);
  }

  // allocate field for the product in the trajectory (usually done in vader,
  // but here the recipes are tested outside of vader infrastructure)
  const size_t productLevels = recipe->productLevels(ingredientsNL);

  // Testing whether (dx, K^T dy) == (K dx, dy)
  // Allocating dxin to contain randomized dx (for the ingredient variables)
  // and Kdx (for the product variable) that will be filled in recipe.executeTL.
  atlas::FieldSet dxin;
  for (size_t jvar = 0; jvar < ingredientVars.size(); ++jvar) {
    addRandomField(dxin, ingredientVars[jvar].name(), fs, ingredientLevels[jvar]);
  }
  addZeroField(dxin, productVar.name(), fs, productLevels);
  // fill the product variable with Kdx
  recipe->executeTL(dxin, traj);

  // Allocating dxout to contain randomized dy (for the product variable)
  // and K^Tdy (for the ingredients variables) that will be updated after
  // recipe.executeAD call.
  atlas::FieldSet dxout;
  addRandomField(dxout, productVar.name(), fs, productLevels);
  for (size_t jvar = 0; jvar < ingredientVars.size(); ++jvar) {
    addZeroField(dxout, ingredientVars[jvar].name(), fs, ingredientLevels[jvar]);
  }
  // after calling recipe->executeAD, dy (the product variable) will be
  // zeroed out, so the copy is needed.
  atlas::Field dy(productVar.name(), dxout.field(productVar.name()).datatype(),
                  dxout.field(productVar.name()).shape());
  auto from_view = atlas::array::make_view<double, 2>(dxout.field(productVar.name()));
  auto to_view = atlas::array::make_view<double, 2>(dy);
  to_view.assign(from_view);
  // fill the ingredients variables with K^T dxout
  recipe->executeAD(dxout, traj);

  double zz1 = 0;
  // Compute (dx, K^T dy)
  for (const auto & ingredientVar : ingredientVars) {
    zz1 += dotProduct(dxin[ingredientVar.name()], dxout[ingredientVar.name()]);
  }
  // Compute (Kdx, dy)
  double zz2 = dotProduct(dxin[productVar.name()], dy);

  oops::Log::info() << "<dx,KTdy>=" << zz1 << std::endl;
  oops::Log::info() << "<Kdx,dy>=" << zz2 << std::endl;
  oops::Log::info() << "<dx,KTdy>-<Kdx,dy>/<dx,KTdy>="
                    << (zz1-zz2)/zz1 << std::endl;
  oops::Log::info() << "<dx,KTdy>-<Kdx,dy>/<Kdx,dy>="
                    << (zz1-zz2)/zz2 << std::endl;
  const double tol = params.adTolerance;
  EXPECT(oops::is_close(zz1, zz2, tol));
}

// -----------------------------------------------------------------------------

class Recipe : public oops::Test {
 public:
  Recipe() {}
  virtual ~Recipe() {}
 private:
  std::string testid() const override {return "vader::test::Recipe";}

  void register_tests() const override {
    std::vector<eckit::testing::Test>& ts = eckit::testing::specification();
    ts.emplace_back(CASE("vader/Recipe/testRecipeNonlinear")
      { testRecipeNonlinear(); });
    ts.emplace_back(CASE("vader/Recipe/testRecipeAdjoint")
      { testRecipeAdjoint(); });
  }

  void clear() const override {}
};

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace vader
