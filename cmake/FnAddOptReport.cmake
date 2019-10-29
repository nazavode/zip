# This file is part of:
# https://github.com/nazavode/cmake-depot.git

# Intel:
# https://software.intel.com/en-us/articles/getting-the-most-out-of-your-intel-compiler-with-the-new-optimization-reports

include(FnGetTargetIncludeDirectories)

# set(OPTREPORT_DEFAULT_COMPILER         CACHE STRING "Default compiler used for optimization record generation when no COMPILER argument is provided")
set(OPTREPORT_DEFAULT_COMPILER_FLAGS  ${CMAKE_CXX_FLAGS_RELEASE} CACHE STRING "Default compiler flags used for optimization record generation when no COMPILER_FLAGS argument is provided")
set(OPTREPORT_DEFAULT_ARCH            x86-64 CACHE STRING "Default code generation architecture to be targeted when no ARCH argument is provided")
set(OPTREPORT_DEFAULT_PASS_FILTER     [a-z]* CACHE STRING "Default regular expression used by the compiler to select compiler passes to be included in the record output")

mark_as_advanced(
    OPTREPORT_DEFAULT_COMPILER_FLAGS
    OPTREPORT_DEFAULT_ARCH
    OPTREPORT_DEFAULT_PASS_FILTER)


function(get_optreport_flag OUTVAR)
    set(flags)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        list(APPEND flags "-fsave-optimization-record")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND flags "-ftree-vectorize")
        list(APPEND flags "-ftree-vectorizer-verbose=2")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        list(APPEND flags "-qopt-report")
    endif()
    set(${OUTVAR} ${flags} PARENT_SCOPE)
endfunction(get_optreport_flag OUTVAR)


function(get_optreport_outfile_flag OUTVAR DESTINATION)
    set(flags)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        list(APPEND flags "-foptimization-record-file='${REPORT_OUTFILE}'")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # TODO Does GNU has something for this?
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        list(APPEND flags "-qopt-report-file='${REPORT_OUTFILE}'")
    endif()
    set(${OUTVAR} ${flags} PARENT_SCOPE)
endfunction()


function(get_optreport_pass_filter_flag OUTVAR FILTER)
    set(flags)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        check_cxx_compiler_flag("-foptimization-record-passes='${FILTER}'" OPTREPORT_COMPILER_HAS_PASS_FILTER)
        if(OPTREPORT_COMPILER_HAS_PASS_FILTER)
            list(APPEND flags "-foptimization-record-passes='${FILTER}'")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # TODO Does GNU has something for this?
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        check_cxx_compiler_flag("-qopt-report-phase='${FILTER}'" OPTREPORT_COMPILER_HAS_PASS_FILTER)
        if(OPTREPORT_COMPILER_HAS_PASS_FILTER)
            list(APPEND flags "-qopt-report-phase='${FILTER}'")
        endif()
    endif()
    set(${OUTVAR} ${flags} PARENT_SCOPE)
endfunction(get_optreport_pass_filter_flag OUTVAR)


function(add_optimization_report TARGET)
    set(oneValueArgs
        DESTINATION
        ARCH
        COMPILER_PASSES)
    set(multiValueArgs
        SOURCES
        COMPILER_FLAGS
        INCLUDES)
    # Parse args:
    cmake_parse_arguments(PARSE_ARGV 1 arg 
        "${options}" "${oneValueArgs}" "${multiValueArgs}")
    # Set fallback values with defaults from cache:
    if(NOT arg_DESTINATION)
        set(arg_DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    if(NOT arg_ARCH)
        set(arg_ARCH ${OPTREPORT_DEFAULT_ARCH})
    endif()
    if(NOT arg_COMPILER_PASSES)
        set(arg_COMPILER_PASSES ${OPTREPORT_DEFAULT_PASS_FILTER})
    endif()
    if(NOT arg_COMPILER_FLAGS)
        set(arg_COMPILER_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
    endif()
    # Construct flags:
    set(flags "")
    # 1. Emit ASM
    list(APPEND flags -S -masm=intel)
    # 2. Target
    list(APPEND flags -march=${arg_ARCH} -mtune=${arg_ARCH})
    # 3. Additional flags
    list(APPEND flags ${arg_COMPILER_FLAGS})
    # 4. Actual optimization report generation
    get_optreport_flag(_flg)
    list(APPEND flags ${_flg})
    get_optreport_pass_filter_flag(_flg ${arg_COMPILER_PASSES})
    list(APPEND flags ${_flg})

    # Preprocess include directories to produce correct include flags:
    foreach(item ${arg_INCLUDES})
        if(TARGET ${item})
            # If the current item is a target, extract its
            # include directories:
            get_target_include_directories(tgt_includes ${item})
            foreach(_inc ${tgt_includes})
                list(APPEND flags "-I${_inc}")
            endforeach()
        else()
            list(APPEND flags "-I${dir}")
        endif()
    endforeach()

    # Make sure destination directory exists:
    file(MAKE_DIRECTORY ${arg_DESTINATION})

    # Generated file list, used to trigger dependencies:
    set(result)
    foreach(in_f ${arg_SOURCES})
        get_filename_component(basename ${in_f} NAME_WE)
        set(asm_out_f "${arg_DESTINATION}/${basename}.s")
        get_optreport_outfile_flag(optreport_out_f "${arg_DESTINATION}/${basename}.opt")
        set(command
            ${CMAKE_CXX_COMPILER} ${flags}
            ${optreport_out_f}
            -o ${asm_out_f} -c ${in_f})
        add_custom_command(
            OUTPUT ${optreport_out_f} ${asm_out_f}
            COMMAND ${command}
            DEPENDS ${in_f}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating optimization record for ${basename}"
            COMMAND_EXPAND_LISTS)
        list(APPEND result ${optrePORT_out_f} ${asm_out_f})
        # Emit generation log
        set(log_out_f "${arg_DESTINATION}/${basename}.opt.log")
        string(REPLACE ";" " " log_content "${command}")
        file(WRITE ${log_out_f} "${log_content}")
    endforeach()
    # Add phony target to trigger all the generated
    # custom commands:
    add_custom_target(${TARGET} ALL DEPENDS ${result})
endfunction()
