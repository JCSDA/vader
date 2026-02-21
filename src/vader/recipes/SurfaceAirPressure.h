/*
 * (C) Copyright 2021-2026 UCAR
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

class SurfaceAirPressure_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(SurfaceAirPressure_AParameters, RecipeParametersBase)

 public:
    oops::RequiredParameter<std::string> name{"recipe name", this};
};

// -------------------------------------------------------------------------------------------------
/*! \brief SurfaceAirPressure_A class defines a recipe for air_pressure_at_surface
 *         from air_pressure_thickness
 *
 *         NL:
 *             ps(j) = ptop + sum_{level=0..nLevel-1} delp(j, level)
 *         TL:
 *             ps'(j) = sum_{level} delp'(j, level)    (ptop fixed)
 *         AD:
 *             delp_ad(j, level) += ps_ad(j)  for each level
 *             ps_ad(j) = 0
 *
 *         where:
 *         - delp is air_pressure_thickness (Pa)
 *         - ps is air_pressure_at_surface (Pa)
 *         - ptop is air_pressure_at_top_of_atmosphere_model (Pa)
 *         - j indexes horizontal points (0..npoint-1)
 *         - level indexes vertical levels (0..nLevel-1)
 *
 *  \details This recipe produces surface pressure from air pressure thickness (delp) by summing
 *           the pressure at the model top with all the delp values.
 */
class SurfaceAirPressure_A : public RecipeBase
{
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef SurfaceAirPressure_AParameters Parameters_;

    SurfaceAirPressure_A(const Parameters_ &, const VaderConfigVars &);

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
