#[=======================================================================[.rst:
CPackSignAndNotarizeDmg
-----------------------

This file is meant to be used up as a ``CPACK_POST_BUILD_SCRIPTS``

It will run only on ``APPLE`` when the generator is ``IFW`` to codesign the resulting .dmg and notarize it.

To do so, it uses the `CodeSigning`_ functions :cmake:command:`codesign_files_macos`

It requires that this be set: :cmake:variable:`CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION`

And it will only notarize if this is set: :cmake:variable:`CPACK_CODESIGNING_NOTARY_PROFILE_NAME`

#]=======================================================================]
message(STATUS "The message from ${CMAKE_CURRENT_LIST_FILE} and generator ${CPACK_GENERATOR}")
message(STATUS "Built packages: ${CPACK_PACKAGE_FILES}")

if(APPLE AND CPACK_GENERATOR STREQUAL "IFW")

  message(DEBUG "CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION=${CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION}")
  message(DEBUG "CPACK_CODESIGNING_NOTARY_PROFILE_NAME=${CPACK_CODESIGNING_NOTARY_PROFILE_NAME}")
  message(DEBUG "CPACK_IFW_PACKAGE_SIGNING_IDENTITY=${CPACK_IFW_PACKAGE_SIGNING_IDENTITY}")
  message(DEBUG "CPACK_CODESIGNING_MACOS_IDENTIFIER=${CPACK_CODESIGNING_MACOS_IDENTIFIER}")

  include(${CMAKE_CURRENT_LIST_DIR}/CodeSigning.cmake)

  if(NOT CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION)
    message(FATAL_ERROR "CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION is required, this should not have happened")
  endif()
  if(NOT CPACK_CODESIGNING_MACOS_IDENTIFIER)
    message(FATAL_ERROR "CPACK_CODESIGNING_MACOS_IDENTIFIER is required, this should not have happened")
  endif()

  # CPACK_PACKAGE_FILES can contain relative paths; resolve them to absolute so
  # EXISTS checks and codesign/notarize steps work regardless of cwd.
  set(_RESOLVED_PACKAGE_FILES "")
  foreach(CPACK_PACKAGE_FILE ${CPACK_PACKAGE_FILES})
    if(NOT IS_ABSOLUTE "${CPACK_PACKAGE_FILE}")
      set(CPACK_PACKAGE_FILE "${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_PACKAGE_FILE}")
    endif()
    if(NOT EXISTS "${CPACK_PACKAGE_FILE}")
      message(STATUS "File does not exist: ${CPACK_PACKAGE_FILE}")

      # wait in case file has not been written to disk yet
      execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 10)

      if(NOT EXISTS "${CPACK_PACKAGE_FILE}")
        message(FATAL_ERROR "File still does not exist: ${CPACK_PACKAGE_FILE}")
      endif()
    endif()
    list(APPEND _RESOLVED_PACKAGE_FILES "${CPACK_PACKAGE_FILE}")
  endforeach()

  codesign_files_macos(
    FILES ${_RESOLVED_PACKAGE_FILES}
    SIGNING_IDENTITY ${CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION}
    IDENTIFIER "${CPACK_CODESIGNING_MACOS_IDENTIFIER}.DmgInstaller"
    FORCE
    VERBOSE
  )

  if(CPACK_CODESIGNING_NOTARY_PROFILE_NAME)
    notarize_files_macos(
      FILES ${_RESOLVED_PACKAGE_FILES}
      NOTARY_PROFILE_NAME ${CPACK_CODESIGNING_NOTARY_PROFILE_NAME}
      STAPLE
      VERIFY
      VERBOSE
    )
  endif()

endif()
