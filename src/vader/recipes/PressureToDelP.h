/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef PRESSURE_TO_DELP_H_
#define PRESSURE_TO_DELP_H_

#include <vector>

#include "atlas/field/FieldSet.h"
#include "vader/vader/recipe.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe base class

class PressureToDelP : public RecipeBase {
   public:
      const static std::string Name;
      const static std::vector<std::string> Ingredients;

      explicit PressureToDelP(const eckit::Configuration &);

      std::string name() const override;
      std::vector<std::string> ingredients() const override;
      bool execute(atlas::FieldSet *) override;

   private:
};

} // namespace vader

#endif  // PRESSURE_TO_DELP_H_
