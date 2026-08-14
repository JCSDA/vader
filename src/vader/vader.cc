/*
 * (C) Copyright 2021-2024  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "atlas/option/Options.h"
#include "oops/util/FieldSetHelpers.h"
#include "oops/util/Logger.h"
#include "oops/util/Timer.h"
#include "vader/DefaultCookbook.h"
#include "vader/vader.h"

namespace vader {

// ------------------------------------------------------------------------------------------------
Vader::~Vader() {
    oops::Log::trace() << "Vader::~Vader done" << std::endl;
}
// ------------------------------------------------------------------------------------------------
void Vader::createCookbook(const cookbookConfigType & cookbook,
                           const std::vector<RecipeParametersWrapper> & allRecpParamWraps) {
    oops::Log::trace() << "entering Vader::createCookbook" << std::endl;
    std::vector<std::unique_ptr<RecipeBase>> recipes;
    for (const auto & defEntry : cookbook) {
        recipes.clear();
        for (const auto & recipeName : defEntry.second) {
            // There might not be any recipe parameters at all.
            // There might not be parameters for THIS recipe.
            // We must prepare for all eventualities.
            bool parametersFound = false;
            for (const auto & singleRecpParamWrap : allRecpParamWraps) {
                if (singleRecpParamWrap.recipeParams.value().name.value() == recipeName) {
                    recipes.push_back(std::unique_ptr<RecipeBase>
                        (RecipeFactory::create(recipeName,
                                               singleRecpParamWrap.recipeParams,
                                               configVariables_)));
                    parametersFound = true;
                    break;
                }
            }
            if (!parametersFound) {
                auto emptyRecipeParams = RecipeFactory::createParameters(recipeName);
                recipes.push_back(std::unique_ptr<RecipeBase>
                                  (RecipeFactory::create(recipeName, *emptyRecipeParams,
                                                         configVariables_)));
            }
        }
        cookbook_[defEntry.first] = std::move(recipes);
    }
    oops::Log::trace() << "leaving Vader::createCookbook" << std::endl;
}
// ------------------------------------------------------------------------------------------------
Vader::Vader(const VaderParameters & parameters, const eckit::Configuration & config) :
             configVariables_(config.getSubConfiguration(configModelVarsKey)) {
    util::Timer timer(classname(), "Vader");
    oops::Log::trace() << "entering Vader::Vader(parameters, constructConfig) " << std::endl;

    // Vader is designed to function without parameters. So VaderParameters
    // should not have any RequiredParameters.
    //
    // To simplify things for vader clients, they should declare vader Parameters with a
    // default construction of empty/default VaderParameters. i.e. their Parameters should contain:
    // oops::Parameter<vader::VaderParameters> vader{"vader", {}, this};
    //
    cookbookConfigType cookbookDefinition;
    if (config.has(configCookbookKey)) {
        // Convert eckit::Configuration to cookbookConfigType (map)
        auto configCookbookDefinition = config.getSubConfiguration(configCookbookKey);
        for (const auto & configCookbookKey : configCookbookDefinition.keys()) {
            cookbookDefinition[oops::Variable(configCookbookKey)] =
                                        configCookbookDefinition.getStringVector(configCookbookKey);
        }
    } else {
        cookbookDefinition = Vader::defaultCookbookDefinition;
    }
    if (parameters.recipeParams.value() == boost::none) {
        createCookbook(cookbookDefinition);
    } else {
        createCookbook(cookbookDefinition, *parameters.recipeParams.value());
    }
    oops::Log::trace() << "leaving Vader::Vader(parameters, constructConfig) " << std::endl;
}
// ------------------------------------------------------------------------------------------------
std::vector<std::string> Vader::getPlanNames() const {
    return getPlanNames(recipeExecutionPlan_);
}
// ------------------------------------------------------------------------------------------------
/*! \brief Get Plan Names
*
* \details **getPlanNames** provides a list of the names, in order, of the recipes in
* plan. (This is primarily useful for testing the Vader algorithm.)
*
* \param[in] plan A populated variable of type vaderPlanType
* \returns Ordered list of the names of the recipes in plan.
*/
std::vector<std::string> Vader::getPlanNames(const vaderPlanType plan) const {
    std::vector<std::string> recipeNames;
    for (const auto & varPlan : plan)
    {
        recipeNames.push_back(varPlan.second->name());
    }
    return recipeNames;
}
// ------------------------------------------------------------------------------------------------
/*! \brief Change Variable
*
* \details **changeVar** This is the signature of changeVar that will typically be called
* by vader clients, since they normally do not do anything with the plan.
*/
oops::Variables Vader::changeVar(atlas::FieldSet & afieldset,
                                 oops::Variables & neededVars) const {
  vaderPlanType plan;
  return changeVar(afieldset, neededVars, plan);
}
// ------------------------------------------------------------------------------------------------
/*! \brief Change Variable
*
* \details **changeVar** is called externally to invoke Vader's non-linear variable change
* functionality. The caller passes an Atlas FieldSet that contains only fields that have already
* been populated with values. These fields serve as the initial ingredients for recipes. The names
* of the desired output variables are passed via the neededVars parameter. After this method is
* complete, Vader will have popluated all the desired variables it can based on
* the ingredients it was given and the recipes in its cookbook. The names of the
* variables Vader created are returned to the caller in two different ways:
* * they are removed from neededVars
* * they are put into the method return object.
* Note that Vader may create some variables not listed in neededVars if they are required
* intermediate ingredients for recipes in order to produce one or more of the neededVars.
* Any variable names remaining in neededVars could not be created by Vader.
*
* \param[in,out] afieldset This is the FieldSet described above
* \param[in,out] neededVars Variables to try to produce
* \param[out] plan Can return the recipe plan for testing validation.
* \returns List of variables VADER was able to produce
*
*/
oops::Variables Vader::changeVar(atlas::FieldSet & afieldset,
                                 oops::Variables & neededVars,
                                 vaderPlanType & plan) const {
    util::Timer timer(classname(), "changeVar");
    oops::Log::trace() << "entering Vader::changeVar " << std::endl;
    oops::Log::info() << "Variables requested from Vader (non-linear): " << neededVars
        << std::endl;

    // onlyIngredientVars are the variables that Vader should NOT later return in varsProduced
    // (i.e. variables both in afieldset AND neededVars will be returned in varsProduced)
    oops::Variables onlyIngredientVars(afieldset.field_names());
    onlyIngredientVars -= neededVars;

    oops::Variables ingredientVars(afieldset.field_names());
    ASSERT_MSG(plan.empty(), "plan must be empty before calling planVariables");
    planVariables(ingredientVars, neededVars, plan);
    executePlanNL(afieldset, plan);

    if (neededVars.size()) {
        oops::Log::info() << "Requested variables Vader could not produce: "
            << neededVars << std::endl;
    } else {
        oops::Log::info() << "Vader produced all requested variables."  << std::endl;
    }

    oops::Variables varsProduced;
    for (const auto & field : afieldset) {
        varsProduced.push_back({field.name()});
    }

    varsProduced -= onlyIngredientVars;
    oops::Log::trace() << "leaving Vader::changeVar" << std::endl;
    return varsProduced;
}
// ------------------------------------------------------------------------------------------------
/*! \brief Change Variable-Trajectory
*
* \details **changeVarTraj** is called externally as a prerequisite to calling changeVarTL or
* changeVarAD. The information passed is stored in the Vader instance, but is not used until
* changeVarTL or changeVarAD is called. The FieldSet passed is used for the trajectory
* variables that are required by the recipes in the recipeExecutionPlan_, and the neededVars passed
* are used as the "to" variables when creating the recipeExecutionPlan_ if changeVarAD is called
* before changeVarTL.
*
* \param[in] trajFieldset This contains trajectory and/or background/first guess fields.
* \param[in] neededVars Increment (dy) variables that will be produced by changeVarTL
*
*/
void Vader::changeVarTraj(atlas::FieldSet const & trajFieldset,
                          oops::Variables const & neededVars) {
    util::Timer timer(classname(), "changeVarTraj");
    oops::Log::trace() << "entering Vader::changeVarTraj " << std::endl;
    oops::Log::info() << "Increment variables passed to Vader::changeVarTraj: " << neededVars
        << std::endl;

    recipeExecutionPlanBuilt_ = false;
    recipeExecutionPlan_.clear();
    toVariables_ = neededVars;
    trajectory_ = util::copyFieldSet(trajFieldset);
    changeVarTrajCalled_ = true;
    oops::Log::trace() << "leaving Vader::changeVarTraj " << std::endl;
}
// ------------------------------------------------------------------------------------------------
/*! \brief Initialize TL/AD
*
* \details **initTLAD** This is the overload of this function that will typically be called
* by vader clients, since they normally do not do anything with the execution plans.
*/
oops::Variables Vader::initTLAD(oops::Variables & ingredientVars) const {
    vaderPlanType trajRecipeExecutionPlan;
    return initTLAD(ingredientVars, trajRecipeExecutionPlan);
}
// ------------------------------------------------------------------------------------------------
/*! \brief Initialize TL/AD
*
* \details **initTLAD** is called by the vader client. It populates a linear recipeExecutionPlan_
* for the Vader instance, and also creates
* and executes a non-linear plan for the instance's trajectory fieldset, trajectory_.
* The recipeExecutionPlan_ produces a plan of recipes that will produce
* as many 'neededVars' as possible given the ingredientVars passed.
* In order for a recipe to be added to the recipeExecutionPlan_:
* 1. It must have TL/AD methods implemented.
* 2. All of its ingredients must be available in the ingredientVars, or can be produced by
*    from the ingredientVars using TL/AD recipes.
* 3. All of its trajectory variables must be available in the trajectory_, or can be produced by
*    from the trajectory_ fields using non-linear recipes.
*
* \param[in,out] ingredientvars The "from" linear ingredient variables that will be present
* \param[out] trajRecipeExecutionPlan Can return the trajectory recipe plan for testing validation.
* \returns The subset of toVariables_ that were produced by the recipe plan.
*/
oops::Variables Vader::initTLAD(oops::Variables & ingredientVars,
                                vaderPlanType & trajRecipeExecutionPlan) const {
    oops::Log::trace() << "entering Vader::initTLAD" << std::endl;
    ASSERT_MSG(changeVarTrajCalled_, "changeVarTraj must be called before calling initTLAD");
    ASSERT_MSG(!recipeExecutionPlanBuilt_, "initTLAD called when Recipe execution"
        " plan already built.");
    trajRecipeExecutionPlan.clear();
    const bool planTLAD = true;
    oops::Variables trajectoryVars(trajectory_.field_names());
    oops::Variables neededVars(toVariables_);
    oops::Variables neededVarsProduced(toVariables_);
    ASSERT_MSG(recipeExecutionPlan_.empty(),
        "recipeExecutionPlan_ must be empty before calling planVariables");
    ASSERT_MSG(trajRecipeExecutionPlan.empty(),
        "trajRecipeExecutionPlan must be empty before calling planVariables");

    planVariables(ingredientVars, neededVars, recipeExecutionPlan_, planTLAD,
                  trajectoryVars, trajRecipeExecutionPlan);
    recipeExecutionPlanBuilt_ = true;
    neededVarsProduced -= neededVars;
    // Update trajectory_ to add any required fields via the trajectory recipe plan
    oops::Log::debug() << "Executing trajRecipeExecutionPlan." << std::endl;
    executePlanNL(trajectory_, trajRecipeExecutionPlan);
    oops::Log::trace() << "leaving Vader::initTLAD" << std::endl;
    return neededVarsProduced;
}
// ------------------------------------------------------------------------------------------------
/*! \brief Change Variable-Tangent Linear
*
* \details **changeVarTL** This is the signature of changeVarTL that will typically be called
* by vader clients, since they normally do not do anything with the execution plans.
*/
oops::Variables Vader::changeVarTL(atlas::FieldSet & afieldset) const
{
    vaderPlanType trajPlan;
    return changeVarTL(afieldset, trajPlan);
}
// ------------------------------------------------------------------------------------------------
/*! \brief Change Variable Tangent Linear
*
* \details **changeVarTL** is called externally to perform the tangent linear (TL) variable change.
* If the recipeExecutionPlan_ has not been created by a prior call to changeVarAD, then
* it will be created first.
* \param[in,out] afieldset This is the input/output fieldset. On input, it should contain the
*                only populated increment fields.
* \param[in,out] neededVars Variables to try to produce
* \param[in,out] trajRecipeExecutionPlan Can return the trajectory recipe plan for testing
                 validation.
* \returns List of variables VADER was able to produce
*/
oops::Variables Vader::changeVarTL(atlas::FieldSet & afieldset,
                                   vaderPlanType & trajRecipeExecutionPlan) const {
    oops::Log::trace() << "entering Vader::changeVarTL" << std::endl;
    oops::Variables varsPopulated;
    ASSERT_MSG(recipeExecutionPlanBuilt_, "initTLAD must be called before Vader::changeVarTL");
    executePlanTL(afieldset, recipeExecutionPlan_);
    for (const auto & varplan : recipeExecutionPlan_) {
        varsPopulated.push_back(varplan.first);
    }
    oops::Log::trace() << "leaving Vader::changeVarTL" << std::endl;
    return varsPopulated;
}
// ------------------------------------------------------------------------------------------------
/*! \brief Change Variable-Adjoint
*
* \details **changeVarAD** This is the signature of changeVarAD that will typically be called
* by vader clients, since they normally do not do anything with the recipe execution plans.
*/
oops::Variables Vader::changeVarAD(atlas::FieldSet & afieldset) const {
  vaderPlanType trajPlan;
  return changeVarAD(afieldset, trajPlan);
}
// ------------------------------------------------------------------------------------------------
/*! \brief Change Variable Adjoint
*
* \details **changeVarAD** is called externally to perform the adjoint (AD) variable change.
* If the recipeExecutionPlan_ has not been created by a prior call to changeVarTL, then
* it will be created first. After the plan is created, the recipes in the plan are
* executed, but in reverse order, calling the AD methods of the planned recipes.
*
* \param[in,out] afieldset This is the input/output fieldset. On input, it should contain the
* fields in the "neededVars" variables specified when changeVarTraj was called.
* \param[in,out] trajRecipeExecutionPlan Can return the trajectory recipe plan for testing
                 validation.
* \returns List of products of the recipes that Vader did the adjoints for.
*/
oops::Variables Vader::changeVarAD(atlas::FieldSet & afieldset,
                                   vaderPlanType & trajRecipeExecutionPlan) const {
oops::Log::trace() << "entering Vader::changeVarAD" << std::endl;
    ASSERT_MSG(recipeExecutionPlanBuilt_, "initTLAD must be called before Vader::changeVarAD");
    oops::Variables varsAdjointed;
    executePlanAD(afieldset, recipeExecutionPlan_);
    for (const auto & varplan : recipeExecutionPlan_) {
        oops::Log::debug() << "Adding to varsAdjointed: " << varplan.first << std::endl;
        varsAdjointed.push_back(varplan.first);
    }
    oops::Log::trace() << "leaving Vader::changeVarAD" << std::endl;
    return varsAdjointed;
}

