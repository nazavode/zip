cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(zipit VERSION 0.1 LANGUAGES CXX)

option(ZIPIT_ENABLE_TEST "Enable unit tests" ON)

# Add extra finders
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Always enable generation of compilation database to
# make clang tooling and vscode happy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Generate compile commands database" FORCE)

###########################################################
# C++ standard level
# Defines target 'zipit::cxxstd'
###########################################################
add_library(_Cxxstd INTERFACE)
add_library(zipit::cxxstd ALIAS _Cxxstd)
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
# Defines target 'zipit::warnings'
###########################################################
add_library(_Warnings INTERFACE)
add_library(zipit::warnings ALIAS _Warnings)
set_property(TARGET _Warnings
             PROPERTY INTERFACE_COMPILE_OPTIONS
                      -Wall
                      -Wextra
                      -Werror)

###########################################################
# Actual library
# Defines target 'zipit::zipit'
###########################################################

add_library(_Zipit INTERFACE)
add_library(zipit::zipit ALIAS _Zipit)

target_sources(_Zipit
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/zipit.h)

target_include_directories(_Zipit
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(_Zipit
    INTERFACE
        zipit::cxxstd
        zipit::warnings)

###########################################################
# Unit tests
# Defines target 'zipit::test'
###########################################################

add_library(_Catch2 INTERFACE)
add_library(zipit::catch2 ALIAS _Catch2)
target_sources(_Catch2
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/test/catch.hpp)
target_include_directories(_Catch2
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/test)

add_executable(_Test
    ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test-zipit.cpp)
add_executable(zipit::test ALIAS _Test)
set_target_properties(_Test
    PROPERTIES
        # zipit::cxxstd isn't enough since INTERFACE_CXX_EXTENSIONS
        # is still broken:
        # https://gitlab.kitware.com/cmake/cmake/issues/17183
        CXX_EXTENSIONS OFF
        OUTPUT_NAME "zipit-test")
target_link_libraries(_Test
    PUBLIC
        zipit::catch2
        zipit::zipit)
    
if(ZIPIT_ENABLE_TEST)
    enable_testing()
    add_test(NAME zipit-test COMMAND _Tests)
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
    zipit::zipit
    zipit::test)

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

###########################################################
# Sanitizers
# Defines the following additional build types:
# - ASAN
# - TSAN
# - UBSAN
###########################################################

function(add_build_type NAME)
    string(REPLACE ";" " " flags "${ARGN}")
    set(CMAKE_C_FLAGS_${NAME}
        ${flags}
        CACHE STRING "Flags used by the C compiler during ${NAME} builds."
        FORCE)
    set(CMAKE_CXX_FLAGS_${NAME}
        ${flags}
        CACHE STRING "Flags used by the C++ compiler during ${NAME} builds."
        FORCE)    
endfunction(add_build_type)

add_build_type(TSAN
    -fsanitize=thread -g -O1)

add_build_type(ASAN
    -fsanitize=address -fno-optimize-sibling-calls
    -fsanitize-address-use-after-scope
    -fno-omit-frame-pointer -g -O1)

add_build_type(UBSAN
    -fsanitize=undefined -g -O1)