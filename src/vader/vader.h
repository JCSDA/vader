/*
 * (C) Copyright 2021-2024 UCAR
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

#include "atlas/field/FieldSet.h"
#include "oops/base/Variable.h"
#include "oops/base/Variables.h"
#include "oops/util/Printable.h"
#include "RecipeBase.h"
#include "VaderParameters.h"

namespace vader {

typedef std::map<oops::Variable, std::vector<std::string>> cookbookConfigType;
// configCookbookKey is a key that can optionally be provided in the eckit::LocalConfiguration
// that can be passed as the second parameter of the Vader constructor. If passed, the value
// is used to define the Vader cookbook for the vader instance. If not passed, the default Vader
// cookbook defined in DefaultCookbook.h will be used.
static const char configCookbookKey[]  = "cookbook";

// configModelVarsKey is a key that can optionally be provided in the eckit::LocalConfiguration
// that can be passed as the second parameter of the Vader constructor. If passed, the values
// will be stored in Vader's configVariables_ property. Whenever a recipe is executed that
// requires a model-specific value in its algorithm, the recipe will look in that Configuration for
// the value, and should generate an error if it isn't found. (Instead of using a default value.)
static const char configModelVarsKey[] = "model data";

// ------------------------------------------------------------------------------------------------
/*! \brief Vader class to handle variable transformations
 *
 *  \details This class provides generic variable transformations via the
 *           changeVar method which is passed an atlas fieldset.
 *
 *           Throughout the Vader code, the primary variables are named using a
 *           metaphor involving ingredients, recipies, and a cookbook.
 *
 *           A 'recipe' is is an object that can produce a single output
 *           variable when provided with a list of required input variables.
 *           The input variables are referred to as the 'ingredients' to the
 *           recipe. The 'cookbook' is the container (a map) that
 *           contains the recipes to be attempted when specified output variable
 *           is desired. The cookbook can contain multiple recipes that produce
 *           the same output variable.
 */

class Vader  : public util::Printable {
 public:
    static const std::string classname() {return "Vader";}
    static const cookbookConfigType defaultCookbookDefinition;
    typedef  std::vector<std::pair<oops::Variable,
                                   const std::unique_ptr<RecipeBase> & >> vaderPlanType;
    Vader(const VaderParameters & parameters,
          const eckit::Configuration & config = eckit::LocalConfiguration());
    Vader(const Vader &) = delete;
    Vader& operator=(const Vader &) = delete;
    ~Vader();

    std::vector<std::string> getPlanNames() const;
    std::vector<std::string> getPlanNames(vaderPlanType plan) const;
    bool needsTLADInit() const {return !recipeExecutionPlanBuilt_;}
    oops::Variables initTLAD(oops::Variables &) const;
    oops::Variables initTLAD(oops::Variables &, vaderPlanType &) const;

    /// Calculates as many variables in the list as possible
    oops::Variables changeVar(atlas::FieldSet &, oops::Variables &,
                              vaderPlanType &) const;
    oops::Variables changeVar(atlas::FieldSet &, oops::Variables &) const;
    void changeVarTraj(atlas::FieldSet const &, oops::Variables const &);
    oops::Variables changeVarTL(atlas::FieldSet &) const;
    oops::Variables changeVarAD(atlas::FieldSet &) const;
    oops::Variables changeVarTL(atlas::FieldSet &, vaderPlanType &) const;
    oops::Variables changeVarAD(atlas::FieldSet &, vaderPlanType &) const;

 private:
    std::map<oops::Variable, std::vector<std::unique_ptr<RecipeBase>>> cookbook_;
    mutable vaderPlanType recipeExecutionPlan_;  // mutable due to being set in const changeVarTL/AD
    mutable oops::Variables toVariables_;  // mutable due to being set in const changeVarTL/AD
    mutable atlas::FieldSet trajectory_;  // mutable due to being set in const changeVarTL/AD
    bool changeVarTrajCalled_ = false;
    mutable bool recipeExecutionPlanBuilt_ = false;  // mutable due to being set in changeVarTL/AD
    const eckit::LocalConfiguration configVariables_;
    std::map<oops::Variable, std::vector<std::string>>
        getDefaultCookbookDef();

    void createCookbook(const cookbookConfigType &,
                        const std::vector<RecipeParametersWrapper> & allRecpParamWraps =
                              std::vector<RecipeParametersWrapper>());

    bool planVariable(oops::Variables &,
                      oops::Variables &,
                      const oops::Variable &,
                      oops::Variables &,
                      vaderPlanType &,
                      const bool,
                      oops::Variables &,
                      vaderPlanType &) const;

    void planVariables(oops::Variables &,
                       oops::Variables &,
                       vaderPlanType &) const;

    void planVariables(oops::Variables &,
                       oops::Variables &,
                       vaderPlanType &,
                       const bool,
                       oops::Variables &,
                       vaderPlanType &) const;

    void createLinearPlanAndSetTraj(oops::Variables &,
                                    oops::Variables &,
                                    vaderPlanType &) const;

    void executePlanNL(atlas::FieldSet &, const vaderPlanType &) const;
    void executePlanTL(atlas::FieldSet &, const vaderPlanType &) const;
    void executePlanAD(atlas::FieldSet &, const vaderPlanType &) const;
    void print(std::ostream &) const;
};

}  // namespace vader