// ------------------------------------------------------------------------------------------------
namespace {

/*! \brief Planning State
*
* \details **PlanningState** records the "state" of the execution plan and the associated variables
* before beginning an attempt to plan a new variable, so that the attempt
* can be completely undone (including changes from all recursive calls) as a unit if the attempt
* ultimately fails.
*
*/
class PlanningState {
 public:
    PlanningState(const Vader::vaderPlanType & plan, const Vader::vaderPlanType & trajPlan,
                  const oops::Variables & neededVars, const oops::Variables & ingredientVars,
                  const oops::Variables & trajectoryVars) :
        planSize_(plan.size()), trajPlanSize_(trajPlan.size()), neededVars_(neededVars),
        ingredientVars_(ingredientVars), trajectoryVars_(trajectoryVars) {}

    void restore(Vader::vaderPlanType & plan, Vader::vaderPlanType & trajPlan,
                 oops::Variables & neededVars, oops::Variables & ingredientVars,
                 oops::Variables & trajectoryVars) const {
        while (plan.size() > planSize_) {
            plan.pop_back();
        }
        while (trajPlan.size() > trajPlanSize_) {
            trajPlan.pop_back();
        }
        neededVars = neededVars_;
        ingredientVars = ingredientVars_;
        trajectoryVars = trajectoryVars_;
    }

