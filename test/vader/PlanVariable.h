/*
 * (C) Copyright 2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define ECKIT_TESTING_SELF_REGISTER_CASES 0

#include "atlas/field/Field.h"
#include "atlas/field/FieldSet.h"
#include "oops/runs/Test.h"
#include "oops/util/Logger.h"
#include "test/TestEnvironment.h"

#include "vader/vader.h"

/// \brief Parameters for each of the planVariable tests.
class PlanVarTestParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(PlanVarTestParameters, Parameters)

 public:
  oops::RequiredParameter<std::map<std::string, std::vector<std::string>>> cookbook{
    "cookbook", "Vader cookbook to use for test", this};
  oops::RequiredParameter<std::vector<std::string>> ingredients{
    "ingredients", "list of all available ingredients", this};
  oops::RequiredParameter<std::vector<std::string>> products{
    "products", "list of all products to plan", this};
  oops::RequiredParameter<bool> planTLAD{"plan for TLAD",
    "flag setting whether to check for TL/AD existence", this};
  oops::RequiredParameter<std::vector<std::string>> expectedPlanned{
    "expected planned products",
    "list of all products that vader should plan", this};
  oops::RequiredParameter<std::vector<std::string>> expectedPlan{
    "expected recipe plan",
    "list of all recipes used to plan all the planned products",
    this};
};

/// \brief Top-level options taken by the PlanVariable test.
class PlanVariableTestParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(PlanVariableTestParameters, Parameters)

 public:
  oops::RequiredParameter<vader::VaderParameters> vader{"vader",
        "vader parameters", this};
  oops::RequiredParameter<std::vector<PlanVarTestParameters>> tests{
        "plan variable tests", "list of test parameters for plan variable",
        this};
};

namespace vader {
namespace test {

// -----------------------------------------------------------------------------
/// \brief Tests planVariable method of vader.
void testPlanVariable() {
  PlanVariableTestParameters params;
  params.validateAndDeserialize(::test::TestEnvironment::config());
  // Perhaps output the list of all available recipes to log

  // loop through all tests
  for (const auto & param : params.tests.value()) {
    vader::Vader vader(params.vader, param.toConfiguration());
    const std::vector<std::string> targetVars = param.products;
    const std::vector<std::string> ingredientVars = param.ingredients;
    bool planTLAD = param.planTLAD;
    if (planTLAD) {
      oops::Log::info() << "Testing plan to do TL/AD from: " << std::endl;
    } else {
      oops::Log::info() << "Testing plan to do only NL from: " << std::endl;
    }
    oops::Log::info() << "  ingredients: " << ingredientVars << std::endl;
    oops::Log::info() << "  to products: " << targetVars << std::endl;
    // Create FieldSet with ingredient vars
    atlas::FieldSet fs;
    for (const auto var : ingredientVars) {
        fs.add(atlas::Field(var, atlas::array::DataType::int32(),
                                 atlas::array::ArrayShape({1})));
    }
    // Find which variables were planned, and test that they are as expected
    oops::Variables vaderVars(targetVars);
    oops::Variables plannedVars;
    std::vector<std::string> plannedRecipeNames;
    if (planTLAD) {
        plannedVars = vader.changeVarTraj(fs, vaderVars);
        plannedRecipeNames = vader.getPlanNames();
    } else {
        Vader::vaderPlanType plan;
        plannedVars = vader.changeVar(fs, vaderVars, plan);
        plannedRecipeNames = vader.getPlanNames(plan);
    }
    const oops::Variables expectedPlannedVars(param.expectedPlanned.value());
    oops::Log::info() << "Variables planned: " << plannedVars << std::endl;
    EXPECT(expectedPlannedVars == plannedVars);
    // Test that the plan is as expected
    const std::vector<std::string> expectedPlan = param.expectedPlan.value();
    for (size_t ii = 0; ii < plannedRecipeNames.size(); ++ii) {
      oops::Log::info() << "Planned recipe #" << ii+1 << ": "
                        << plannedRecipeNames[ii] << std::endl;
      EXPECT(plannedRecipeNames[ii] == expectedPlan[ii]);
    }
    oops::Log::info() << std::endl;
  }
}

// -----------------------------------------------------------------------------

class PlanVariable : public oops::Test {
 public:
  PlanVariable() {}
  virtual ~PlanVariable() {}
 private:
  std::string testid() const override {return "vader::test::PlanVariable";}

  void register_tests() const override {
    std::vector<eckit::testing::Test>& ts = eckit::testing::specification();

    ts.emplace_back(CASE("vader/PlanVariable/testPlanVariable")
      { testPlanVariable(); });
  }

  void clear() const override {}
};

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace vader
