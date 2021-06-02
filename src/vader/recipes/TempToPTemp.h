/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef TEMP_TO_PTEMP_H_
#define TEMP_TO_PTEMP_H_

#include <vector>
#include <string>

#include "eckit/config/Configuration.h"
#include "atlas/field/FieldSet.h"
#include "vader/vader/RecipeBase.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe base class

class TempToPTemp : public RecipeBase {
   public:
      const static std::string Name;
      const static std::vector<std::string> Ingredients;

      TempToPTemp(const eckit::Configuration &);

      // Recipe base class overrides
      std::string name() const override;
      std::vector<std::string> ingredients() const override;
      bool execute(atlas::FieldSet *) override;

   private:
      void print(std::ostream &) const override;
};

} // namespace vader

#endif  // TEMP_TO_PTEMP_H_
