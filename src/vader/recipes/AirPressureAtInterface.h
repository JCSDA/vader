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

namespace vader
{

// -------------------------------------------------------------------------------------------------

class AirPressureAtInterface_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(AirPressureAtInterface_AParameters, RecipeParametersBase)

 public:
    oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief AirPressureAtInterface_A class defines a recipe for pressure levels from pressure
           thickness.
 *
 *  \details This recipe uses surface pressure with hybrid sigma pressure coordinate coefficients
 *           ak and bk to compute pressure at the layer edges. It does not provide TL/AD algorithms.
 */
class AirPressureAtInterface_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef AirPressureAtInterface_AParameters Parameters_;

    AirPressureAtInterface_A(const Parameters_ &, const VaderConfigVars &);

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

class AirPressureAtInterface_BParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(AirPressureAtInterface_BParameters, RecipeParametersBase)

 public:
    oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief AirPressureAtInterface_B class defines a recipe for pressure levels from pressure
           thickness.
 *
 *  \details This recipe sums the pressure thickness value with the pressure level beneath to
 *           produce the next pressure level value. It does not provide TL/AD algorithms.
 *           Note that executing this recipe requires that air_pressure_at_top_of_atmosphere_model
 *           be populated in the VaderConfigVars passed to the recipe constructor.
 */
class AirPressureAtInterface_B : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef AirPressureAtInterface_BParameters Parameters_;

    AirPressureAtInterface_B(const Parameters_ &, const VaderConfigVars &);

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
class AirPressureAtInterface_CParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(AirPressureAtInterface_CParameters, RecipeParametersBase)

 public:
    oops::RequiredParameter<std::string> name{"recipe name", this};
};


/*! \brief AirPressureAtInterface_C class defines a recipe for pressure levels at MPAS layer interfaces.
 *
 *  \details This recipe interpolate the pressure from MPAS layer mid-points
 *   (height coordinate) to produce the pressure at the layer interfcaces. It does not
 *   provide TL/AD algorithms.
 */
class AirPressureAtInterface_C : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef AirPressureAtInterface_CParameters Parameters_;

    AirPressureAtInterface_C(const Parameters_ &, const VaderConfigVars &);

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

class AirPressureAtInterface_DParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(AirPressureAtInterface_DParameters, RecipeParametersBase)

 public:
    oops::RequiredParameter<std::string> name{"recipe name", this};
};

/*! \brief AirPressureAtInterface_D class defines a recipe for pressure edges from pressure
           at surface.
 *
 *  \details This recipe uses surface pressure with hybrid sigma pressure coordinate coefficients
 *           ak and bk to compute pressure at the layer edges. It provides TL/AD algorithms.
 */
class AirPressureAtInterface_D : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef AirPressureAtInterface_DParameters Parameters_;

    AirPressureAtInterface_D(const Parameters_ &, const VaderConfigVars &);

    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    oops::Variables trajectoryVars() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    void executeNL(atlas::FieldSet &) override;
    bool hasTLAD() const override { return true; }
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

// -------------------------------------------------------------------------------------------------

}  // namespace vader
