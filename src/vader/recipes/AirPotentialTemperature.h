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
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

class AirPotentialTemperature_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(AirPotentialTemperature_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief AirPotentialTemperature_A class defines a recipe for potential temperature
 *
 *  \details This instantiation of RecipeBase produces potential temperature
 *           using temperature and surface pressure as inputs. The parameters
 *           p0 and kappa can be specified via the constructor configuration.
 *           If they are not, the code will attempt to provide default values.
 *           (See https://glossary.ametsoc.org/wiki/Potential_temperature)
 */
class AirPotentialTemperature_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef AirPotentialTemperature_AParameters Parameters_;

    AirPotentialTemperature_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

// -------------------------------------------------------------------------------------------------

class AirPotentialTemperature_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(AirPotentialTemperature_BParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief AirPotentialTemperature_B class defines a recipe for potential temperature
 *
 *  \details This instantiation of RecipeBase produces potential temperature
 *           using temperature and pressure to the kappa.
 *           (See https://glossary.ametsoc.org/wiki/Potential_temperature)
 */
class AirPotentialTemperature_B : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef AirPotentialTemperature_BParameters Parameters_;

    AirPotentialTemperature_B(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

}  // namespace vader
