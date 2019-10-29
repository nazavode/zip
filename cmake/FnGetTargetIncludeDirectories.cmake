function(get_target_include_directories _outvar _target)
    # Returns all the include directories
    # associated to the given targets.
    set(includes "")
    foreach(tgt ${_target} ${ARGN})
        get_target_property(tgt_type ${tgt} TYPE)
        set(properties
            INTERFACE_INCLUDE_DIRECTORIES
            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
        if(NOT ${tgt_type} STREQUAL "INTERFACE_LIBRARY")
            set(properties
                ${properties}
                INCLUDE_DIRECTORIES)
        endif()
        foreach(property ${properties})
            get_target_property(result ${tgt} ${property})
            if(NOT ("${result}" STREQUAL "result-NOTFOUND" OR "${result}" STREQUAL ""))
                list(APPEND includes ${result})
            endif()
        endforeach()
    endforeach()
    set(${_outvar} ${includes} PARENT_SCOPE)
endfunction(get_target_include_directories)