 private:
    const size_t planSize_;
    const size_t trajPlanSize_;
    const oops::Variables neededVars_;
    const oops::Variables ingredientVars_;
    const oops::Variables trajectoryVars_;
};

}  // namespace

// ------------------------------------------------------------------------------------------------
/*! \brief Plan Variables
*
* \details **planVariables** This overload just calls the other overload of planVariables in
* non-linear mode, first populating the linear-related parameters with unused stubs.
*
*/
void Vader::planVariables(oops::Variables & ingredientVars,
    oops::Variables & neededVars,
    vaderPlanType & plan) const {
    vaderPlanType emptyTrajPlan;
    oops::Variables emptyTrajVars;
    planVariables(ingredientVars, neededVars, plan, false, emptyTrajVars, emptyTrajPlan);
}

// ------------------------------------------------------------------------------------------------
/*! \brief Plan Variables
*
* \details **planVariables** calls the planVariable method for each variable in neededVars,
           creating a recipe plan. When called with planTLAD set to true, it will also produce
           a plan to produce the necessary trajectory variables for the recipes in the first plan.
*
* \param[in,out] ingredientVars List of variables that will be provided. ("From" variables)
* \param[in,out] neededVars List of variables to try to produce. ("To" variables)
* \param[out] plan The recipe execution plan that produces the maximum number of neededVars
* \param[in] planTLAD Flag to only consider recipes that have TLAD implemented for primary plan
* \param[in,out] trajectoryVars List of variables that will be provided as ingredients for the
* trajectory plan, which will be the variables in the FieldSet first passed to changeVarTraj.
* \param[in, out] trajRecipeExecutionPlan The (non-linear) recipe execution plan that produces any
* missing trajectory vars
*
*/
void Vader::planVariables(oops::Variables & ingredientVars,
                          oops::Variables & neededVars,
                          vaderPlanType & plan,
                          const bool planTLAD,
                          oops::Variables & trajectoryVars,
                          vaderPlanType & trajRecipeExecutionPlan) const {
    oops::Log::trace() << "entering Vader::planVariables" << std::endl;

    oops::Log::info() << "Ingredient variables passed to Vader::planVariables: " << ingredientVars
                      << std::endl;
    // Loop through all the requested fields in neededVars
    // Since neededVars can be modified by planVariable and planVariable calls
    // itself recursively, we make a copy of the list here before we start.

    const oops::Variables targetVariables(neededVars);

    for (const auto & targetVariable : targetVariables) {
        oops::Log::debug() <<
            "Vader::planVariables processing top-level requested variable: "
            << targetVariable << std::endl;
        // Since this function is called recursively, make sure targetVariable is
        // still needed
        if (!neededVars.has(targetVariable)) {
            oops::Log::debug() << targetVariable <<
                " is no longer in the variable list neededVars. Moving on." << std::endl;
            continue;
        }
        oops::Variables excludedVars;
        // Save the variable lists and plan sizes so they can be restored if we can't plan this
        // variable.
        const PlanningState stateBeforeTarget(plan, trajRecipeExecutionPlan, neededVars,
                                              ingredientVars, trajectoryVars);
        if (!planVariable(ingredientVars, neededVars, targetVariable, excludedVars, plan,
                          planTLAD, trajectoryVars, trajRecipeExecutionPlan)) {
            oops::Log::debug() << "Removing any planned recipes for " << targetVariable <<
                " since we couldn't plan it." << std::endl;
            stateBeforeTarget.restore(plan, trajRecipeExecutionPlan, neededVars, ingredientVars,
                                      trajectoryVars);
        }
    }
    oops::Log::trace() << "leaving Vader::planVariables" << std::endl;
}

