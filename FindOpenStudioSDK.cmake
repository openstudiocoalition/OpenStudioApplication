set(OPENSTUDIO_VERSION_MAJOR 3)
set(OPENSTUDIO_VERSION_MINOR 3)
set(OPENSTUDIO_VERSION_PATCH 0)
set(OPENSTUDIO_VERSION "${OPENSTUDIO_VERSION_MAJOR}.${OPENSTUDIO_VERSION_MINOR}.${OPENSTUDIO_VERSION_PATCH}")

#If this is a release enter the SHA as "+79857912c4"
#set(OPENSTUDIO_VERSION_SHA "+09b7c8a554")
#If this is a pre-release enter the pre-release and SHA as "-rc1+79857912c4"
set(OPENSTUDIO_VERSION_SHA "-alpha+d92b642abb")

# Paths where the cmake-downloaded archives will be put
set(OPENSTUDIO_ARCHIVE_DIR "${PROJECT_BINARY_DIR}/OpenStudio-${OPENSTUDIO_VERSION}")

# If downloaded, we need the SHA to match. This block is here since we need "OPENSTUDIO_PLATFORM" anyways
if(APPLE)
  set(OPENSTUDIO_EXPECTED_HASH f231e3c3225fa21fe41ceef437665d72)
  set(OPENSTUDIO_PLATFORM "Darwin")
  set(OPENSTUDIO_EXT "tar.gz")
elseif(UNIX)

  if(LSB_RELEASE_VERSION_SHORT MATCHES "20.04")
    set(OPENSTUDIO_EXPECTED_HASH c69e28c7893a765324a431ad71965611)
    set(OPENSTUDIO_PLATFORM "Ubuntu-20.04")
  else() # Assumes 18.04
    set(OPENSTUDIO_EXPECTED_HASH c9958dd6e0a612480fd6b562497ca2ea)
    set(OPENSTUDIO_PLATFORM "Ubuntu-18.04")
  endif()
  set(OPENSTUDIO_EXT "tar.gz")

elseif(WIN32)
  set(OPENSTUDIO_EXPECTED_HASH 42bdcf13b06d6b140c7b42d2101bc710) # TODO: OpenStudio's Jenkins windows CI instance has been stuck for 3 days
  set(OPENSTUDIO_PLATFORM "Windows")
  set(OPENSTUDIO_EXT "tar.gz")
endif()


set(OPENSTUDIO_ARCHIVE_BASENAME "OpenStudio-${OPENSTUDIO_VERSION}${OPENSTUDIO_VERSION_SHA}-${OPENSTUDIO_PLATFORM}")
set(OPENSTUDIO_ARCHIVE_NAME "${OPENSTUDIO_ARCHIVE_BASENAME}.${OPENSTUDIO_EXT}"
  CACHE STRING "Archive Name, with extension" FORCE)

# See if we can find the openstudio package with the right short version (eg 3.1.0) directly
# (either installed and in PATH, or if user supplied openstudio_DIR in the cmake command)
find_package(openstudio "${OPENSTUDIO_VERSION}" CONFIG
  PATHS
    "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_BASENAME}"
  HINTS
    "${OPENSTUDIO_ARCHIVE_DIR}"
  NO_DEFAULT_PATH
  NO_PACKAGE_ROOT_PATH
)
if(openstudio_FOUND)
  message("Found specified openstudio at openstudio_DIR=${openstudio_DIR}")
