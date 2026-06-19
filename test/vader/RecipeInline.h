/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#define ECKIT_TESTING_SELF_REGISTER_CASES 0

#include "atlas/field.h"
#include "atlas/functionspace.h"
#include "atlas/grid.h"

#include "eckit/config/LocalConfiguration.h"
#include "eckit/mpi/Comm.h"

#include "oops/mpi/mpi.h"
#include "oops/runs/Test.h"
#include "oops/util/FieldSetOperations.h"
#include "oops/util/Logger.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "test/TestEnvironment.h"

#include "vader/RecipeBase.h"
#include "Utils.h"

namespace vader {
namespace test {

/// \brief Top-level options taken by the inline (yaml-data) Recipe test.
///
/// Unlike the file-based Recipe test, all ingredient values and the expected
/// product are given directly in the yaml as a single vertical column per
/// field. The test grid is hard-coded and intentionally absent from the yaml:
/// since recipes never couple horizontally, the same column is duplicated to
/// every node, so the yaml author only ever needs to provide one column.
class RecipeInlineTestParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(RecipeInlineTestParameters, Parameters)

 public:
  oops::RequiredParameter<vader::RecipeParametersWrapper> recipe{
        "recipe", "recipe parameters", this};
  oops::RequiredParameter<eckit::LocalConfiguration> columns{
        "columns", "one vertical column of input values per ingredient", this};
  oops::RequiredParameter<eckit::LocalConfiguration> expected{
        "expected", "one vertical column of expected values for the product", this};
  oops::Parameter<double> nlTolerance{"nonlinear test tolerance",
        "nonlinear test tolerance", 1e-12, this};
  oops::Parameter<eckit::LocalConfiguration> modelData{
        "model data", eckit::LocalConfiguration(), this};
};

// -----------------------------------------------------------------------------
/// \brief Tests nonlinear recipe against inline yaml data: builds the
/// ingredient fields from the per-field columns in the yaml, runs executeNL,
/// and compares the computed product to the expected column from the yaml.
void testRecipeNonlinearInline() {
  RecipeInlineTestParameters params;
  params.validateAndDeserialize(::test::TestEnvironment::config());

  // create recipe
  const auto & recipeParams = params.recipe.value().recipeParams.value();
  std::unique_ptr<RecipeBase> recipe(RecipeFactory::create(
      recipeParams.name, recipeParams, params.modelData.value()));
  const oops::Variables ingredientVars = recipe->ingredients();
  const oops::Variable productVar = recipe->product();
  oops::Log::info() << "Testing non-linear vader recipe (inline data): "
                    << recipe->name() << std::endl;
  oops::Log::info() << " Ingredients: " << ingredientVars << std::endl;
  oops::Log::info() << " Product: " << productVar << std::endl;

  // Small grid hard-coded here. Its horizontal layout is irrelevant because
  // every node is filled with the same column (recipes never couple across
  // columns), so the yaml author never has to reason about node ordering.
  eckit::LocalConfiguration gridConfig;
  gridConfig.set("type", "regular_gaussian");
  gridConfig.set("N", 2);
  const atlas::StructuredGrid grid(gridConfig);
  const atlas::functionspace::StructuredColumns fs(grid);

  const eckit::LocalConfiguration columnsConfig = params.columns.value();
  const eckit::LocalConfiguration expectedConfig = params.expected.value();

  // Every multi-level column (each ingredient and the expected product) must
  // have at least minLevels levels. Three is the meaningful minimum for
  // recipes that use for_each_column, so that is the value chosen. Recipes that
  // use for_each_value may choose to use identical values across all levels, or
  // may choose to use the three levels to test different branches in the lambda.
  //
  // Columns of a single level are also allowed since, for example, surface variables
  // have only a single level.
  const size_t minLevels = 3;
  auto readColumn = [&](const eckit::LocalConfiguration & config,
                        const std::string & blockName,
                        const std::string & name) {
    if (!config.has(name)) {
      ABORT("Inline recipe test: '" + blockName + "' is missing the column '" +
            name + "' required by recipe.");
    }
    const std::vector<double> column = config.getDoubleVector(name);
    if (column.size() > 1 && column.size() < minLevels) {
      ABORT("Inline recipe test: column '" + name + "' in '" + blockName +
            "' has " + std::to_string(column.size()) + " level(s); use 1 for "
            "a field with a single level or at least " + std::to_string(minLevels) +
            " for a vertical column.");
    }
    return column;
  };

  // Flag any column in 'columns' that the recipe does not consume -- almost
  // always a misspelled ingredient name, caught here rather than silently
  // ignored.
  for (const std::string & key : columnsConfig.keys()) {
    if (!ingredientVars.has(key)) {
      ABORT("Inline recipe test: 'columns' has '" + key + "', which is not an "
            "ingredient of recipe. Check for a typo.");
    }
  }

  // Read each ingredient column from the yaml and duplicate it to all grid nodes
  // just to make a legitimate Field.
  atlas::FieldSet vader_computed;
  for (const auto & var : ingredientVars) {
    const std::string & name = var.name();
    addFieldFromColumn(vader_computed, name,
                       readColumn(columnsConfig, "columns", name), fs);
  }

  // Check number of levels in the product variable for this recipe.
  const size_t productLevels = recipe->productLevels(vader_computed);
  const std::vector<double> expectedColumn =
      readColumn(expectedConfig, "expected", productVar.name());
  if (expectedColumn.size() != productLevels) {
    ABORT("Inline recipe test: expected column '" + productVar.name() + "' has " +
          std::to_string(expectedColumn.size()) + " level(s), but recipe "
          "produces " + std::to_string(productLevels) + " level(s).");
  }

  // duplicate the expected product column the same way as the ingredients.
  atlas::FieldSet reference;
  addFieldFromColumn(reference, productVar.name(), expectedColumn, fs);

  // Allocate the product field (usually done in vader, but here the recipes
  // are tested outside of vader infrastructure) and run the NL recipe.
  addZeroField(vader_computed, productVar.name(), fs, productLevels);
  recipe->executeNL(vader_computed);

  // Compute relative norm of the difference between expected and computed.
  util::subtractFieldSets(vader_computed, reference);
  const double norm = util::normField(vader_computed[productVar.name()],
                                      oops::mpi::world());
  const double refNorm = util::normField(reference[productVar.name()],
                                         oops::mpi::world());
  oops::Log::info() << "Relative norm of (computed - expected): "
                    << norm / refNorm << std::endl;
  const double tol = params.nlTolerance;
  EXPECT(oops::is_close_absolute(norm / refNorm, 0.0, tol));
}

// -----------------------------------------------------------------------------

class RecipeInline : public oops::Test {
 public:
  RecipeInline() {}
  virtual ~RecipeInline() {}
 private:
  std::string testid() const override {return "vader::test::RecipeInline";}

  void register_tests() const override {
    std::vector<eckit::testing::Test>& ts = eckit::testing::specification();
    ts.emplace_back(CASE("vader/RecipeInline/testRecipeNonlinearInline")
      { testRecipeNonlinearInline(); });
  }

  void clear() const override {}
};

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace vader