// ------------------------------------------------------------------------------------------------
/*! \brief Plan Variable
*
* \details **planVariable** contains Vader's primary algorithm for attempting to
* find a recipe or sequence of recipes that can produce a desired variable. It:
* * Checks the cookbook for recipes for the desired field (the targetVariable)
* * Checks each recipe to see if its required ingredients have been provided
* * If an ingredient is missing, recursively calls itself to attempt to get it.
* * If planTLAD is true, it only considers recipes with TLAD methods implemented, and also requires
* that the recipe's trajectory variables are available or can be produced.
* * Adds the variable and recipe name to the "plan" if the recipe is viable.
* * If successful, removes the targetVariable from neededVars and returns 'true'
*
* \param[in,out] ingredientVars Available ingredient variables
* \param[in,out] neededVars Variables to try to produce
* \param[in] targetVariable variable this instance is trying to plan
* \param[in,out] excludedVars variables to not try to create due to recursion chain
* \param[in,out] plan ordered list of viable recipes that will get exectued later
* \param[in] planTLAD Flag to only consider recipes that have TLAD implemented
* \param[in, out] trajFieldset Fieldset containing trajectory variables
* \param[in,out] trajPlan ordered list of NL recipes to produce trajectory variables
* \return boolean 'true' if it successfully creates a plan for targetVariable, else false
*
*/
bool Vader::planVariable(oops::Variables & ingredientVars,
                         oops::Variables & neededVars,
                         const oops::Variable & targetVariable,
                         oops::Variables & excludedVars,
                         vaderPlanType & plan,
                         const bool planTLAD,
                         oops::Variables & trajectoryVars,
                         vaderPlanType & trajPlan) const {
    bool variablePlanned = false;
    oops::Log::trace() << "entering Vader::planVariable for variable: " << targetVariable <<
        std::endl;
    if (planTLAD) {
        oops::Log::debug() << "planVariable in TL/AD mode" << std::endl;
    } else {
        oops::Log::debug() << "planVariable in NL mode" << std::endl;
    }


    // Check if needed variable is part of the ingredients already.
    // Note: not comparing metadata because targetVar has levels, and
    // ingredientVars don't have them
    bool variableExists = ingredientVars.has(targetVariable);
    if (variableExists) {
        oops::Log::debug() << "Variable is part of the ingredients already. " << std::endl;
        neededVars -= targetVariable;
        return true;  // Don't need to check any recipes.
    }

    auto recipeList = cookbook_.find(targetVariable);

    // If recipeList is found, recipeList->second is a vector of unique_ptr's
    // to Recipe objects that produce 'variableName'
    if ((recipeList != cookbook_.end()) && !recipeList->second.empty()) {
        oops::Log::debug() <<
            "Vader cookbook contains at least one recipe for '" << targetVariable << "'" <<
            std::endl;
        for (const auto & recipe : recipeList->second) {
            if (planTLAD && !recipe->hasTLAD()) {
                oops::Log::debug() << "Not checking recipe: '" << recipe->name() <<
                    "' since it does not have TL/AD methods implemented." << std::endl;
                continue;
            }
            if (!planTLAD && !recipe->hasNL()) {
                oops::Log::debug() << "Not checking recipe: '" << recipe->name() <<
                    "' since it does not have NL method implemented." << std::endl;
                continue;
            }
            // Check that this recipe is not already in the plan to prevent
            // infinite recursion.
            for (const auto & planRecipe : plan) {
                if (planRecipe.second->name() == recipe->name()) {
                    break;
                }
            }
            oops::Log::debug() << "Checking to see if we have ingredients for recipe: " <<
                recipe->name() << std::endl;
            const PlanningState stateBeforeRecipe(plan, trajPlan, neededVars, ingredientVars,
                                                  trajectoryVars);
            bool haveIngredient = false;
            for (const auto & ingredient : recipe->ingredients()) {
                if (ingredient == targetVariable) {
                    oops::Log::error() << "Error: Ingredient list for " <<
                        recipe->name() << " contains the target." << std::endl;
                    break;
                }
                haveIngredient = ingredientVars.has(ingredient);
                if (!haveIngredient) {
                    oops::Log::debug() << "ingredient " << ingredient << " for recipe "
                        << recipe->name() << " not in ingredient variables." << std::endl;
                    if (excludedVars.has(ingredient)) {
                        oops::Log::debug() << "This ingredient is already being worked on at a " <<
                        "higher level of planVariable recursion, so not trying to create it at "
                        "this level." << std::endl;
                        haveIngredient = false;
                    } else {
                        oops::Log::debug() << "Seeing if we can create this ingredient " <<
                            "by calling planVariable recursively." << std::endl;
                        // Put the current targetVariable in excludedVars so we don't try to
                        // create it again as an ingredient at a lower level of recursion. Then
                        // call planVariable recursively.
                        excludedVars.push_back(targetVariable);
                        haveIngredient = planVariable(ingredientVars, neededVars, ingredient,
                                            excludedVars, plan, planTLAD, trajectoryVars, trajPlan);
                        // Remove the ingredient from excludedVars since we're back to this level.
                        excludedVars -= targetVariable;
                    }
                }
                oops::Log::debug() << "ingredient " << ingredient << " for recipe "
                     << recipe->name() << (haveIngredient ? " is" : " is not") << " available."
                     << std::endl;
                if (!haveIngredient) break;  // Missing an ingredient. Don't check the others.
            }
            bool haveTrajVars = true;
            if (haveIngredient && planTLAD) {
                oops::Log::debug() <<
                    "All ingredients are in the increment vars. Now checking the trajectory vars."
                    << std::endl;
                oops::Variables trajNeededVars = recipe->trajectoryVars();
                oops::Log::debug() <<
                    "Trajectory vars for recipe " << recipe->name() << " are: " << std::endl <<
                    trajNeededVars << std::endl;
                planVariables(trajectoryVars, trajNeededVars, trajPlan);
                if (trajNeededVars.size() == 0) {
                    oops::Log::debug() <<
                        "All trajectory vars available for recipe" << recipe->name() << std::endl;
                } else {
                    oops::Log::debug() << "Trajectory variables not available for recipe " <<
                        recipe->name() << std::endl;
                    haveTrajVars = false;
                }
            }
            if (haveIngredient && haveTrajVars) {
                oops::Log::debug() << "Adding recipe " << recipe->name() << " to plan."
                                   << std::endl;
                plan.push_back(std::pair<oops::Variable, const std::unique_ptr<RecipeBase> &>
                                                                    ({targetVariable, recipe}));
                variablePlanned = true;
                neededVars -= targetVariable;
                ingredientVars.push_back(targetVariable);
                break;  // Found a viable recipe. Don't need to check any other recipes.
            } else {
                oops::Log::debug() <<
                    "Not adding recipe " << recipe->name() << " to plan." << std::endl;
                // Undo everything this attempt did before trying the next recipe.
                stateBeforeRecipe.restore(plan, trajPlan, neededVars, ingredientVars,
                                          trajectoryVars);
            }
        }
    } else {
        oops::Log::debug() << "Vader cookbook does not contain a recipe for: "
            << targetVariable << std::endl;
    }
    oops::Log::trace() << "leaving Vader::planVariable for variable: " << targetVariable <<
        std::endl;
    return variablePlanned;
}

