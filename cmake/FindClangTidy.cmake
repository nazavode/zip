#
# .rst: FindClangTidy
# ---------------
# cmake-format: off
#
# The module defines the following variables
#
# ``CLANG_TIDY_EXECUTABLE``
#   Path to clang-format executable
# ``CLANG_TIDY_FOUND``
#   True if the clang-format executable was found.
#
# Example usage:
#
# .. code-block:: cmake
#
#    find_package(ClangFormat)
#    if(CLANG_TIDY_FOUND)
#      message("clang-format executable found: ${CLANG_TIDY_EXECUTABLE}\n"
#              "version: ${CLANG_TIDY_VERSION}")
#    endif()
# cmake-format: on

find_program(CLANG_TIDY_EXECUTABLE
             HINTS "${ClangTidy_ROOT}"
             NAMES clang-tidy
                   clang-tidy-20
                   clang-tidy-19
                   clang-tidy-18
                   clang-tidy-17
                   clang-tidy-16
                   clang-tidy-15
                   clang-tidy-14
                   clang-tidy-13
                   clang-tidy-12
                   clang-tidy-11
                   clang-tidy-10
                   clang-tidy-9
                   clang-tidy-8
                   clang-tidy-7
                   clang-tidy-6
                   clang-tidy-5
                   clang-tidy-4
                   clang-tidy-3
             DOC "clang-tidy executable")
mark_as_advanced(CLANG_TIDY_EXECUTABLE)

if(CLANG_TIDY_EXECUTABLE)
  set(CLANG_TIDY_FOUND TRUE)
else()
  set(CLANG_TIDY_FOUND FALSE)
endif()
