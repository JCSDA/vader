#
# (C) Copyright 2021 UCAR.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

################################################################################
# VADER
################################################################################

cmake_minimum_required( VERSION 3.23 )

project( vader VERSION 1.10.0 LANGUAGES CXX Fortran )

## Ecbuild integration
find_package( ecbuild QUIET )
include( ecbuild_system NO_POLICY_SCOPE )
ecbuild_declare_project()
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
set( CMAKE_DIRECTORY_LABELS ${PROJECT_NAME} )

## Configuration options
include( ${PROJECT_NAME}_compiler_flags )
option( ENABLE_VADER_DOC "Build VADER documentation" OFF )
option( OPENMP "Build VADER with OpenMP support" ON )

message( STATUS "VADER variables")
message( STATUS "  - ENABLE_VADER_DOC: ${ENABLE_VADER_DOC}" )
message( STATUS "  - OPENMP: ${OPENMP}" )

## Dependencies
find_package( Boost 1.64.0 REQUIRED )

# Required
find_package( jedicmake QUIET )  # Prefer find modules from jedi-cmake
find_package( MPI REQUIRED COMPONENTS C Fortran )
find_package( oops 1.13.0 REQUIRED )
find_package( NetCDF REQUIRED COMPONENTS C Fortran )
if(OPENMP)
  find_package( OpenMP REQUIRED COMPONENTS CXX )
endif()

find_package( gsw QUIET )
if( ${gsw_FOUND} )
    message(STATUS "GSW FOUND; Including Marine Recipes gsw (${gsw_VERSION})")
else( ${gsw_FOUND} )
    message(STATUS "GSW NOT FOUND: Excluding Marine Recipes")
endif( ${gsw_FOUND} )

## Sources
add_subdirectory( src )
add_subdirectory( tools )

if( ENABLE_VADER_DOC )
    add_subdirectory( docs )
endif()

if( BUILD_TESTING )
  # Test data
  find_package( jedi-model-data 1.0.0 QUIET )

  # If test data repo found build the tests
  if( jedi-model-data_FOUND )
      message( STATUS "jedi-model-data found; building vader tests" )
      add_subdirectory( test )
  else()
      message( STATUS "jedi-model-data not found; skipping vader tests" )
  endif()

  ## Tests
  ecbuild_add_test( TARGET ${PROJECT_NAME}_coding_norms
                    TYPE SCRIPT
                    COMMAND ${PROJECT_NAME}_cpplint.py
                    ARGS --quiet --recursive ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/test
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
endif()

## Package Config
ecbuild_install_project( NAME ${PROJECT_NAME} )

ecbuild_print_summary()
