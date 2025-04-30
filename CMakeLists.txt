cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(
  zip
  VERSION 0.1
  LANGUAGES CXX)

# cmake-format: off
option(ZIP_ENABLE_TEST "Enable building unit tests" OFF)
option(ZIP_ENABLE_OPT_TEST "Enable optimization tests (needs lit+filecheck)" OFF)
option(ZIP_ENABLE_BENCHMARK "Enable building micro-benchmarks" OFF)
set(ZIP_USE_SANITIZER "" CACHE STRING
      "Define the sanitizer used to build tests. Possible values are: Address, Undefined and Address;Undefined. Defaults to empty string.")
# cmake-format: on

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(cmake/CPM.cmake)

# ##############################################################################
# Internal build configuration. Defines target ZipConfig. Meant to be privately
# linked to non-interface internal targets. Not to be exported.
# ##############################################################################

add_library(ZipConfig INTERFACE)

set(WARNINGS -pedantic-errors -Wall -Wextra -Werror -Wconversion
             -Wsign-conversion)

set_target_properties(
  ZipConfig
  PROPERTIES INTERFACE_CXX_EXTENSIONS OFF
             INTERFACE_COMPILE_FEATURES cxx_std_17
             INTERFACE_COMPILE_OPTIONS "${WARNINGS}")

include(FnGetSanitizerFlags)
get_sanitizer_flags(SANITIZER_FLAGS)
if(SANITIZER_FLAGS)
  message(STATUS "Setting sanitizer flags: ${SANITIZER_FLAGS}")
  target_compile_options(ZipConfig INTERFACE ${SANITIZER_FLAGS})
  target_link_options(ZipConfig INTERFACE ${SANITIZER_FLAGS})
endif()

# ##############################################################################
# Library. Defines target 'zip::zip'
# ##############################################################################

add_library(ZipLib INTERFACE)
add_library(zip::zip ALIAS ZipLib)

target_sources(ZipLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/zip.h)

target_include_directories(
  ZipLib INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(ZipLib PROPERTIES INTERFACE_CXX_EXTENSIONS OFF
                                        INTERFACE_COMPILE_FEATURES cxx_std_17)

# ##############################################################################
# Unit tests.
# ##############################################################################

if(ZIP_ENABLE_TEST)
  cpmaddpackage(NAME GTest GITHUB_REPOSITORY google/googletest VERSION 1.16.0)

  add_executable(
    ZipUnitTest
    ${CMAKE_CURRENT_SOURCE_DIR}/test/unit/test-zip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/unit/test-iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/unit/sanitize-options.cpp)
  add_executable(zip::unittest ALIAS ZipUnitTest)
  set_target_properties(
    ZipUnitTest
    PROPERTIES # INTERFACE_CXX_EXTENSIONS is still broken:
               # https://gitlab.kitware.com/cmake/cmake/issues/17183
               CXX_EXTENSIONS OFF OUTPUT_NAME "zip-test")

  target_link_libraries(ZipUnitTest PRIVATE ZipConfig zip::zip GTest::gtest
                                            GTest::gmock GTest::gtest_main)
  enable_testing()
  add_test(NAME zip-test COMMAND ZipUnitTest)
endif(ZIP_ENABLE_TEST)

# ##############################################################################
# Optimization tests.
# ##############################################################################

if(ZIP_ENABLE_OPT_TEST)
  find_package(FileCheck REQUIRED)
  if(NOT FILECHECK_FOUND)
    message(FATAL_ERROR "FileCheck not found.")
  endif()
  set(ZIP_FILECHECK ${FILECHECK_EXECUTABLE})
  message(STATUS "Found FileCheck: ${FILECHECK_EXECUTABLE}")

  find_program(ZIP_LIT lit HINTS $ENV{ZIP_LIT} REQUIRED)
  message(STATUS "Found lit: ${ZIP_LIT}")

  set(ZIP_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
  set(ZIP_BUILD_ROOT "${CMAKE_CURRENT_BINARY_DIR}")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/opt/lit.site.cfg.py.in
                 ${CMAKE_CURRENT_BINARY_DIR}/test/opt/lit.site.cfg.py @ONLY)

  add_custom_target(
    test-opt
    COMMAND ${ZIP_LIT} test/opt/
    DEPENDS zip::zip
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif(ZIP_ENABLE_OPT_TEST)

# ##############################################################################
# Micro-benchmarks.
# ##############################################################################

if(ZIP_ENABLE_BENCHMARK)

  cpmaddpackage(
    NAME
    benchmark
    GITHUB_REPOSITORY
    google/benchmark
    VERSION
    1.9.2
    OPTIONS
    "BENCHMARK_ENABLE_TESTING OFF"
    "BENCHMARK_ENABLE_WERROR OFF"
    "BENCHMARK_FORCE_WERROR OFF"
    "BENCHMARK_USE_LIBCXX ON")

  add_executable(ZipBenchmark
                 ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/bench-iterator.cpp)
  add_executable(zip::benchmark ALIAS ZipBenchmark)
  set_target_properties(
    ZipBenchmark
    PROPERTIES # INTERFACE_CXX_EXTENSIONS is still broken:
               # https://gitlab.kitware.com/cmake/cmake/issues/17183
               CXX_EXTENSIONS OFF OUTPUT_NAME "zip-benchmark")

  target_link_libraries(
    ZipBenchmark PRIVATE ZipConfig zip::zip benchmark::benchmark
                         benchmark::benchmark_main)
endif(ZIP_ENABLE_BENCHMARK)

# ##############################################################################
# Tooling. Defines the following targets (only if the corresponding tool is
# found):
#
# * format (run clang-format on all sources)
# * format-fix (run clang-format fixers in place on all sources)
# * tidy (run clang-tidy on all sources)
# * tidy-fix   (run clang-tidy fixers in place on all sources)
#
# ##############################################################################
set(_AllTargets ZipLib ZipUnitTest ZipBenchmark)
foreach(tgt IN LISTS _AllTargets)
  if(TARGET ${tgt})
    list(APPEND _Targets ${tgt})
  endif()
endforeach()
include(FnGetTargetSources)
get_target_sources(_Sources ${_Targets})

find_package(ClangFormat)

if(CLANG_FORMAT_FOUND)
  message(STATUS "clang-format: found ${CLANG_FORMAT_EXECUTABLE}")
  message(STATUS "clang-format: added custom target 'format'")
  message(STATUS "clang-format: added custom target 'format-fix'")
  add_custom_target(
    format
    COMMAND ${CLANG_FORMAT_EXECUTABLE} -style=file --dry-run --Werror
            ${_Sources}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(
    format-fix
    COMMAND ${CLANG_FORMAT_EXECUTABLE} -style=file -i ${_Sources}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif(CLANG_FORMAT_FOUND)

find_package(ClangTidy QUIET)

if(CLANG_TIDY_FOUND)
  message(STATUS "clang-tidy: found ${CLANG_TIDY_EXECUTABLE}")
  message(STATUS "clang-tidy: added custom target 'tidy'")
  message(STATUS "clang-tidy: added custom target 'tidy-fix'")
  set(tidy_cmd ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_CURRENT_BINARY_DIR}
               -warnings-as-errors=*)
  add_custom_target(
    tidy
    COMMAND ${tidy_cmd} ${_Sources}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(
    tidy-fix
    COMMAND ${tidy_cmd} -fix ${_Sources}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif(CLANG_TIDY_FOUND)
