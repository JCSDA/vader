/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef VADER_H_
#define VADER_H_

#include <unordered_map>
#include <memory>

#include "atlas/field/FieldSet.h"
#include "oops/base/Variables.h"

#include "RecipeBase.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Vader class

class Vader {
   public:
      Vader(const eckit::Configuration & config);
      ~Vader();

      void changeVar(atlas::FieldSet * afieldset, oops::Variables &) const;

   private:
      std::unordered_map<std::string, std::vector<std::unique_ptr<RecipeBase>>> cookbook_;
      std::unordered_map<std::string, std::vector<std::string>> getDefaultCookbookDef();

      void createCookbook(std::unordered_map<std::string, std::vector<std::string>>, const eckit::Configuration &);
      bool getVariable(atlas::FieldSet * afieldset, oops::Variables & neededVars, const std::string targetVariable) const;
};

} // namespace vader

#endif  // VADER_H_
