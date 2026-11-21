# Downloads a pre-built HTML export of EnergyPlus's Input Output Reference documentation
# and stages it for installation alongside the application, so doc links can point at a
# local copy instead of a live website.
#
# This is a stopgap: the export is produced manually today, not by a reproducible pipeline
# we control. Bump ENERGYPLUS_DOCS_VERSION/_URL/_EXPECTED_MD5 together whenever a new export
# is published for a newer bundled EnergyPlus version.

set(ENERGYPLUS_DOCS_VERSION "25.2.0")
set(ENERGYPLUS_DOCS_URL "https://drive.google.com/uc?export=download&id=1nUXQjdpX_AlCqA121Rnedr8edpS2lie5")
set(ENERGYPLUS_DOCS_EXPECTED_MD5 "f13e0a862e6a956f716d78def055a804")

set(ENERGYPLUS_DOCS_ARCHIVE_DIR "${PROJECT_BINARY_DIR}/EnergyPlusDocsArchive")
set(ENERGYPLUS_DOCS_ARCHIVE_NAME "EnergyPlus-docs-${ENERGYPLUS_DOCS_VERSION}.tar.gz")
set(ENERGYPLUS_DOCS_ARCHIVE_PATH "${ENERGYPLUS_DOCS_ARCHIVE_DIR}/${ENERGYPLUS_DOCS_ARCHIVE_NAME}")

# Where the extracted, installable HTML ends up. Kept as a cache variable so the rest of
# the build (the install() rule, and the .cxx.in path baked in for build-tree runs) can
# refer to it without re-deriving it.
set(ENERGYPLUS_DOCS_DIR "${PROJECT_BINARY_DIR}/EnergyPlus/doc/input-output-reference"
  CACHE PATH "Directory containing the extracted EnergyPlus Input Output Reference HTML" FORCE)

file(MAKE_DIRECTORY "${ENERGYPLUS_DOCS_ARCHIVE_DIR}")

set(ENERGYPLUS_DOCS_HASH "")
if(EXISTS "${ENERGYPLUS_DOCS_ARCHIVE_PATH}")
  file(MD5 "${ENERGYPLUS_DOCS_ARCHIVE_PATH}" ENERGYPLUS_DOCS_HASH)
endif()

set(ENERGYPLUS_DOCS_NEEDS_EXTRACT FALSE)

if(NOT EXISTS "${ENERGYPLUS_DOCS_ARCHIVE_PATH}" OR NOT "${ENERGYPLUS_DOCS_HASH}" MATCHES "${ENERGYPLUS_DOCS_EXPECTED_MD5}")
  if(NOT EXISTS "${ENERGYPLUS_DOCS_ARCHIVE_PATH}")
    message(STATUS "EnergyPlus docs archive doesn't exist at \"${ENERGYPLUS_DOCS_ARCHIVE_PATH}\"")
  else()
    message(STATUS
      "Existing EnergyPlus docs archive md5sum HASH mismatch\n"
      "     for file: ${ENERGYPLUS_DOCS_ARCHIVE_PATH}\n"
      "       expected hash: [${ENERGYPLUS_DOCS_EXPECTED_MD5}]\n"
      "         actual hash: [${ENERGYPLUS_DOCS_HASH}]\n"
    )
  endif()

  message(STATUS "Downloading EnergyPlus docs: ${ENERGYPLUS_DOCS_URL}")
  file(DOWNLOAD "${ENERGYPLUS_DOCS_URL}" "${ENERGYPLUS_DOCS_ARCHIVE_PATH}"
    SHOW_PROGRESS
    INACTIVITY_TIMEOUT 900 # 15-min timeout
    STATUS ENERGYPLUS_DOCS_DOWNLOAD_STATUS
  )
  list(GET ENERGYPLUS_DOCS_DOWNLOAD_STATUS 0 ENERGYPLUS_DOCS_DOWNLOAD_STATUS_CODE)
  list(GET ENERGYPLUS_DOCS_DOWNLOAD_STATUS 1 ENERGYPLUS_DOCS_DOWNLOAD_ERROR_MSG)

  if(ENERGYPLUS_DOCS_DOWNLOAD_STATUS_CODE)
    message(FATAL_ERROR
      "Download of EnergyPlus docs from ${ENERGYPLUS_DOCS_URL} failed: "
      "status code = ${ENERGYPLUS_DOCS_DOWNLOAD_STATUS_CODE}, message = ${ENERGYPLUS_DOCS_DOWNLOAD_ERROR_MSG}"
    )
  endif()

  file(MD5 "${ENERGYPLUS_DOCS_ARCHIVE_PATH}" ENERGYPLUS_DOCS_HASH)
  if(NOT "${ENERGYPLUS_DOCS_HASH}" MATCHES "${ENERGYPLUS_DOCS_EXPECTED_MD5}")
    message(FATAL_ERROR
      "Download of EnergyPlus docs seemed to have worked, but archive md5sum HASH mismatch\n"
      "     for file: ${ENERGYPLUS_DOCS_ARCHIVE_PATH}\n"
      "     from URL: ${ENERGYPLUS_DOCS_URL}\n"
      "       expected hash: [${ENERGYPLUS_DOCS_EXPECTED_MD5}]\n"
      "         actual hash: [${ENERGYPLUS_DOCS_HASH}]\n"
    )
  endif()

  message(STATUS "Download of EnergyPlus docs succeeded")
  set(ENERGYPLUS_DOCS_NEEDS_EXTRACT TRUE)