/*! \brief creates a field with correct functionspace and levels, and optionally
 *         initializes to zero. If the field already exists, asserts that it has
 *         enough levels in it.
 * \param[in,out] afieldset  Fieldset that needs to have the requested field
 *                           allocated
 * \param[in]     fieldname  Name of the field to be created or checked for dims
 * \param[in]     fs         Functionspace for creating the new field
 * \param[in]     nlevels    Number of levels for creating the new field
 * \param[in]     initZero   Initialize field with zeros? (default: false)
 */
void checkOrAddField(atlas::FieldSet & afieldset, const std::string & fieldname,
                     const atlas::FunctionSpace & fs, const int nlevels,
                     const bool initZero = false) {
    oops::Log::trace() << "entering checkOrAddField for Field: " << fieldname << std::endl;
    if (afieldset.has(fieldname))
    {
        // Verify the number of levels in the Field is enough for the recipe
        ASSERT(afieldset.field(fieldname).shape(1) >= nlevels);
    } else {
        // Create the field and put it in the FieldSet
        atlas::Field newField = fs.createField<double>(
                    atlas::option::name(fieldname) |
                    atlas::option::levels(nlevels));
        if (initZero) {
            // A new field for adjoint needs to be zeroed out
            auto view = atlas::array::make_view<double, 2>(newField);
            for (int j0 = 0; j0 < newField.shape(0); ++j0) {
                for (int j1 = 0; j1 < newField.shape(1); ++j1) {
                    view(j0, j1) = 0.0;
                }
            }
        }
        afieldset.add(newField);
    }
    oops::Log::trace() << "leaving checkOrAddField" << std::endl;
}

