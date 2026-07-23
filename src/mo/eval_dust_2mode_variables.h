/*
 * (C) Crown Copyright 2023-2025 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */


#pragma once
#include "atlas/field/FieldSet.h"

namespace mo {

void eval_dust_accumulation_mode_mass_fraction_nl(
  atlas::FieldSet & stateFlds);

void eval_dust_coarse_mode_mass_fraction_nl(
  atlas::FieldSet & stateFlds);

void eval_dust_accumulation_mode_number_fraction_nl(
  atlas::FieldSet & stateFlds);

void eval_dust_coarse_mode_number_fraction_nl(
  atlas::FieldSet & stateFlds);

void eval_dust_accumulation_mode_mass_fraction_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

void eval_dust_coarse_mode_mass_fraction_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

void eval_dust_accumulation_mode_number_fraction_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

void eval_dust_coarse_mode_number_fraction_tl(
  atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

void eval_dust_accumulation_mode_mass_fraction_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

void eval_dust_coarse_mode_mass_fraction_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

void eval_dust_accumulation_mode_number_fraction_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

void eval_dust_coarse_mode_number_fraction_ad(
  atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

}  // namespace mo

