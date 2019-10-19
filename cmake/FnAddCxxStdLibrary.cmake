# * Creates an interface library target that sets the requested C++ standard level
#
# add_cxxstd_library(<target> <stdlevel>)
#
# This file is part of:
# https://github.com/nazavode/cmake-depot.git
#
# Copyright 2018 Federico Ficarelli
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# cmake-format: on

function(add_cxxstd_library TARGET CXXSTD)
    add_library(${TARGET} INTERFACE)
    set_property(TARGET ${TARGET} PROPERTY INTERFACE_CXX_EXTENSIONS OFF)
    if(cxx_std_${CXXSTD} IN_LIST CMAKE_CXX_COMPILE_FEATURES)
        set_property(
            TARGET ${TARGET}
            PROPERTY INTERFACE_COMPILE_FEATURES cxx_std_${CXXSTD})
    else()
        if(MSVC)
            set(_cxx_std_flag  /std:c++${CXXSTD})
        else()
            set(_cxx_std_flag -std=c++${CXXSTD})
        endif()
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag(${_cxx_std_flag} COMPILER_SUPPORTS_CXX_STD${CXXSTD}_FLAG)
        if(COMPILER_SUPPORTS_CXX_STD${CXXSTD}_FLAG)
            message(STATUS "CMake compile features not available for the current \
toolchain or CMake version, setting C++ standard level directly via compiler \
flags. Please be aware that setting flags doesn't check the actual C++${CXXSTD} \
standard support provided by the underlying toolchain, e.g.: build may fail \
on unsupported features.")
            set_property(
                TARGET ${TARGET}
                PROPERTY INTERFACE_COMPILE_OPTIONS ${_cxx_std_flag})
        else()
            message(WARNING "Compiler seems to be unable to accept '${_cxx_std_flag}', 
build will probably fail. Please set CMAKE_CXX_FLAGS to some sensible value for \
your toolchain.")
        endif()
    endif()
endfunction(add_cxxstd_library)