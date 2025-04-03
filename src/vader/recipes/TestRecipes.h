/*
 * (C) Copyright 2021-2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/PointCloud.h"
#include "atlas/util/Point.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class TestRecipeParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(TestRecipeParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief Test_VarA_from_B class defines a recipe for TestVarA
 *
 *  \details This instantiation of RecipeBase produces TestVarA
 *           using TestVarB as input. Non-linear recipe only.
 */
class Test_VarA_from_B : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef TestRecipeParameters Parameters_;

    Test_VarA_from_B(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override {return oops::Variable{"TestVarA"};}
    oops::Variables ingredients() const override {
      return oops::Variables{std::vector<std::string>{"TestVarB"}};
    }
    oops::Variables trajectoryVars() const override {
      return oops::Variables{std::vector<std::string>{"TestVarB"}};
    }
    size_t productLevels(const atlas::FieldSet &) const override {return 1;}
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override
                                {return atlas::functionspace::PointCloud({
                                    atlas::PointXY(0.0, 0.0)});}
    // bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    // void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    // void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};

// ------------------------------------------------------------------------------------------------
/*! \brief Test_VarA_from_C_D class defines a recipe for TestVarA
 *
 *  \details This instantiation of RecipeBase produces TestVarA
 *           using TestVarC and TestVarD as input. Non-linear recipe only.
 */
class Test_VarA_from_C_D : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef TestRecipeParameters Parameters_;

    Test_VarA_from_C_D(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override {return oops::Variable{"TestVarA"};}
    oops::Variables ingredients() const override {
      return oops::Variables{std::vector<std::string>{"TestVarC", "TestVarD"}};
    }
    size_t productLevels(const atlas::FieldSet &) const override {return 1;}
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override
                                {return atlas::functionspace::PointCloud({
                                    atlas::PointXY(0.0, 0.0)});}
    // bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    // void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    // void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};

// ------------------------------------------------------------------------------------------------
/*! \brief Test_VarB_from_E class defines a recipe for TestVarB
 *
 *  \details This instantiation of RecipeBase produces TestVarB
 *           using TestVarE as input. Non-linear recipe only.
 */
class Test_VarB_from_E : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef TestRecipeParameters Parameters_;

    Test_VarB_from_E(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override {return oops::Variable{"TestVarB"};}
    oops::Variables ingredients() const override {
      return oops::Variables{std::vector<std::string>{"TestVarE"}};
    }
    size_t productLevels(const atlas::FieldSet &) const override {return 1;}
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override
                                {return atlas::functionspace::PointCloud({
                                    atlas::PointXY(0.0, 0.0)});}
    // bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    // void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    // void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};
// ------------------------------------------------------------------------------------------------
/*! \brief Test_VarB_from_A class defines a recipe for TestVarB
 *
 *  \details This instantiation of RecipeBase produces TestVarB
 *           using TestVarA as input. Non-linear recipe only.
 *           This recipe combines with recipe Test_VarA_from_B to make 
 *           sure the Vader algorithm does not end up in an infinte
 *           recursion loop when attempting to create ingredients.
 */
class Test_VarB_from_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef TestRecipeParameters Parameters_;

    Test_VarB_from_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override {return oops::Variable{"TestVarB"};}
    oops::Variables ingredients() const override {
      return oops::Variables{std::vector<std::string>{"TestVarA"}};
    }
    oops::Variables trajectoryVars() const override {
      return oops::Variables{std::vector<std::string>{"TestVarA"}};
    }
    size_t productLevels(const atlas::FieldSet &) const override {return 1;}
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override
                                {return atlas::functionspace::PointCloud({
                                    atlas::PointXY(0.0, 0.0)});}
    bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};
// ------------------------------------------------------------------------------------------------
/*! \brief Test_VarC_from_F class defines a recipe for TestVarC
 *
 *  \details This instantiation of RecipeBase produces TestVarC
 *           using TestVarF as input. Non-linear recipe only.
 */
class Test_VarC_from_F : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef TestRecipeParameters Parameters_;

    Test_VarC_from_F(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override {return oops::Variable{"TestVarC"};}
    oops::Variables ingredients() const override {
      return oops::Variables{std::vector<std::string>{"TestVarF"}};
    }
    size_t productLevels(const atlas::FieldSet &) const override {return 1;}
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override
                                {return atlas::functionspace::PointCloud({
                                    atlas::PointXY(0.0, 0.0)});}
    // bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    // void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    // void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};
// ------------------------------------------------------------------------------------------------
/*! \brief Test_VarA_from_G class defines a recipe for TestVarC
 *
 *  \details This instantiation of RecipeBase produces TestVarA
 *           using TestVarG as input. Linear recipe only.
 */
class Test_VarA_from_G : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef TestRecipeParameters Parameters_;

    Test_VarA_from_G(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override {return oops::Variable{"TestVarA"};}
    oops::Variables ingredients() const override {
      return oops::Variables{std::vector<std::string>{"TestVarG"}};
    }
    oops::Variables trajectoryVars() const override {
      return oops::Variables{std::vector<std::string>{"TestVarG"}};
    }
    size_t productLevels(const atlas::FieldSet &) const override {return 1;}
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override
                                {return atlas::functionspace::PointCloud({
                                    atlas::PointXY(0.0, 0.0)});}
    bool hasTLAD() const override { return true; }
    bool hasNL() const override { return false; }
    void executeNL(atlas::FieldSet &) override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};

}  // namespace vader
