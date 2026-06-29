/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class MoleFractionOfOzoneInAir_A_Parameters
  : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(
      MoleFractionOfOzoneInAir_A_Parameters,
      RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
      "recipe name",
      this};
};

// -------------------------------------------------------------------------------------------------
/*! \brief Converts ozone mass mixing ratio to ozone mole fraction in ppm.
 *
 * The recipe uses the model-data constant `constoz`, defined as the ratio
 * of the molecular weight of dry air to the molecular weight of ozone,
 * multiplied by 1.0e6:
 *
 *     constoz = (M_dry_air / M_ozone) * 1.0e6
 *
 * Using M_dry_air = 28.9644 g mol-1 and M_ozone = 47.9982 g mol-1 gives:
 *
 *     constoz = 603447.6
 *
 * Multiplying ozone mass mixing ratio in kg kg-1 by `constoz` produces
 * ozone mole fraction in parts per million. For an ideal gas mixture,
 * mole fraction in ppm is numerically equivalent to volume mixing ratio
 * in ppmv.
 *
 * NL:
 *     o3_mole = constoz * o3_mass
 *
 * TL:
 *     o3_mole_tl = constoz * o3_mass_tl
 *
 * AD:
 *     o3_mass_ad += constoz * o3_mole_ad
 *     o3_mole_ad = 0
 */

class MoleFractionOfOzoneInAir_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef MoleFractionOfOzoneInAir_A_Parameters Parameters_;

    MoleFractionOfOzoneInAir_A(const Parameters_ &, const VaderConfigVars &);

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
