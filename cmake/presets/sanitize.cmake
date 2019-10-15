# Enable Address SANitizer (ASAN) and Undefined Behaviour Sanitizer (UBSAN) for Debug build type.

set(sanitize "-fsanitize=address,undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls -O1 -g")

# Debug
set(CMAKE_C_FLAGS_DEBUG   "${sanitize}" CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG "${sanitize}" CACHE STRING "")

unset(sanitize)
