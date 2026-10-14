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

class OzoneMassMixingRatio_A_Parameters
  : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(
      OzoneMassMixingRatio_A_Parameters,
      RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
      "recipe name",
      this};
};

// -------------------------------------------------------------------------------------------------
/*! \brief Converts ozone mole fraction in ppm to ozone mass mixing ratio.
 *
 * This is the inverse of MoleFractionOfOzoneInAir_A, and uses the same model-data
 * constant `constoz`, defined as the ratio of the molecular weight of dry air to the
 * molecular weight of ozone, multiplied by 1.0e6:
 *
 *     constoz = (M_dry_air / M_ozone) * 1.0e6
 *
 * Using M_dry_air = 28.9644 g mol-1 and M_ozone = 47.9982 g mol-1 gives:
 *
 *     constoz = 603447.6
 *
 * It is needed when mole_fraction_of_ozone_in_air is a control variable but the
 * analysis is carried in ozone_mass_mixing_ratio, as in a GSI static-B 3D-Var.
 *
 * NL:
 *     o3_mass = o3_mole / constoz
 *
 * TL:
 *     o3_mass_tl = o3_mole_tl / constoz
 *
 * AD:
 *     o3_mole_ad += o3_mass_ad / constoz
 *     o3_mass_ad = 0
 */

class OzoneMassMixingRatio_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef OzoneMassMixingRatio_A_Parameters Parameters_;

    OzoneMassMixingRatio_A(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    oops::Variables trajectoryVars() const override;
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
