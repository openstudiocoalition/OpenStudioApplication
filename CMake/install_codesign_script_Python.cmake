#[=======================================================================[.rst:
install_codesign_script_Python
------------------------------

This file is meant to be used up as a ``install(SCRIPT)``

Pre-conditions:

- It requires that you're running on ``APPLE``

- **Important: In the scope of this** ``install(SCRIPT)``, you must define

    * :cmake:variable:`CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION`
    * :cmake:variable:`CPACK_CODESIGNING_MACOS_IDENTIFIER`
    * You need to also set a variable ``FILES_TO_SIGN``

**In the scope** meaning that you have to issue some ``install(CODE ..)`` commands prior to calling it, and they must be:

    * Part of the same ``project()``
    * Using the same CPack ``COMPONENT``

This script will codesign the Python/ sos which are globbed.

To do so, it uses the `CodeSigning`_ functions :cmake:command:`codesign_files_macos`

This script will therefore run in the CPack staging area, and should be added after installation of root dylibs (they may come from fixup_executable)
after any rpath adjustments, to ensure the signature sticks.

Usage::

  if(APPLE AND CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION)
    install(CODE "set(CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION \"${CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION}\")" COMPONENT Python)
    install(CODE "set(CPACK_CODESIGNING_MACOS_IDENTIFIER \"${CPACK_CODESIGNING_MACOS_IDENTIFIER}\")" COMPONENT Python)
    # call the script
    install(SCRIPT "${CMAKE_CURRENT_LIST_DIR}/install_codesign_script_Python.cmake" COMPONENT Python)
  endif()
#]=======================================================================]

#------------------------------------------------------------------------------
# Just a helper
function(print_relative_paths)
  set(prefix "")
  set(valueLessKeywords NAME_ONLY NEWLINE)
  set(singleValueKeywords PREFIX BASE_DIRECTORY)
  set(multiValueKeywords ABSOLUTE_PATHS)
  cmake_parse_arguments(
    PARSE_ARGV 0 # Start at one with NAME is the first param
      "${prefix}"
      "${valueLessKeywords}"
      "${singleValueKeywords}"
      "${multiValueKeywords}"
  )

  if (_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Extra unknown arguments were passed: ${_UNPARSED_ARGUMENTS}")
  endif()
  if (_KEYWORDS_MISSING_VALUES)
    message(FATAL_ERROR "Keywords missing values: ${_KEYWORDS_MISSING_VALUES}")
  endif()

  if(NOT _BASE_DIRECTORY)
    set(_BASE_DIRECTORY ${CMAKE_INSTALL_PREFIX})
  endif()

  foreach(path ${_ABSOLUTE_PATHS})
    if(_NAME_ONLY)
      cmake_path(GET path FILENAME rel_path)
    else()
      cmake_path(RELATIVE_PATH path BASE_DIRECTORY ${_BASE_DIRECTORY} OUTPUT_VARIABLE rel_path)
    endif()
    list(APPEND rel_paths ${rel_path})
  endforeach()

  if(_NEWLINE)
    message("${_PREFIX}")
    foreach(path ${rel_paths})
      message("   - ${path}")
    endforeach()
  else()
    message("${_PREFIX}${rel_paths}")
  endif()
endfunction()
#------------------------------------------------------------------------------

message("Codesigning inner executables and library from ${CMAKE_CURRENT_LIST_FILE}")

message("CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
message("CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION=${CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION}")
message("CPACK_CODESIGNING_MACOS_IDENTIFIER=${CPACK_CODESIGNING_MACOS_IDENTIFIER}")
message("CMAKE_INSTALL_COMPONENT=${CMAKE_INSTALL_COMPONENT}")
message("CPACK_GENERATOR=${CPACK_GENERATOR}")

if(NOT CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION)
  message(FATAL_ERROR "CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION is required")
endif()

if(NOT CPACK_CODESIGNING_MACOS_IDENTIFIER)
  message(FATAL_ERROR "CPACK_CODESIGNING_MACOS_IDENTIFIER is required")
endif()

file(GLOB _PYTHON_OS_SOS "${CMAKE_INSTALL_PREFIX}/Python/*.so")
print_relative_paths(PREFIX "PYTHON_OS_SOS=" ABSOLUTE_PATHS ${_PYTHON_OS_SOS})

include(${CMAKE_CURRENT_LIST_DIR}/CodeSigning.cmake)
codesign_files_macos(
  FILES ${_PYTHON_OS_SOS}
  SIGNING_IDENTITY ${CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION}
  PREFIX "${CPACK_CODESIGNING_MACOS_IDENTIFIER}."
  FORCE VERBOSE
)

# Clean up to avoid multiple passes (several components) appending to a pre-existing list
unset(_PYTHON_OS_SOS)

message("Finished Codesigning inner executables and library for CPack Component Python")
