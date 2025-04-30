#
# .rst: FindFileCheck
# ---------------
# cmake-format: off
#
# The module defines the following variables
#
# ``FILECHECK_EXECUTABLE``
#   Path to FileCheck executable
# ``FILECHECK_FOUND``
#   True if the FileCheck executable was found.
#
# Example usage:
#
# .. code-block:: cmake
#
#    find_package(FileCheck)
#    if(FILECHECK_FOUND)
#      message("FileCheck executable found: ${FILECHECK_EXECUTABLE}")
#    endif()
# cmake-format: on

find_program(FILECHECK_EXECUTABLE
             HINTS "${FileCheck_ROOT}"
             NAMES filecheck
                   FileCheck
                   FileCheck-20
                   FileCheck-19
                   FileCheck-18
                   FileCheck-17
                   FileCheck-16
                   FileCheck-15
                   FileCheck-14
                   FileCheck-13
                   FileCheck-12
                   FileCheck-11
                   FileCheck-10
                   FileCheck-9
                   FileCheck-8
                   FileCheck-7
                   FileCheck-6
                   FileCheck-5
                   FileCheck-4
                   FileCheck-3
             DOC "FileCheck executable")
mark_as_advanced(FILECHECK_EXECUTABLE)

if(FILECHECK_EXECUTABLE)
  set(FILECHECK_FOUND TRUE)
else()
  set(FILECHECK_FOUND FALSE)
endif()