else()

  # Not found: no problem, we download it
  # base link for release builds
  set(OPENSTUDIO_BASELINK_RELEASE
    #"https://openstudio-builds.s3.amazonaws.com/${OPENSTUDIO_VERSION}"
    "https://github.com/NREL/OpenStudio/releases/download/v${OPENSTUDIO_VERSION}/"
    CACHE STRING "Base link to where the openstudio archives are hosted" FORCE)

  # base link for develop builds. (Using https will fail)
  # Note: this should be set to ""http://openstudio-ci-builds.s3-website-us-west-2.amazonaws.com/develop" for nightly builds
  # Occasionally we can point to a specific PR by using something like ""http://openstudio-ci-builds.s3-website-us-west-2.amazonaws.com/PR-4080"
  set(OPENSTUDIO_BASELINK_CI
    "http://openstudio-ci-builds.s3-website-us-west-2.amazonaws.com/develop"
    #"http://openstudio-ci-builds.s3-website-us-west-2.amazonaws.com/PR-4121"
    CACHE STRING "Base link to where the openstudio develop archives are hosted" FORCE)

  # Make subdir if it doesn't exist
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${OPENSTUDIO_ARCHIVE_DIR}/")

  # First pass: if the archive exists, compute the MD5
  if(EXISTS "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}")
    file(MD5 "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}" OPENSTUDIO_HASH)
  endif()

  # If the archive doesn't exist, or the archive has the wrong MD5, then resort to download
  if(NOT EXISTS "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}" OR NOT "${OPENSTUDIO_HASH}" MATCHES "${OPENSTUDIO_EXPECTED_HASH}")

    if(NOT EXISTS "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}")
      message(STATUS "Archive doesn't exist at \"${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}\"")
    else()
      message(STATUS
        "Existing archive md5sum HASH mismatch\n"
        "     for file: ${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}\n"
        "       expected hash: [${OPENSTUDIO_EXPECTED_HASH}]\n"
        "         actual hash: [${OPENSTUDIO_HASH}]\n"
      )
    endif()

    # Remove the old extracted dir if exists
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_BASENAME}")

    # Try with the official releases first, then fall back to the CI (develop) nightly builds
    foreach(BASELINK IN LISTS OPENSTUDIO_BASELINK_RELEASE OPENSTUDIO_BASELINK_CI)
      set(OPENSTUDIO_URL "${BASELINK}/${OPENSTUDIO_ARCHIVE_NAME}")

      if(BASELINK MATCHES "amazonaws")
        string(REPLACE "+" "%2B" OPENSTUDIO_URL ${OPENSTUDIO_URL})
      endif()

      message(STATUS "Try Downloading OpenStudio SDK: ${OPENSTUDIO_URL}")

      # Cannot use EXPECTED_MD5 here, or it'll throw an error, which we do not want.
      file(DOWNLOAD "${OPENSTUDIO_URL}" "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}"
        SHOW_PROGRESS
        INACTIVITY_TIMEOUT 900 # 15-min timeout
        STATUS DOWNLOAD_STATUS
      )
    list(GET DOWNLOAD_STATUS 0 DOWNLOAD_STATUS_CODE)
    list(GET DOWNLOAD_STATUS 1 DOWNLOAD_ERROR_MSG)

      # If download didn't work
      if (DOWNLOAD_STATUS_CODE)
        message(STATUS "Download failed status code = ${DOWNLOAD_STATUS_CODE}, message = ${DOWNLOAD_ERROR_MSG}")
        if(DOWNLOAD_STATUS_CODE EQUAL 28)
          message(FATAL_ERROR
            "Download from ${OPENSTUDIO_URL} has failed apparently due to timeout. Increase timeout or manually download the archive in ${OPENSTUDIO_ARCHIVE_DIR}"
          )
        endif()
      else()
        # If download at least appears to have worked
        if(EXISTS "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}")
          file(MD5 "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}" OPENSTUDIO_HASH)
          if ( "${OPENSTUDIO_HASH}" MATCHES "${OPENSTUDIO_EXPECTED_HASH}")
            # If it worked, break!
            message(STATUS "Download OpenStudio SDK is a success, from: ${OPENSTUDIO_URL}")
            break()
          else()
            set(ERROR_MSG
              "Download seemed to have worked, but archive md5sum HASH mismatch\n"
              "     for file: ${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}\n"
              "     from URL: ${OPENSTUDIO_URL}\n"
              "       expected hash: [${OPENSTUDIO_EXPECTED_HASH}]\n"
              "         actual hash: [${OPENSTUDIO_HASH}]\n"
            )
            # If official => FATAL_ERROR. If CI build, it's possibe the package is overriden by nightly build if the SHA of develop is the same as the
            # day before, so only warn
            if (BASELINK STREQUAL OPENSTUDIO_BASELINK_RELEASE)
              message(FATAL_ERROR "${ERROR_MSG}")
            else()
              message(AUTHOR_WARNING "${ERROR_MSG}")
            endif()
          endif()
        endif()
      endif()
    endforeach()

  endif()

  # Allows placing the archive manually in the build dir too
  # If the Extracted directory doesn't exist yet, unset the cache and extract the archive
  if (NOT EXISTS "${OPENSTUDIO_ARCHIVE_BASENAME}")
    unset(OPENSTUDIO_FOUND CACHE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_NAME}" WORKING_DIRECTORY "${OPENSTUDIO_ARCHIVE_DIR}")
  endif()

  # Now we do a REQUIRED check, which will throw if not found
  if(UNIX AND NOT APPLE)
    find_package(openstudio "${OPENSTUDIO_VERSION}" CONFIG REQUIRED
      PATHS "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_BASENAME}/usr/local/"
      NO_CMAKE_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH
    )
  else()
    find_package(openstudio "${OPENSTUDIO_VERSION}" CONFIG REQUIRED
      PATHS "${OPENSTUDIO_ARCHIVE_DIR}/${OPENSTUDIO_ARCHIVE_BASENAME}"
      NO_CMAKE_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH
    )
  endif()

