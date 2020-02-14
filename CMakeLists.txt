cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(zip VERSION 0.1 LANGUAGES CXX)

option(ZIP_ENABLE_TEST      "Enable building unit tests" ON)
option(ZIP_ENABLE_BENCHMARK "Enable building micro-benchmarks" OFF)
option(ZIP_ENABLE_OPTREPORT "Enable generation of optimization reports" OFF)

# Add extra finders
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Always enable generation of compilation database to
# make clang tooling and vscode happy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Generate compile commands database" FORCE)

###########################################################
# C++ standard level
# Defines target 'zip::cxxstd'
###########################################################
include(FnAddCxxStdLibrary)
add_cxxstd_library(ZipCxxStd 17)
add_library(zip::cxxstd ALIAS ZipCxxStd)

###########################################################
# Compiler warnings
# Defines target 'zip::warnings'
###########################################################
add_library(ZipWarnings INTERFACE)
add_library(zip::warnings ALIAS ZipWarnings)
set_property(TARGET ZipWarnings
             PROPERTY INTERFACE_COMPILE_OPTIONS
                      -pedantic-errors
                      -Wall
                      -Wextra
                      -Werror
                      -Wconversion
                      -Wsign-conversion)

###########################################################
# Actual library
# Defines target 'zip::zip'
###########################################################

add_library(ZipLib INTERFACE)
add_library(zip::zip ALIAS ZipLib)

target_sources(ZipLib
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/zip.h)

target_include_directories(ZipLib
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(ZipLib
    INTERFACE
        zip::cxxstd)

###########################################################
# Unit tests
# Defines target 'zip::test'
###########################################################
add_executable(ZipTest
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test-zip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test-iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/sanitize-options.cpp)
add_executable(zip::test ALIAS ZipTest)
set_target_properties(ZipTest
    PROPERTIES
        # zip::cxxstd isn't enough since INTERFACE_CXX_EXTENSIONS
        # is still broken:
        # https://gitlab.kitware.com/cmake/cmake/issues/17183
        CXX_EXTENSIONS OFF
        OUTPUT_NAME "zip-test")
target_link_libraries(ZipTest
    PRIVATE
        zip::cxxstd
        zip::warnings
        zip::zip)
# Check for -Wgnu-zero-variadic-macro-arguments

    
if(ZIP_ENABLE_TEST)
    find_package(GTest QUIET)
    if(NOT GTEST_FOUND)
        message(STATUS "Installing googletest via submodule")
        include(FnGitSubmoduleUpdate)
        git_submodule_update(vendor/googletest)
        set(vendor_source_dir ${CMAKE_SOURCE_DIR}/vendor/googletest)
        set(vendor_binary_dir ${CMAKE_BINARY_DIR}/vendor/googletest)
        # Prevent overriding the parent project's compiler/linker
        # settings on Windows
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(BUILD_GTEST ON CACHE BOOL "" FORCE)
        set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        set(INSTALL_GMOCK OFF CACHE BOOL "" FORCE)
        # Add googletest directly to our build. This defines
        # the gtest and gtest_main targets.
        add_subdirectory(${vendor_source_dir} ${vendor_binary_dir} EXCLUDE_FROM_ALL)
        # The gtest/gtest_main targets carry header search path
        # dependencies automatically when using CMake 2.8.11 or
        # later. Otherwise we have to add them here ourselves.
        if (CMAKE_VERSION VERSION_LESS 2.8.11)
            include_directories("${gtest_SOURCE_DIR}/include")
        endif ()
    endif()

    target_link_libraries(ZipTest
        PRIVATE
            gtest
            gmock
            gtest_main)
    enable_testing()
    add_test(NAME zip-test COMMAND ZipTest)
else(ZIP_ENABLE_TEST)
    # Don't download Google Test and skip target
    set_target_properties(ZipTest
        PROPERTIES
            EXCLUDE_FROM_ALL ON)
endif(ZIP_ENABLE_TEST)

###########################################################
# Micro-benchmarks
# Defines target 'zip::benchmark'
###########################################################
add_executable(ZipBenchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/bench-iterator.cpp)
add_executable(zip::benchmark ALIAS ZipBenchmark)
set_target_properties(ZipBenchmark
        PROPERTIES
            # zip::cxxstd isn't enough since INTERFACE_CXX_EXTENSIONS
            # is still broken:
            # https://gitlab.kitware.com/cmake/cmake/issues/17183
            CXX_EXTENSIONS OFF
            OUTPUT_NAME "zip-benchmark")
target_link_libraries(ZipBenchmark
    PRIVATE
        zip::cxxstd
        zip::warnings
        zip::zip)

if(ZIP_ENABLE_BENCHMARK)
    find_package(benchmark QUIET)
    if(NOT benchmark_FOUND)
        message(STATUS "Installing google benchmark via submodule")
        include(FnGitSubmoduleUpdate)
        git_submodule_update(vendor/benchmark)
        set(vendor_source_dir ${CMAKE_SOURCE_DIR}/vendor/benchmark)
        set(vendor_binary_dir ${CMAKE_BINARY_DIR}/vendor/benchmark)
        set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_ENABLE_LTO ${CMAKE_INTERPROCEDURAL_OPTIMIZATION} CACHE BOOL "" FORCE)
        # Add benchmark directly to our build. This defines
        # the 'benchmark' target.
        add_subdirectory(${vendor_source_dir} ${vendor_binary_dir} EXCLUDE_FROM_ALL)
        target_link_libraries(ZipBenchmark
            PRIVATE
                benchmark
                benchmark_main)
    endif()
