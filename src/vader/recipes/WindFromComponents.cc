/*
 * (C) Copyright 2026 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>

#include "oops/util/for_each.h"
#include "vader/recipes/WindFromComponents.h"

namespace vader {

// Tolerance for zero wind checks
static constexpr double epsilon = 1.0e-10;

// -------------------------------------------------------------------------------------------------

void windSpeedNL(const atlas::Field & u, const atlas::Field & v, atlas::Field & speed) {
  util::for_each_column(
    [](const auto u_col,
       const auto v_col,
       auto speed_col) {
       speed_col(0) = std::sqrt(u_col(0) * u_col(0) + v_col(0) * v_col(0));
    },
    u, v, speed);
}

void windSpeedTL(const atlas::Field & u, const atlas::Field & v,
                 const atlas::Field & u_tl, const atlas::Field & v_tl, atlas::Field & speed_tl) {
  util::for_each_column(
    [](const auto u_col,
       const auto v_col,
       const auto u_tl_col,
       const auto v_tl_col,
       auto speed_tl_col) {
        const double speed = std::sqrt(u_col(0) * u_col(0) + v_col(0) * v_col(0));
        if (speed > epsilon) {
          speed_tl_col(0) = (u_tl_col(0) * u_col(0) + v_tl_col(0) * v_col(0)) / speed;
        } else {
          speed_tl_col(0) = 0.0;
        }
    },
    u, v, u_tl, v_tl, speed_tl);
}

void windSpeedAD(const atlas::Field & u, const atlas::Field & v,
                 atlas::Field & u_ad, atlas::Field & v_ad, atlas::Field & speed_ad) {
  util::for_each_column(
    [](const auto u_col,
       const auto v_col,
       auto u_ad_col,
       auto v_ad_col,
       auto speed_ad_col) {
        const double speed = std::sqrt(u_col(0) * u_col(0) + v_col(0) * v_col(0));
        if (speed > epsilon) {
          u_ad_col(0) += (u_col(0) / speed) * speed_ad_col(0);
          v_ad_col(0) += (v_col(0) / speed) * speed_ad_col(0);
        }
        speed_ad_col(0) = 0.0;
    },
    u, v, u_ad, v_ad, speed_ad);
}

// -------------------------------------------------------------------------------------------------

void windToDirectionNL(const atlas::Field & u, const atlas::Field & v, atlas::Field & dir,
                       const double radToDeg) {
  util::for_each_value(
    [radToDeg](const double uu,
               const double vv,
               double & dd) {
        dd = std::atan2(uu, vv) * radToDeg;
        if (dd < 0.0) {
          dd += 360.0;
        }
    },
    u, v, dir);
}

// atan2(u,v) and atan2(-u,-v) differ by pi (a constant), so their partial derivatives
// with respect to u and v are identical:
//   d(atan2(u,v))/du = v / speed_sq
//   d(atan2(u,v))/dv = -u / speed_sq
void windToDirectionTL(const atlas::Field & u, const atlas::Field & v,
                       const atlas::Field & u_tl, const atlas::Field & v_tl,
                       atlas::Field & dir_tl, const double radToDeg) {
  util::for_each_value(
    [radToDeg](const double uu,
               const double vv,
               const double uu_tl,
               const double vv_tl,
               double & dd_tl) {
        const double speed_sq = uu * uu + vv * vv;
        if (speed_sq > epsilon) {
          dd_tl = (vv * uu_tl / speed_sq - uu * vv_tl / speed_sq) * radToDeg;
        } else {
          dd_tl = 0.0;
        }
    },
    u, v, u_tl, v_tl, dir_tl);
}

void windToDirectionAD(const atlas::Field & u, const atlas::Field & v,
                       atlas::Field & u_ad, atlas::Field & v_ad,
                       atlas::Field & dir_ad, const double radToDeg) {
  util::for_each_value(
    [radToDeg](const double uu,
               const double vv,
               double & uu_ad,
               double & vv_ad,
               double & dd_ad) {
        const double speed_sq = uu * uu + vv * vv;
        if (speed_sq > epsilon) {
          uu_ad += (vv / speed_sq) * dd_ad * radToDeg;
          vv_ad += (-uu / speed_sq) * dd_ad * radToDeg;
        }
        dd_ad = 0.0;
    },
    u, v, u_ad, v_ad, dir_ad);
}

}  // namespace vader
