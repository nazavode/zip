# Auto-detect arch of the build machine and optimize/generate for it

set(arch "-march=native -mtune=native")

# Release
set(CMAKE_C_FLAGS_RELEASE   "${arch} -O3 -DNDEBUG"                CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "${arch} -O3 -DNDEBUG -stdlib=libc++" CACHE STRING "")

# RelWithDebInfo
set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${arch} -g -O2 -DNDEBUG"                CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${arch} -g -O2 -DNDEBUG -stdlib=libc++" CACHE STRING "")

# Debug
set(CMAKE_C_FLAGS_DEBUG   "${arch} -g -O0"                CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG "${arch} -g -O0 -stdlib=libc++" CACHE STRING "")

unset(arch)