/*! \brief Attempts to call the other overload of this function by getting the functionspace and
 *         levels from a templateFieldSet that hopefully already contains a field with the same
 *         name.
 * \param[in,out] afieldset  Fieldset that needs to have the requested field
 *                           allocated
 * \param[in]     fieldname        Name of the field to be created or checked for dims
 * \param[in]     templateFieldSet FieldSet that may be used to harvest info about the field
 * \param[in]     initZero         Initialize field with zeros? (default: false)
 */
void checkOrAddField(atlas::FieldSet & afieldset, const std::string & fieldname,
    const atlas::FieldSet & templateFieldSet, const bool initZero = false) {
    oops::Log::trace() << "entering checkOrAddField (template fieldset) for Field: " << fieldname
        << std::endl;
    if (templateFieldSet.has(fieldname)) {
        checkOrAddField(afieldset, fieldname, templateFieldSet.field(fieldname).functionspace(),
                        templateFieldSet.field(fieldname).levels(), initZero);
    } else {
        // If this ASSERT is hit, the Vader client will need to create a zero field in the fieldset
        // before calling Vader. Perhaps someday we'll need to pass more information to Vader to
        // allow it to create the field.
        ASSERT_MSG(afieldset.has(fieldname),
            "Field " + fieldname + " is not present and Vader has insufficient information to "
            "create it.");
    }
    oops::Log::trace() << "leaving checkOrAddField (template fieldset)" << std::endl;
}

