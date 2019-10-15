cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(zip VERSION 0.1 LANGUAGES CXX)

option(ZIP_ENABLE_TEST "Enable building unit tests" ON)
option(ZIP_ENABLE_BENCHMARK "Enable building micro-benchmarks" OFF)

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
add_library(_Cxxstd INTERFACE)
add_library(zip::cxxstd ALIAS _Cxxstd)
# TODO still not sure about switching off language extensions on clients
set_property(TARGET _Cxxstd PROPERTY INTERFACE_CXX_EXTENSIONS OFF)
if(cxx_std_17 IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set_property(TARGET _Cxxstd PROPERTY INTERFACE_COMPILE_FEATURES cxx_std_17)
else()
    set(_cxx_std_flag -std=c++17)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(${_cxx_std_flag} COMPILER_SUPPORTS_CXX_STD17_FLAG)
    if(COMPILER_SUPPORTS_CXX_STD17_FLAG)
        message(STATUS "CMake compile features not available for the current \
toolchain or CMake version, setting C++ standard level directly via compiler \
flags. Please be aware that setting flags doesn't check the actual C++17 \
standard support provided by the underlying toolchain, e.g.: build may fail \
on unsupported features.")
        set_property(TARGET _Cxxstd PROPERTY INTERFACE_COMPILE_OPTIONS ${_cxx_std_flag})
    else()
        message(WARNING "Compiler seems to be unable to accept '${_cxx_std_flag}',
build will probably fail. Please set CMAKE_CXX_FLAGS to some sensible value for \
your toolchain.")
    endif()
endif()

###########################################################
# Compiler warnings
# Defines target 'zip::warnings'
###########################################################
add_library(_Warnings INTERFACE)
add_library(zip::warnings ALIAS _Warnings)
set_property(TARGET _Warnings
             PROPERTY INTERFACE_COMPILE_OPTIONS
                      -Wall
                      -Wextra
                      -Werror)

###########################################################
# Actual library
# Defines target 'zip::zip'
###########################################################

add_library(_Zip INTERFACE)
add_library(zip::zip ALIAS _Zip)

target_sources(_Zip
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/zip.h)

target_include_directories(_Zip
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(_Zip
    INTERFACE
        zip::cxxstd
        zip::warnings)

###########################################################
# Unit tests
# Defines target 'zip::test'
###########################################################
add_library(_Catch2 INTERFACE)
add_library(zip::catch2 ALIAS _Catch2)
target_sources(_Catch2
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/test/catch.hpp)
target_compile_definitions(_Catch2
    INTERFACE
        # Defer STATIC_REQUIRE failure/success to runtime
        CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
target_include_directories(_Catch2
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/test)

add_executable(_Test
    ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/sanitizer-options.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test-zip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test-iterator.cpp)
add_executable(zip::test ALIAS _Test)
set_target_properties(_Test
    PROPERTIES
        # zip::cxxstd isn't enough since INTERFACE_CXX_EXTENSIONS
        # is still broken:
        # https://gitlab.kitware.com/cmake/cmake/issues/17183
        CXX_EXTENSIONS OFF
        OUTPUT_NAME "zip-test")
target_link_libraries(_Test
    PUBLIC
        zip::catch2
        zip::zip)
    
if(ZIP_ENABLE_TEST)
    enable_testing()
    add_test(NAME zip-test COMMAND _Tests)
endif()

###########################################################
# Micro-benchmarks
# Defines target 'zip::benchmark'
###########################################################
add_executable(_Benchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/bench-iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/main.cpp)
add_executable(zip::benchmark ALIAS _Benchmark)
set_target_properties(_Benchmark
        PROPERTIES
            # zip::cxxstd isn't enough since INTERFACE_CXX_EXTENSIONS
            # is still broken:
            # https://gitlab.kitware.com/cmake/cmake/issues/17183
            CXX_EXTENSIONS OFF
            OUTPUT_NAME "zip-benchmark")
target_link_libraries(_Benchmark
    PUBLIC
        zip::zip)
if(ZIP_ENABLE_BENCHMARK)
find_package(benchmark QUIET)
if(NOT benchmark_FOUND)
    message(STATUS "Installing google benchmark via submodule")
    execute_process(COMMAND git submodule update --init -- external/benchmark
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    set(vendor_source_dir ${CMAKE_SOURCE_DIR}/external/benchmark)
    set(vendor_binary_dir ${CMAKE_BINARY_DIR}/external/benchmark)

    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_LTO ${CMAKE_INTERPROCEDURAL_OPTIMIZATION} CACHE BOOL "" FORCE)

    # Add benchmark directly to our build. This defines
    # the 'benchmark' target.
    add_subdirectory(${vendor_source_dir} ${vendor_binary_dir} EXCLUDE_FROM_ALL)

    target_link_libraries(_Benchmark
    PUBLIC
        benchmark)
endif()

else()
    # Don't download Google Benchmark and skip target
    set_target_properties(_Benchmark
        PROPERTIES
            EXCLUDE_FROM_ALL ON)
endif()

###########################################################
# Tooling
# Defines the following targets (only if the corresponding
# tool is found):
# - format-fix (run clang-format fixers in place on all sources)
# - tidy       (run clang-tidy on all sources)
# - tidy-fix   (run clang-tidy fixers in place on all sources)
###########################################################
function(get_target_sources _outvar _target)
    # Returns all the source files
    # (including headers) associated to the given
    # targets.
    set(sources "")
    foreach(tgt ${_target} ${ARGN})
        get_target_property(tgt_type ${tgt} TYPE)
        set(properties
            INTERFACE_SOURCES
            INTERFACE_HEADER)
        if(NOT ${tgt_type} STREQUAL "INTERFACE_LIBRARY")
            set(properties
                ${properties}
                SOURCES
                PUBLIC_HEADER
                PRIVATE_HEADER)
        endif()
        foreach(property ${properties})
            get_target_property(result ${tgt} ${property})
            if(NOT ("${result}" STREQUAL "result-NOTFOUND" OR "${result}" STREQUAL ""))
                list(APPEND sources ${result})
            endif()
        endforeach()
    endforeach()
    set(${_outvar} ${sources} PARENT_SCOPE)
endfunction(get_target_sources)

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
endif()

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
endif()
