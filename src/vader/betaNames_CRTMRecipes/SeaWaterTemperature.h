/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "OceanConversions/OceanConversions.interface.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

class SeaWaterTemperature_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SeaWaterTemperature_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief SeaWaterTemperature_A class defines a non-linear recipe for insitu water temperature
 *
 *  \details This instantiation of RecipeBase produces insitu water temperature using
 *           sea water potential temperature, salinity, longitude, lattitude, and
 *           sea_water_depth information.
 */
class SeaWaterTemperature_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef SeaWaterTemperature_AParameters Parameters_;

    SeaWaterTemperature_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return false; }
    void executeNL(atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

// -------------------------------------------------------------------------------------------------

class SeaWaterTemperature_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SeaWaterTemperature_BParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief SeaWaterTemperature_B class defines TL/AD recipes for insitu water temperature
 *.        It is separated from SeaWaterTemperature_A to isolate TL/AD methods and
 *         to be able to specify different ingredients for NL and TL/AD.
 *
 *  \details This instantiation of RecipeBase in its TL operator updates insitu water temperature
 *           increment using sea water potential temperature, salinity, longitude, lattitude,
 *           sea water depth, sea area fraction and sea water temperature in the trajectory,
 *           and sea water potential temperature and salinity in the increment.
 */
class SeaWaterTemperature_B : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;
    static const oops::Variables TrajectoryVars;

    typedef SeaWaterTemperature_BParameters Parameters_;

    SeaWaterTemperature_B(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    oops::Variables trajectoryVars() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    bool hasNL() const override { return false; }
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    void computeJac(const atlas::FieldSet &);

    const VaderConfigVars & configVariables_;

    // Jacobian fields
    bool haveJac_;
    std::unique_ptr<atlas::Field> jacT_;
    std::unique_ptr<atlas::Field> jacS_;
};

}  // namespace vader