else(ZIP_ENABLE_BENCHMARK)
    # Don't download Google Benchmark and skip target
    set_target_properties(ZipBenchmark
        PROPERTIES
            EXCLUDE_FROM_ALL ON)
endif(ZIP_ENABLE_BENCHMARK)

###########################################################
# Tooling
# Defines the following targets (only if the corresponding
# tool is found):
# - format-fix (run clang-format fixers in place on all sources)
# - tidy       (run clang-tidy on all sources)
# - tidy-fix   (run clang-tidy fixers in place on all sources)
###########################################################
include(FnGetTargetSources)
get_target_sources(_AllSources
    zip::zip
    zip::test
    zip::benchmark)

find_package(ClangFormat QUIET)
if(CLANG_FORMAT_FOUND)
    message(STATUS "clang-format: found ${CLANG_FORMAT_EXECUTABLE}")
    message(STATUS "clang-format: added custom target 'format-fix'")
    add_custom_target(format-fix
        COMMAND
            ${CLANG_FORMAT_EXECUTABLE} -style=file -i ${_AllSources}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif(CLANG_FORMAT_FOUND)

find_package(ClangTidy QUIET)
if(CLANG_TIDY_FOUND)
    message(STATUS "clang-tidy: found ${CLANG_TIDY_EXECUTABLE}")
    message(STATUS "clang-tidy: added custom targets 'tidy' and 'tidy-fix'")
    set(tidy_cmd
        ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_CURRENT_BINARY_DIR} -warnings-as-errors=*)
    add_custom_target(tidy
                      COMMAND ${tidy_cmd} ${_AllSources}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_custom_target(tidy-fix
                      COMMAND ${tidy_cmd} -fix ${_AllSources}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif(CLANG_TIDY_FOUND)

###########################################################
# Optimization reports
###########################################################

if(ZIP_ENABLE_OPTREPORT)
    include(FnAddOptReport)
    add_optreport(
        ZipOptreport
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/test/opt/Point1dIteratorSum.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/opt/Point1dZipSum.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/opt/Point2dSoaZipSum.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/opt/Point3dSoaZipSum.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/opt/Point4dSoaZipSum.cpp
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/opt
        COMPILER_FLAGS -O3 -DNDEBUG -std=c++17 -stdlib=libc++
        INCLUDES zip::zip
        ARCH skylake-avx512
        COMPILER_PASSES loop-vectorize)
endif(ZIP_ENABLE_OPTREPORT)
