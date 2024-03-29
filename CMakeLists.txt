#
# (C) Copyright 2021 UCAR.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

################################################################################
# VADER
################################################################################

cmake_minimum_required( VERSION 3.12 )
# option(VADER_BUILD_LANGUAGE_FORTRAN "Build Fortran bindings." ON)
list( APPEND VADER_LANGUAGES CXX)
# if (VADER_BUILD_LANGUAGE_FORTRAN)
# 	list( APPEND VADER_LANGUAGES Fortran)
# endif()

project( vader VERSION 1.1.0 LANGUAGES ${VADER_LANGUAGES} )

## Ecbuild integration
find_package( ecbuild QUIET )
include( ecbuild_system NO_POLICY_SCOPE )
ecbuild_declare_project()
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
set( CMAKE_DIRECTORY_LABELS ${PROJECT_NAME} )

## Configuration options
include( ${PROJECT_NAME}_compiler_flags )
option( ENABLE_VADER_DOC "Build VADER documentation" OFF )
option( ENABLE_VADER_MO  "Build VADER Met Office Code" OFF )

message( STATUS "VADER variables")
message( STATUS "  - ENABLE_VADER_DOC: ${ENABLE_VADER_DOC}" )
message( STATUS "  - ENABLE_VADER_MO: ${ENABLE_VADER_MO}" )

## Dependencies

# Required
find_package( jedicmake QUIET )  # Prefer find modules from jedi-cmake
find_package( oops 1.0.0 REQUIRED )

## Sources
add_subdirectory( src )
# add_subdirectory( test )
add_subdirectory( tools )

if( ENABLE_VADER_DOC )
    add_subdirectory( docs )
endif()

## Tests
ecbuild_add_test( TARGET ${PROJECT_NAME}_coding_norms
                  TYPE SCRIPT
                  COMMAND ${PROJECT_NAME}_cpplint.py
                  ARGS --quiet --recursive ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/test
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

## Package Config
ecbuild_install_project( NAME ${PROJECT_NAME} )

ecbuild_print_summary()
