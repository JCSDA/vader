/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "atlas/array.h"
#include "oops/util/abor1_cpp.h"
#include "oops/util/DateTime.h"
#include "oops/util/Logger.h"
#include "vader/betaNames_CRTMRecipes/GSI_Specific/LeafAreaIndex.h"

namespace vader {

const char LeafAreaIndex_A::Name[] = "LeafAreaIndex_A";
const oops::Variables LeafAreaIndex_A::Ingredients{std::vector<std::string>{
    "gsi_surface_type_index", "vtype", "stype", "vegetation_type_index"}};

static RecipeMaker<LeafAreaIndex_A> makerLeafAreaIndex_A_(LeafAreaIndex_A::Name);

// LAI lookup tables for the 13 CRTM microwave vegetation types.
static constexpr std::array<double, 13> kLaiMin = {
    3.08, 1.85, 2.80, 5.00, 1.00, 0.50, 0.52, 0.60, 0.50, 0.60, 0.10, 1.56, 0.01};
static constexpr std::array<double, 13> kLaiMax = {
    6.48, 3.31, 5.50, 6.40, 5.16, 3.66, 2.90, 2.60, 3.66, 2.60, 0.75, 5.68, 0.01};

// Days-of-year for mid-Jan and mid-Jul (leap years ignored).
static constexpr std::array<double, 3> kDayOfPeak = {15.5, 196.5, 380.5};

LeafAreaIndex_A::LeafAreaIndex_A(const Parameters_ & params,
                                 const VaderConfigVars & configVariables)
    : configVariables_{configVariables} {
  oops::Log::trace() << "LeafAreaIndex_A::LeafAreaIndex_A(params)" << std::endl;
}

std::string LeafAreaIndex_A::name() const { return LeafAreaIndex_A::Name; }

oops::Variable LeafAreaIndex_A::product() const { return oops::Variable{"leaf_area_index"}; }

oops::Variables LeafAreaIndex_A::ingredients() const { return LeafAreaIndex_A::Ingredients; }

size_t LeafAreaIndex_A::productLevels(const atlas::FieldSet & afieldset) const { return 1; }

atlas::FunctionSpace LeafAreaIndex_A::productFunctionSpace(
    const atlas::FieldSet & afieldset) const {
  return afieldset.field("gsi_surface_type_index").functionspace();
}

void LeafAreaIndex_A::executeNL(atlas::FieldSet & afieldset) {
  oops::Log::trace() << "LeafAreaIndex_A::executeNL starting" << std::endl;

  // Day-of-year (0-based, fractional) is derived from the valid time that
  // fv3-jedi stamps onto every input field's metadata before invoking Vader.
  // Matches the legacy convention in fv3jedi_vc_model2geovals_mod.f90:
  //     fractional_day_of_year = datetime_seconds_since_jan1(time) / 86400
  const std::string datetime_str = afieldset.field("vtype").metadata().getString("datetime");
  const util::DateTime datetime(datetime_str);
  double doy = static_cast<double>(datetime.secondsSinceJan1()) / 86400.0;

  // Compute season interpolation weights once.
  if (doy < kDayOfPeak[0]) doy += 365.0;

  int n1 = -1;
  int n2 = -1;
  for (int ni = 0; ni < 2; ++ni) {
    if (doy >= kDayOfPeak[ni] && doy < kDayOfPeak[ni + 1]) {
      n1 = ni;
      n2 = ni + 1;
      break;
    }
  }
  if (n1 < 0) {
    ABORT("LeafAreaIndex_A::executeNL received invalid day_of_year");
  }
  const double w1 = (kDayOfPeak[n2] - doy) / (kDayOfPeak[n2] - kDayOfPeak[n1]);
  const double w2 = (doy - kDayOfPeak[n1]) / (kDayOfPeak[n2] - kDayOfPeak[n1]);
  // Map index n2 == 2 back into the {0, 1} = {min, max} pair.
  if (n2 == 2) n2 = 0;

  const atlas::Field & sfc_type_field = afieldset.field("gsi_surface_type_index");
  auto sfc_type_view = atlas::array::make_view<const double, 2>(sfc_type_field);
  auto vtype_view = atlas::array::make_view<const double, 2>(afieldset.field("vtype"));
  auto stype_view = atlas::array::make_view<const double, 2>(afieldset.field("stype"));
  auto vegtype_idx_view = atlas::array::make_view<const double, 2>(
      afieldset.field("vegetation_type_index"));
  // Atlas convention: lonlat(:, 0) = longitude, lonlat(:, 1) = latitude (degrees).
  auto lonlat_view = atlas::array::make_view<const double, 2>(
      sfc_type_field.functionspace().lonlat());
  auto lai_view = atlas::array::make_view<double, 2>(afieldset.field("leaf_area_index"));

  const size_t nx = sfc_type_view.shape(0);

  // Pass 1: zero everything, then compute LAI on land cells.
  bool any_glacial = false;
  for (size_t i = 0; i < nx; ++i) lai_view(i, 0) = 0.0;

  for (size_t i = 0; i < nx; ++i) {
    if (std::lround(sfc_type_view(i, 0)) != 1) continue;
    const int64_t vtype = std::lround(vtype_view(i, 0));
    const int64_t stype = std::lround(stype_view(i, 0));
    if (vtype == 15 || stype == 16) any_glacial = true;
    const int64_t lai_veg_type = std::lround(vegtype_idx_view(i, 0));
    if (lai_veg_type > 0 &&
        lai_veg_type <= static_cast<int64_t>(kLaiMin.size())) {
      const std::array<double, 2> lai_season = {
          kLaiMin[lai_veg_type - 1], kLaiMax[lai_veg_type - 1]};
      if (lonlat_view(i, 1) < 0.0) {
        lai_view(i, 0) = w1 * lai_season[n2] + w2 * lai_season[n1];
      } else {
        lai_view(i, 0) = w1 * lai_season[n1] + w2 * lai_season[n2];
      }
    }
  }

  if (any_glacial) {
    for (size_t i = 0; i < nx; ++i) lai_view(i, 0) = 0.0;
  }

  oops::Log::trace() << "LeafAreaIndex_A::executeNL done" << std::endl;
}

}  // namespace vader
