/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "vader/RecipeBase.h"

#include <map>
#include <vector>

#include "oops/util/abor1_cpp.h"
#include "oops/util/Logger.h"

namespace vader {

// ------------------------------------------------------------------------------------------------

RecipeFactory::RecipeFactory(const std::string & name) {
  if (getMakers().find(name) != getMakers().end()) {
    oops::Log::error() << name << " already registered in vader::RecipeFactory."
      << std::endl;
    ABORT("Element already registered in vader::RecipeFactory.");
  }
  getMakers()[name] = this;
}

// ------------------------------------------------------------------------------------------------

RecipeBase * RecipeFactory::create(const std::string name,
                                   const RecipeParametersBase & params) {
  oops::Log::trace() << "RecipeFactory::create(name, params) starting for name: " << name <<
    std::endl;
  typename std::map<std::string, RecipeFactory*>::iterator jloc = getMakers().find(name);
  if (jloc == getMakers().end()) {
    oops::Log::error() << name << " does not exist in vader::RecipeFactory." << std::endl;
    ABORT("Element does not exist in vader::RecipeFactory.");
  }
  RecipeBase * ptr = jloc->second->make(params);
  oops::Log::trace() << "RecipeFactory::create(name, params) finished for name: " << name <<
    std::endl;
  return ptr;
}

// ------------------------------------------------------------------------------------------------

RecipeBase * RecipeFactory::create(const std::string name) {
  oops::Log::trace() << "RecipeFactory::create(name) starting for name: "
    << name << std::endl;
  typename std::map<std::string, RecipeFactory*>::iterator jloc =
    getMakers().find(name);
  if (jloc == getMakers().end()) {
    oops::Log::error() << name << " does not exist in vader::RecipeFactory." << std::endl;
    ABORT("Element does not exist in vader::RecipeFactory.");
  }
  RecipeBase * ptr = jloc->second->make();
  oops::Log::trace() << "RecipeFactory::create(name) finished for name: " << name << std::endl;
  return ptr;
}

// ------------------------------------------------------------------------------------------------

std::unique_ptr<RecipeParametersBase>
RecipeFactory::createParameters(const std::string &name) {
  typename std::map<std::string, RecipeFactory*>::iterator it =
      getMakers().find(name);
  if (it == getMakers().end()) {
    throw std::runtime_error(name + " does not exist in vader::RecipeFactory");
  }
  return it->second->makeParameters();
}

void RecipeBase::print(std::ostream & os) const {
  os << name();
}

// ------------------------------------------------------------------------------------------------

}  // namespace vader
