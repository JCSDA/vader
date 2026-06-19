/*
 * (C) Copyright 2026 UCAR
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

class TropopausePressure_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(TropopausePressure_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// -------------------------------------------------------------------------------------------------

/*! \brief TropopausePressure_A computes tropopause pressure
 *
 *  \details Formula: Finds tropopause based on vertical gradient of potential temperature.
 *           Uses delta-Theta/delta-Z threshold (10/1500 K/m) to identify tropopause.
 *           Checks over at least 35 hPa pressure difference and ensures result is above 700 hPa.
 */
class TropopausePressure_A : public RecipeBase {
 public:
  static const char Name[];
  static const oops::Variables Ingredients;

  typedef TropopausePressure_A_Parameters Parameters_;

  TropopausePressure_A(const Parameters_ &, const VaderConfigVars &);

  // Recipe base class overrides
  std::string name() const override;
  oops::Variable product() const override;
  oops::Variables ingredients() const override;
  size_t productLevels(const atlas::FieldSet &) const override;
  atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
  bool hasTLAD() const override { return true; }
  void executeNL(atlas::FieldSet &) override;
  void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
  void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
  const VaderConfigVars & configVariables_;
};

}  // namespace vader
