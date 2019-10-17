if(__optimization_record)
  return()
endif()
set(__optimization_record INCLUDED)

set(OPTRECORD_CXXFLAGS "-O3 -DNDEBUG -std=c++17" CACHE INTERNAL "")
set(OPTRECORD_ARCH "skylake-avx512" CACHE INTERNAL "")
set(OPTRECORD_PASSES "[a-z]*" CACHE INTERNAL "")

function(add_optimization_record out_var)
  set(result)
  foreach(in_f ${ARGN})
    # file(RELATIVE_PATH out_f ${CMAKE_CURRENT_SOURCE_DIR} ${in_f})
    get_filename_component(basename ${in_f} NAME_WE)
    set(asm_out_f "${CMAKE_CURRENT_BINARY_DIR}/${basename}.s")
    set(optrecord_out_f "${CMAKE_CURRENT_BINARY_DIR}/${basename}.opt.yaml")
    add_custom_command(OUTPUT ${optrecord_out_f}
      COMMAND ${CMAKE_CXX_COMPILER} -S ${OPTRECORD_CXXFLAGS} -march=${OPTRECORD_ARCH} -mtune=${OPTRECORD_ARCH} -fsave-optimization-record -foptimization-record-file=${optrecord_out_f} -o ${asm_out_f} -c ${in_f}
      DEPENDS ${in_f}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating optimization record for ${in_f}"
      VERBATIM
      )
    list(APPEND result ${optrecord_out_f})
  endforeach()
  set(${out_var} "${result}" PARENT_SCOPE)
endfunction()
