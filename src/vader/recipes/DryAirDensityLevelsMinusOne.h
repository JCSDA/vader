/*
 * (C) Copyright 2021-2022 UCAR
 * (C) Crown Copyright 2023-2024 Met Office.
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

// ------------------------------------------------------------------------------------------------

// For renaming to CCPP standard in (hopefully) near future:
/* \brief DryAirDensityLevelsMinusOne_A class defines a recipe for dry air density
 *
 *  \details This instantiation of RecipeBase produces dry_air_density
 *           using air_potential_temperature_at_interface,
 *           water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water,
 *           cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water,
 *           cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water and
 *           air_pressure_extended_up_by_one as inputs.
 */
/*! \brief DryAirDensityLevelsMinusOne_A class defines a recipe for dry air density
 *
 *  \details This instantiation of RecipeBase produces dry air density
 *           using air_potential_temperature, specific humidity,
 *           cloud_liquid_water_mixing_ratio_wrt_moist_air_and_condensed_water,
 *           cloud_ice_mixing_ratio_wrt_moist_air_and_condensed_water
 *           and pressure (air_pressure_levels) as inputs.
 */

class DryAirDensityLevelsMinusOne_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef EmptyRecipeParameters Parameters_;

    DryAirDensityLevelsMinusOne_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
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

// ------------------------------------------------------------------------------------------------

}  // namespace vader
