# get_sanitizer_flags(VAR): compute the flags needed to enable sanitizers and
# sets them to the output variable ${VAR}. Flags are computed according to the
# value of the global variable ZIP_USE_SANITIZER.

function(_append_common_sanitizer_flags VAR)
  string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)
  # Append -fno-omit-frame-pointer and turn on debug info to get better stack
  # traces.
  list(APPEND ${VAR} "-fno-omit-frame-pointer")
  if(NOT uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG"
     AND NOT uppercase_CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO"
     AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND ${VAR} "-gline-tables-only")
  endif()
endfunction()

function(get_sanitizer_flags VAR)
  if(NOT ZIP_USE_SANITIZER)
    return()
  endif()
  set(SANITIZER_FLAGS)
  _append_common_sanitizer_flags(SANITIZER_FLAGS)
  if(ZIP_USE_SANITIZER STREQUAL "Address")
    list(APPEND SANITIZER_FLAGS "-fsanitize=address")
  elseif(ZIP_USE_SANITIZER STREQUAL "Undefined")
    list(APPEND SANITIZER_FLAGS "-fsanitize=undefined")
  elseif(ZIP_USE_SANITIZER STREQUAL "Address;Undefined"
         OR ZIP_USE_SANITIZER STREQUAL "Undefined;Address")
    list(APPEND SANITIZER_FLAGS "-fsanitize=address,undefined")
  else()
    message(
      FATAL_ERROR "Unsupported value of ZIP_USE_SANITIZER: ${ZIP_USE_SANITIZER}"
    )
  endif()
  set(${VAR}
      ${SANITIZER_FLAGS}
      PARENT_SCOPE)
endfunction()
