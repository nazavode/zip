# Auto-detect arch of the build machine and optimize/generate for it

set(opt "-O3 -DNDEBUG -march=native -mtune=native")

# Release
set(CMAKE_C_FLAGS_RELEASE   "${opt}" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "${opt}" CACHE STRING "")

# RelWithDebInfo
set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${opt} -g" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${opt} -g" CACHE STRING "")

unset(opt)