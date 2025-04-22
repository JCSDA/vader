/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/base/Variable.h"
#include "oops/base/Variables.h"
#include "oops/util/AssociativeContainers.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "oops/util/parameters/RequiredPolymorphicParameter.h"
#include "oops/util/Printable.h"

namespace vader {

static const char * recipeNameString = "recipe name";
typedef eckit::Configuration VaderConfigVars;

// ------------------------------------------------------------------------------------------------
class RecipeParametersBase : public oops::Parameters {
  OOPS_ABSTRACT_PARAMETERS(RecipeParametersBase, Parameters)

 public:
  oops::RequiredParameter<std::string> name{
     recipeNameString,
     this};
};

// ------------------------------------------------------------------------------------------------
class EmptyRecipeParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(EmptyRecipeParameters, RecipeParametersBase)
};


// ------------------------------------------------------------------------------------------------
/*! \brief RecipeBase class defines interface for individual variable
           transformations.
 *
 *  \details This pure virtual class provides the template for all concrete
 *           classes that calculate a variable using other variables
 *           (ingredients) as input.
 */

class RecipeBase : public util::Printable,
                   private boost::noncopyable {
 public:
  RecipeBase() {}
  virtual ~RecipeBase() {}

/// Name of the recipe
  virtual std::string name() const = 0;

/// Variable Produced
  virtual oops::Variable product() const = 0;

/// Ingredients (list of variables required to setup and execute recipe)
  virtual oops::Variables ingredients() const = 0;

/// Required variables in Trajectory (only for recipes with TL/AD methods)
  virtual oops::Variables trajectoryVars() const { return oops::Variables(); }

/// Method returning the number of levels in the variable produced
  virtual size_t productLevels(const atlas::FieldSet &) const = 0;

/// Method returning the functionspace for the variable produced
  virtual atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const = 0;

/// Flag indicating whether the recipe implements the executeTL/AD methods.
  virtual bool hasTLAD() const { return false; }

/// Flag indicating whether the recipe implements the executeNL method.
  virtual bool hasNL() const { return true; }

/// Execute methods perform the variable change
  virtual void executeNL(atlas::FieldSet &) {}
  virtual void executeTL(atlas::FieldSet &, const atlas::FieldSet &) {}
  virtual void executeAD(atlas::FieldSet &, const atlas::FieldSet &) {}

 private:
  virtual void print(std::ostream &) const;
};

// ------------------------------------------------------------------------------------------------

/// Recipe Factory
class RecipeFactory {
 public:
  static RecipeBase * create(const std::string name,
                             const RecipeParametersBase &,
                             const eckit::Configuration &);
  static std::unique_ptr<RecipeParametersBase> createParameters(const std::string &);

  static std::vector<std::string> getMakerNames() {
    return oops::keys(getMakers());
  }

  virtual ~RecipeFactory() = default;

 protected:
  explicit RecipeFactory(const std::string &);

 private:
  virtual RecipeBase* make(const RecipeParametersBase &,
                           const VaderConfigVars &) = 0;

  virtual std::unique_ptr<RecipeParametersBase> makeParameters() const = 0;

  static std::map < std::string, RecipeFactory * > & getMakers() {
    static std::map < std::string, RecipeFactory * > makers_;
    return makers_;
  }
};

// ------------------------------------------------------------------------------------------------

template<class T>
class RecipeMaker : public RecipeFactory {
    typedef typename T::Parameters_ Parameters_;
    RecipeBase * make(const RecipeParametersBase & params,
                      const VaderConfigVars & configVariables) override {
        const auto &stronglyTypedParams = dynamic_cast<const Parameters_&>(params);
        return new T(stronglyTypedParams, configVariables);
    }
    std::unique_ptr<RecipeParametersBase> makeParameters() const override {
        return std::make_unique<Parameters_>();
    }

 public:
     explicit RecipeMaker(const std::string & name) : RecipeFactory(name) {}
};

// ------------------------------------------------------------------------------------------------
class RecipeParametersWrapper : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(RecipeParametersWrapper, Parameters)

 public:
    oops::RequiredPolymorphicParameter<RecipeParametersBase, RecipeFactory> recipeParams {
        recipeNameString,
        this};
};
// ------------------------------------------------------------------------------------------------

}  // namespace vader