endif()

if(ENERGYPLUS_DOCS_NEEDS_EXTRACT OR NOT EXISTS "${ENERGYPLUS_DOCS_DIR}")
  if(EXISTS "${ENERGYPLUS_DOCS_DIR}")
    file(REMOVE_RECURSE "${ENERGYPLUS_DOCS_DIR}")
  endif()

  set(ENERGYPLUS_DOCS_EXTRACT_STAGING "${ENERGYPLUS_DOCS_ARCHIVE_DIR}/extracted")
  file(REMOVE_RECURSE "${ENERGYPLUS_DOCS_EXTRACT_STAGING}")
  file(MAKE_DIRECTORY "${ENERGYPLUS_DOCS_EXTRACT_STAGING}")

  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xfz "${ENERGYPLUS_DOCS_ARCHIVE_PATH}"
    WORKING_DIRECTORY "${ENERGYPLUS_DOCS_EXTRACT_STAGING}"
    RESULT_VARIABLE ENERGYPLUS_DOCS_EXTRACT_RESULT
  )
  if(NOT ENERGYPLUS_DOCS_EXTRACT_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to extract ${ENERGYPLUS_DOCS_ARCHIVE_PATH}")
  endif()

  # The archive wraps everything in a single top-level directory; find it regardless of name.
  file(GLOB ENERGYPLUS_DOCS_EXTRACTED_SUBDIRS LIST_DIRECTORIES TRUE "${ENERGYPLUS_DOCS_EXTRACT_STAGING}/*")
  list(LENGTH ENERGYPLUS_DOCS_EXTRACTED_SUBDIRS ENERGYPLUS_DOCS_EXTRACTED_SUBDIRS_COUNT)
  if(NOT ENERGYPLUS_DOCS_EXTRACTED_SUBDIRS_COUNT EQUAL 1)
    message(FATAL_ERROR "Expected exactly one top-level directory in ${ENERGYPLUS_DOCS_ARCHIVE_NAME}, found ${ENERGYPLUS_DOCS_EXTRACTED_SUBDIRS_COUNT}")
  endif()
  list(GET ENERGYPLUS_DOCS_EXTRACTED_SUBDIRS 0 ENERGYPLUS_DOCS_EXTRACTED_SUBDIR)

  # Drop macOS AppleDouble resource-fork files left over from how the export was archived.
  file(GLOB_RECURSE ENERGYPLUS_DOCS_APPLEDOUBLE_FILES "${ENERGYPLUS_DOCS_EXTRACTED_SUBDIR}/._*")
  if(ENERGYPLUS_DOCS_APPLEDOUBLE_FILES)
    file(REMOVE ${ENERGYPLUS_DOCS_APPLEDOUBLE_FILES})
  endif()

  get_filename_component(ENERGYPLUS_DOCS_DIR_PARENT "${ENERGYPLUS_DOCS_DIR}" DIRECTORY)
  file(MAKE_DIRECTORY "${ENERGYPLUS_DOCS_DIR_PARENT}")
  file(RENAME "${ENERGYPLUS_DOCS_EXTRACTED_SUBDIR}" "${ENERGYPLUS_DOCS_DIR}")
  file(REMOVE_RECURSE "${ENERGYPLUS_DOCS_EXTRACT_STAGING}")

  message(STATUS "EnergyPlus docs extracted to ${ENERGYPLUS_DOCS_DIR}")
endif()
