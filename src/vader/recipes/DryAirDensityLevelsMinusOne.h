/*
 * (C) Copyright 2021-2022 UCAR
 * (C) Crown Copyright 2023-2024 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_RECIPES_DRYAIRDENSITYLEVELSMINUSONE_H_
#define SRC_VADER_RECIPES_DRYAIRDENSITYLEVELSMINUSONE_H_

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
 *           using potential_temperature, specific humidity,
 *           mass_content_of_cloud_liquid_water_in_atmosphere_layer,
 *           mass_content_of_cloud_ice_in_atmosphere_layer
 *           and pressure (air_pressure_levels) as inputs.
 */

class DryAirDensityLevelsMinusOne_A : public RecipeBase {
 public:
    static const char Name[];
    static const std::vector<std::string> Ingredients;

    typedef EmptyRecipeParameters Parameters_;

    DryAirDensityLevelsMinusOne_A(const Parameters_ &, const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    std::string product() const override;
    std::vector<std::string> ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    bool executeNL(atlas::FieldSet &) override;
    bool executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    bool executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    const VaderConfigVars & configVariables_;
};

// ------------------------------------------------------------------------------------------------

}  // namespace vader

#endif  // SRC_VADER_RECIPES_DRYAIRDENSITYLEVELSMINUSONE_H_