endif()

get_filename_component(openstudio_ROOT_DIR "${openstudio_DIR}/../../../" ABSOLUTE)
set(openstudio_ROOT_DIR "${openstudio_ROOT_DIR}" CACHE STRING "This is the path to the root of SDK, under which you'll find bin/, lib/, etc" FORCE)

# Do an extra check?
find_program(openstudio_EXECUTABLE
  NAMES openstudio
  PATHS "${openstudio_ROOT_DIR}/bin/"
  NO_CMAKE_PATH
  NO_CMAKE_ENVIRONMENT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
)

if (NOT EXISTS "${openstudio_EXECUTABLE}")
  message(AUTHOR_WARNING "Could not find the CLI at ${openstudio_EXECUTABLE}")
  set_property (CACHE openstudio_EXECUTABLE PROPERTY VALUE "openstudio_EXECUTABLE-NOTFOUND")
else()
  # Check the version it returns
  # executable found must have a specific version
  execute_process (COMMAND "${openstudio_EXECUTABLE}" "openstudio_version"
    RESULT_VARIABLE CLI_RESULT
    OUTPUT_VARIABLE CLI_VERSION
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if (CLI_RESULT)
    message(AUTHOR_WARNING "Cannot use the openstudio CLI at \"${openstudio_EXECUTABLE}\"")
  elseif(NOT CLI_VERSION STREQUAL "${OPENSTUDIO_VERSION}${OPENSTUDIO_VERSION_SHA}")
    execute_process (COMMAND "${openstudio_EXECUTABLE}" -e "puts OpenStudio::openStudioVersion"
      RESULT_VARIABLE CLI_RESULT
      OUTPUT_VARIABLE CLI_SHORT_VERSION
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (CLI_SHORT_VERSION VERSION_EQUAL OPENSTUDIO_VERSION)
      message(AUTHOR_WARNING
        "CLI has the same short version (${OPENSTUDIO_VERSION}) but not the same long version."
        "It has \"${CLI_VERSION}\" versus expected \"${OPENSTUDIO_VERSION}${OPENSTUDIO_VERSION_SHA}\""
      )
    else()
      message(WARNING
        "CLI has a wrong version: \"${CLI_VERSION}\" versus expected \"${OPENSTUDIO_VERSION}${OPENSTUDIO_VERSION_SHA}\""
      )
    endif()
  else()
    message(STATUS
      "Found openstudio CLI with the exact expected long version:\n"
      "  version: ${CLI_VERSION}\n"
      "  openstudio_EXECUTABLE: \"${openstudio_EXECUTABLE}\"\n"
    )
  endif()
endif()
