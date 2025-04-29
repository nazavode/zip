cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(
  zip
  VERSION 0.1
  LANGUAGES CXX)

option(ZIP_ENABLE_TEST "Enable building unit tests" OFF)
option(ZIP_ENABLE_OPT_TEST "Enable optimization tests (needs lit+filecheck)"
       OFF)
option(ZIP_ENABLE_BENCHMARK "Enable building micro-benchmarks" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# ##############################################################################
# C++ standard level. Defines target 'zip::cxxstd'.
# ##############################################################################

add_library(ZipCxxStd INTERFACE)
set_property(TARGET ZipCxxStd PROPERTY INTERFACE_CXX_EXTENSIONS OFF)
set_property(TARGET ZipCxxStd PROPERTY INTERFACE_COMPILE_FEATURES cxx_std_17)
add_library(zip::cxxstd ALIAS ZipCxxStd)

# ##############################################################################
# Compiler warnings. Defines target 'zip::warnings'.
# ##############################################################################
add_library(ZipWarnings INTERFACE)
add_library(zip::warnings ALIAS ZipWarnings)
set_property(
  TARGET ZipWarnings
  PROPERTY INTERFACE_COMPILE_OPTIONS
           -pedantic-errors
           -Wall
           -Wextra
           -Werror
           -Wconversion
           -Wsign-conversion)

# ##############################################################################
# Library. Defines target 'zip::zip'
# ##############################################################################

add_library(ZipLib INTERFACE)
add_library(zip::zip ALIAS ZipLib)

target_sources(ZipLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/zip.h)

target_include_directories(
  ZipLib INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(ZipLib INTERFACE zip::cxxstd)

# ##############################################################################
# Unit tests. Defines target 'zip::unittest'
# ##############################################################################
add_executable(
  ZipUnitTest
  ${CMAKE_CURRENT_SOURCE_DIR}/test/unit/test-zip.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/unit/test-iterator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/unit/sanitize-options.cpp)
add_executable(zip::unittest ALIAS ZipUnitTest)
set_target_properties(
  ZipUnitTest
  PROPERTIES # zip::cxxstd isn't enough since INTERFACE_CXX_EXTENSIONS
             # is still broken:
             # https://gitlab.kitware.com/cmake/cmake/issues/17183
             CXX_EXTENSIONS OFF
             OUTPUT_NAME "zip-test")
target_link_libraries(ZipUnitTest PRIVATE zip::cxxstd zip::warnings zip::zip)
# Check for -Wgnu-zero-variadic-macro-arguments

if(ZIP_ENABLE_TEST)
  find_package(GTest REQUIRED CONFIG)
  target_link_libraries(ZipUnitTest PRIVATE GTest::gtest GTest::gmock
                                            GTest::gtest_main)
  enable_testing()
  add_test(NAME zip-test COMMAND ZipUnitTest)
else(ZIP_ENABLE_TEST)
  set_target_properties(ZipUnitTest PROPERTIES EXCLUDE_FROM_ALL ON)
endif(ZIP_ENABLE_TEST)

# ##############################################################################
# Optimization tests. Defines target check-zip
# ##############################################################################

if(ZIP_ENABLE_OPT_TEST)
  find_program(
    ZIP_FILECHECK
    NAMES FileCheck filecheck
    HINTS $ENV{ZIP_FILECHECK} REQUIRED)
  get_filename_component(ZIP_FILECHECK_PATH "${ZIP_FILECHECK}" DIRECTORY)
  message(STATUS "Found FileCheck: ${ZIP_FILECHECK}")

  find_program(ZIP_LIT lit HINTS $ENV{ZIP_LIT} REQUIRED)
  message(STATUS "Found lit: ${ZIP_LIT}")

  set(ZIP_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
  set(ZIP_BUILD_ROOT "${CMAKE_CURRENT_BINARY_DIR}")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/opt/lit.site.cfg.py.in
                 ${CMAKE_CURRENT_BINARY_DIR}/test/opt/lit.site.cfg.py @ONLY)

  add_custom_target(
    check-zip ALL
    COMMAND ${ZIP_LIT} test/opt/
    DEPENDS zip::zip
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif(ZIP_ENABLE_OPT_TEST)

# ##############################################################################
# Micro-benchmarks. Defines target 'zip::benchmark'
# ##############################################################################
add_executable(ZipBenchmark
               ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/bench-iterator.cpp)
add_executable(zip::benchmark ALIAS ZipBenchmark)
set_target_properties(
  ZipBenchmark
  PROPERTIES # zip::cxxstd isn't enough since INTERFACE_CXX_EXTENSIONS
             # is still broken:
             # https://gitlab.kitware.com/cmake/cmake/issues/17183
             CXX_EXTENSIONS OFF
             OUTPUT_NAME "zip-benchmark")
target_link_libraries(ZipBenchmark PRIVATE zip::cxxstd zip::warnings zip::zip)

if(ZIP_ENABLE_BENCHMARK)
  find_package(benchmark REQUIRED CONFIG)
  target_link_libraries(ZipBenchmark PRIVATE benchmark::benchmark
                                             benchmark::benchmark_main)

else(ZIP_ENABLE_BENCHMARK)
  # Don't download Google Benchmark and skip target
  set_target_properties(ZipBenchmark PROPERTIES EXCLUDE_FROM_ALL ON)
endif(ZIP_ENABLE_BENCHMARK)

# ##############################################################################
# Tooling Defines the following targets (only if the corresponding tool is
# found): - format-fix (run clang-format fixers in place on all sources) - tidy
# (run clang-tidy on all sources) - tidy-fix   (run clang-tidy fixers in place
# on all sources)
# ##############################################################################
include(FnGetTargetSources)
get_target_sources(_AllSources zip::zip zip::unittest zip::benchmark)

find_package(ClangFormat QUIET)
if(CLANG_FORMAT_FOUND)
  message(STATUS "clang-format: found ${CLANG_FORMAT_EXECUTABLE}")
  message(STATUS "clang-format: added custom target 'format-fix'")
  add_custom_target(
    format-fix
    COMMAND ${CLANG_FORMAT_EXECUTABLE} -style=file -i ${_AllSources}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif(CLANG_FORMAT_FOUND)

find_package(ClangTidy QUIET)
if(CLANG_TIDY_FOUND)
  message(STATUS "clang-tidy: found ${CLANG_TIDY_EXECUTABLE}")
  message(STATUS "clang-tidy: added custom targets 'tidy' and 'tidy-fix'")
  set(tidy_cmd ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_CURRENT_BINARY_DIR}
               -warnings-as-errors=*)
  add_custom_target(
    tidy
    COMMAND ${tidy_cmd} ${_AllSources}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(
    tidy-fix
    COMMAND ${tidy_cmd} -fix ${_AllSources}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif(CLANG_TIDY_FOUND)