// ------------------------------------------------------------------------------------------------
/*! \brief Execute Plan (non-linear)
*
* \details **executePlanNL** calls, in order, the 'executeNL' method of the
* recipes specified in the recipeExecutionPlan that is passed in. (The recipeExecutionPlan is
* created through calls to planVariable.) Before executing the recipe, validations
* are performed on the passed FieldSet to ensure it has the ingredients, and the
* product Field is created and added if not already present.
*
* \param[in,out] afieldset A fieldset contaning only populated ingredient fields
* \param[in] recipeExecutionPlan ordered list of recipes that are to be exectued
*
*/
void Vader::executePlanNL(atlas::FieldSet & afieldset,
                          const vaderPlanType & recipeExecutionPlan) const {
    oops::Log::trace() << "entering Vader::executePlanNL" <<  std::endl;
    for (const auto & varPlan : recipeExecutionPlan) {
        for (const auto & ingredient :  varPlan.second->ingredients()) {
            ASSERT(afieldset.has(ingredient.name()));
        }
        // add product field to the fieldset if needed
        checkOrAddField(afieldset, varPlan.first.name(),
                        varPlan.second->productFunctionSpace(afieldset),
                        varPlan.second->productLevels(afieldset));
        varPlan.second->executeNL(afieldset);
        oops::Log::info() << "Variable '" << varPlan.first <<
            "' calculated using Vader recipe " << varPlan.second->name() << std::endl;
    }
    oops::Log::trace() << "leaving Vader::executePlanNL" <<  std::endl;
}
// ------------------------------------------------------------------------------------------------
/*! \brief Execute Plan (tangent linear)
*
* \details **executePlanTL** calls, in order, the 'execute' (tangent linear) method of the
* recipes specified in the recipeExecutionPlan that is passed in. (The recipeExecutionPlan is
* created through calls to planVariable.)
*
* \param[in,out] afieldset A fieldset contaning only populated ingredient (dx) fields
* \param[in] recipeExecutionPlan ordered list of recipes that are to be exectued
*
*/
void Vader::executePlanTL(atlas::FieldSet & afieldset,
                          const vaderPlanType & recipeExecutionPlan) const {
    oops::Log::trace() << "entering Vader::executePlanTL" <<  std::endl;
    // We must get the recipes specified in the recipeExecutionPlan out of the cookbook,
    // where they live
    for (const auto & varPlan : recipeExecutionPlan) {
        ASSERT(varPlan.second->hasTLAD());
        oops::Log::debug() << "Vader calculating variable '" << varPlan.first <<
            "' using TL recipe with name: " << varPlan.second->name() << std::endl;
        // add product field to the fieldset if needed
        checkOrAddField(afieldset, varPlan.first.name(),
                        varPlan.second->productFunctionSpace(afieldset),
                        varPlan.second->productLevels(afieldset));
        varPlan.second->executeTL(afieldset, trajectory_);
    }
    oops::Log::trace() << "leaving Vader::executePlanTL" <<  std::endl;
}
// ------------------------------------------------------------------------------------------------
/*! \brief Execute Plan (adjoint)
*
* \details **executePlanAD** calls, in reverse order, the 'execute' (adjoint) method of the
* recipes specified in the recipeExecutionPlan that is passed in. (The recipeExecutionPlan is
* created through calls to planVariable.)
*
* \param[in,out] afieldset A fieldset contaning only populated dy fields
* \param[in] recipeExecutionPlan ordered list of recipes that are to be exectued (reverse order)
*
*/
void Vader::executePlanAD(atlas::FieldSet & afieldset,
                          const vaderPlanType & recipeExecutionPlan) const {
    oops::Log::trace() << "entering Vader::executePlanAD" <<  std::endl;
    // We must get the recipes specified in the recipeExecutionPlan out of the cookbook,
    // where they live
    // We execute the adjoints in reverse order of the recipeExecutionPlan
    for (auto varPlanIt = recipeExecutionPlan.rbegin();
         varPlanIt != recipeExecutionPlan.rend();
         ++varPlanIt) {
        ASSERT(varPlanIt->second->hasTLAD());
        oops::Log::debug()  << "Vader performing adjoint of recipe with name: " <<
            varPlanIt->second->name() << std::endl;
        ASSERT(afieldset.has(varPlanIt->first.name()));
        // Check if ingredients need to be allocated
        for (const auto & ingredient :  varPlanIt->second->ingredients()) {
            // add ingredient field to the fieldset and zero out if needed
            checkOrAddField(afieldset, ingredient.name(), trajectory_, true);
        }
        varPlanIt->second->executeAD(afieldset, trajectory_);
    }
    oops::Log::trace() << "leaving Vader::executePlanAD" <<  std::endl;
}
// ------------------------------------------------------------------------------------------------
/*! \brief print
*
* \details **print** this method prints the current Vader cookbook in yaml form to an osstream
*
* \param[in] os the stream to print to
*
*/
void Vader::print(std::ostream & os) const {
    oops::Log::trace() << "entering Vader::print" <<  std::endl;
    std::string cookbookString("VADER Cookbook\ncookbook:\n");
    for (const auto & cbMember : cookbook_) {
        cookbookString += "  ";  // 2 space indent for yaml
        bool firstRecipe = true;
        cookbookString += cbMember.first.name() + ": [";
        for (const auto & recipe : cbMember.second) {
            if (firstRecipe) {
                firstRecipe = false;
            } else {
                cookbookString += ",";
            }
            cookbookString += recipe->name();
        }
        cookbookString += "]\n";
    }
    os << cookbookString;
    oops::Log::trace() << "leaving Vader::print" <<  std::endl;
}
}  // namespace vader
