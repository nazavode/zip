set(OPTRECORD_DEFAULT_COMPILER        ${CMAKE_CXX_COMPILER} CACHE STRING "Default compiler used for optimization record generation when no COMPILER argument is provided")
set(OPTRECORD_DEFAULT_COMPILER_FLAGS  -O3;-DNDEBUG CACHE STRING "Default compiler flags used for optimization record generation when no COMPILER_FLAGS argument is provided")
set(OPTRECORD_DEFAULT_ARCH            x86-64 CACHE STRING "Default code generation architecture to be targeted when no ARCH argument is provided")
set(OPTRECORD_DEFAULT_COMPILER_PASSES [a-z]* CACHE STRING "Default regular expression used by the compiler to select compiler passes to be included in the record output")

function(add_optimization_record TARGET)
    set(oneValueArgs
        COMPILER
        DESTINATION
        ARCH
        COMPILER_PASSES)
    set(multiValueArgs
        SOURCES
        COMPILER_FLAGS
        INCLUDE_DIRECTORIES)
    # Parse args:
    cmake_parse_arguments(PARSE_ARGV 1 arg 
        "${options}" "${oneValueArgs}" "${multiValueArgs}")
    # Set fallback values with defaults from cache:
    if(NOT arg_COMPILER)
        set(arg_COMPILER ${OPTRECORD_DEFAULT_COMPILER})
    endif()
    if(NOT arg_COMPILER_FLAGS)
        set(arg_COMPILER_FLAGS ${OPTRECORD_DEFAULT_COMPILER_FLAGS})
    endif()
    if(NOT arg_ARCH)
        set(arg_ARCH ${OPTRECORD_DEFAULT_ARCH})
    endif()
    if(NOT arg_COMPILER_PASSES)
        set(arg_COMPILER_PASSES ${OPTRECORD_DEFAULT_COMPILER_PASSES})
    endif()
    # Make sure destination directory exists:
    file(MAKE_DIRECTORY ${arg_DESTINATION})
    # Preprocess include directories to produce correct include flags:
    set(includes "")
    foreach(dir ${arg_INCLUDE_DIRECTORIES})
        list(APPEND includes "-I${dir}")
    endforeach()
    # Generated file list, use to trigger dependencies:
    set(result)
    foreach(in_f ${arg_SOURCES})
        get_filename_component(basename ${in_f} NAME_WE)
        set(asm_out_f "${arg_DESTINATION}/${basename}.s")
        set(optrecord_out_f "${arg_DESTINATION}/${basename}.opt.yaml")
        set(command
            ${arg_COMPILER} ${arg_COMPILER_FLAGS} -S
            ${includes}
            -march=${arg_ARCH}
            -mtune=${arg_ARCH}
            -fsave-optimization-record
            -foptimization-record-passes='${arg_COMPILER_PASSES}'
            -foptimization-record-file=${optrecord_out_f}
            -o ${asm_out_f} -c ${in_f})
        add_custom_command(
            OUTPUT ${optrecord_out_f} ${asm_out_f}
            COMMAND ${command}
            DEPENDS ${in_f}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating optimization record for ${basename}"
            COMMAND_EXPAND_LISTS)
        list(APPEND result ${optrecord_out_f} ${asm_out_f})
        # Emit generation log
        set(log_out_f "${arg_DESTINATION}/${basename}.opt.log")
        string(REPLACE ";" " " log_content "${command}")
        file(WRITE ${log_out_f} "${log_content}")
    endforeach()
    # Add phony target to trigger all the generated
    # custom commands:
    add_custom_target(${TARGET} ALL DEPENDS ${result})
endfunction()
