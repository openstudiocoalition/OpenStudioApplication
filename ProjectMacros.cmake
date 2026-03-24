include(CMakeParseArguments)

if(NOT USE_PCH)
  macro(AddPCH TARGET_NAME)
  endmacro()
endif()

# Add google tests macro
macro(ADD_GOOGLE_TESTS executable)
  if(MSVC)
    # QT-Separation-Move
    file(TO_NATIVE_PATH "${QT_INSTALL_DIR}/bin/" QT_BIN_PATH) # DLM:
    file(TO_NATIVE_PATH "${OPENSSL_ROOT_DIR}/bin/" OPENSSL_BIN_PATH)
    string(REGEX REPLACE "([^\\]);" "\\1\\\\;" CURRENT_ENV "$ENV{PATH}")
    set(NEWPATH "${QT_BIN_PATH};${OPENSSL_BIN_PATH};${CURRENT_ENV}")
  else()
    set(NEWPATH $ENV{PATH})
  endif()

  foreach(source ${ARGN})
    if(NOT "${source}" MATCHES "/moc_.*cxx")
      string(REGEX MATCH .*cpp source "${source}")
      if(source)
        file(READ "${source}" contents)
        string(REGEX MATCHALL "TEST_?F?\\(([A-Za-z_0-9 ,]+)\\)" found_tests ${contents})
        foreach(hit ${found_tests})
          string(REGEX REPLACE ".*\\(([A-Za-z_0-9]+)[, ]*([A-Za-z_0-9]+)\\).*" "\\1.\\2" test_name ${hit})
          add_test(${test_name} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}" --gtest_filter=${test_name})
          set_tests_properties(${test_name} PROPERTIES TIMEOUT 660 ENVIRONMENT "PATH=${NEWPATH}")
        endforeach()
      endif()
    endif()
  endforeach()
endmacro()

# Create source groups automatically based on file path
macro(CREATE_SRC_GROUPS SRC)
  foreach(F ${SRC})
    string(REGEX MATCH "(^.*)([/\\].*$)" M ${F})
    if(CMAKE_MATCH_1)
      string(REGEX REPLACE "[/\\]" "\\\\" DIR ${CMAKE_MATCH_1})
      source_group(${DIR} FILES ${F})
    else()
      source_group(\\ FILES ${F})
    endif()
  endforeach()
endmacro()

# Create test targets
macro(CREATE_TEST_TARGETS BASE_NAME SRC DEPENDENCIES)
  if(BUILD_TESTING)
    add_executable(${BASE_NAME}_tests ${SRC})

    if (APPLE)
      add_custom_command(TARGET ${BASE_NAME}_tests
        POST_BUILD
        COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "${openstudio_ROOT_DIR}/lib/" $<TARGET_FILE:${BASE_NAME}_tests>
        COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "${QT_INSTALL_DIR}/lib/" $<TARGET_FILE:${BASE_NAME}_tests>)
    endif()

    list(APPEND ALL_TESTING_TARGETS "${BASE_NAME}_tests")
    set(ALL_TESTING_TARGETS "${ALL_TESTING_TARGETS}" PARENT_SCOPE)


    CREATE_SRC_GROUPS("${SRC}")

    if(TARGET ${BASE_NAME})
      get_target_property(BASE_NAME_TYPE ${BASE_NAME} TYPE)
      if("${BASE_NAME_TYPE}" STREQUAL "EXECUTABLE")
        # don't link base name
        set(ALL_DEPENDENCIES ${DEPENDENCIES})
      else()
        # also link base name
        set(ALL_DEPENDENCIES ${BASE_NAME} ${DEPENDENCIES})
      endif()
    else()
      # don't link base name
      set(ALL_DEPENDENCIES ${DEPENDENCIES})
    endif()

    target_link_libraries(${BASE_NAME}_tests
      GTest::gtest
      ${ALL_DEPENDENCIES}
    )

    # Tell cmake to discover tests by calling test_exe --gtest_list_tests
    # gtest_discover_tests(${BASE_NAME}_tests
    #   PROPERTIES TIMEOUT 660
    # )

    # Unfortunately, the above won't work without a significant refactor of our cmake code.
    # There are ordering issues... openstudio_app is the one that copies most of the needed DLLs on windows (Qt, openstudiolib.dll, etc)
    # utilities is the first built, so when the test target is added it will try to call the openstudio_utilities_tests.exe and that will fail because of the missing DLLs
    ADD_GOOGLE_TESTS(${BASE_NAME}_tests ${SRC})

    if(TARGET "${BASE_NAME}_resources")
      add_dependencies("${BASE_NAME}_tests" "${BASE_NAME}_resources")
    endif()

    AddPCH(${BASE_NAME}_tests)

    ## suppress deprecated warnings in unit tests
    if(UNIX)
      set_target_properties(${ALL_TESTING_TARGETS} PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations")
    elseif(MSVC)
      set_target_properties(${ALL_TESTING_TARGETS} PROPERTIES COMPILE_FLAGS "/wd4996")
    endif()

  endif()
endmacro()

# adds custom command to update a resource via configure
macro(CONFIGURE_FILE_WITH_CHECKSUM INPUT_FILE OUTPUT_FILE)
  SET(TMP_OUTPUT_FILE "${OUTPUT_FILE}.tmp")

  if(NOT EXISTS "${OUTPUT_FILE}")
    configure_file( "${INPUT_FILE}" "${OUTPUT_FILE}" )
  else()
    configure_file( "${INPUT_FILE}" "${TMP_OUTPUT_FILE}" )
    file(MD5 "${OUTPUT_FILE}" EXISTING_HASH)
    file(MD5 "${TMP_OUTPUT_FILE}" NEW_HASH)
    if (NOT "${EXISTING_HASH}" MATCHES "${NEW_HASH}")
      configure_file( "${INPUT_FILE}" "${OUTPUT_FILE}" )
    endif()
  endif()
endmacro()

