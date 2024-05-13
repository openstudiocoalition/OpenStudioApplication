if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
  # using Clang
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  #set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --unresolved-symbols=ignore-all")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --unresolved-symbols=ignore-all")
endif()

if (UNIX)
  # Disable register warnings
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=register -Wno-register")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

add_library(openstudio_modeleditor_rb MODULE
  RubyAPI.hpp
  openstudio_modeleditor_rb.cpp
)

target_include_directories(openstudio_modeleditor_rb SYSTEM PRIVATE ${Ruby_INCLUDE_DIRS})

set_target_properties(openstudio_modeleditor_rb PROPERTIES PREFIX "")
set_target_properties(openstudio_modeleditor_rb PROPERTIES OUTPUT_NAME openstudio_modeleditor)

set_target_properties(openstudio_modeleditor_rb PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/ruby/")
set_target_properties(openstudio_modeleditor_rb PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/ruby/")
set_target_properties(openstudio_modeleditor_rb PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ruby/")

if(APPLE)
  set_target_properties(openstudio_modeleditor_rb PROPERTIES SUFFIX ".bundle" )

  # DLM: these properties affect the rpath of executables, not the rpath added before load library commands
  #set_target_properties(openstudio_modeleditor_rb PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE )
  #set_target_properties(openstudio_modeleditor_rb PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE )
  #set_target_properties(openstudio_modeleditor_rb PROPERTIES INSTALL_RPATH "@loader_path/../OpenStudioApp.app/Contents/Frameworks/" )
  #set_target_properties(openstudio_modeleditor_rb PROPERTIES BUILD_WITH_INSTALL_NAME_DIR TRUE )
  #set_target_properties(openstudio_modeleditor_rb PROPERTIES INSTALL_NAME_DIR "@loader_path/../OpenStudioApp.app/Contents/Frameworks/" )
else()
  set_target_properties(openstudio_modeleditor_rb PROPERTIES SUFFIX ".so" )
endif()

if(MSVC)
  # wd4996=no deprecated warnings ; wd5033=register
  set_target_properties(openstudio_modeleditor_rb PROPERTIES COMPILE_FLAGS "/bigobj /wd4996 /wd5033")
endif()

add_custom_command(TARGET openstudio_modeleditor_rb
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/openstudio_modeleditor.rb $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/openstudio_modeleditor.rb
)

if(APPLE)
  find_library(QT_QCOCOA NAMES libqcocoa.dylib PATHS "${QT_INSTALL_DIR}/plugins/platforms" NO_DEFAULT_PATH)

  add_custom_command(TARGET openstudio_modeleditor_rb
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/platforms/
    # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt6::QCocoaIntegrationPlugin> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/platforms/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_QCOCOA} $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/platforms/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_DIR}/lib/QtCore.framework $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtCore.framework
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_DIR}/lib/QtCore5Compat.framework $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtCore5Compat.framework
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_DIR}/lib/QtDBus.framework $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtDBus.framework
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_DIR}/lib/QtGui.framework $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtGui.framework
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_DIR}/lib/QtNetwork.framework $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtNetwork.framework
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_DIR}/lib/QtPrintSupport.framework $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtPrintSupport.framework
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_DIR}/lib/QtSvg.framework $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtSvg.framework
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_DIR}/lib/QtWidgets.framework $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtWidgets.framework
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_INSTALL_DIR}/lib/QtXml.framework $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtXml.framework
    # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openstudio::openstudiolib> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ruby "${PROJECT_SOURCE_DIR}/SketchUpInstallName.rb" $<TARGET_FILE:openstudio_modeleditor_rb>
    # Add for local build dir. TODO: find a better way, this sticks onto the installed openstudio_modeleditor.bundle
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "${openstudio_ROOT_DIR}/lib/" $<TARGET_FILE:openstudio_modeleditor_rb>
  )
endif()

target_link_libraries(openstudio_modeleditor_rb
  ruby_OpenStudioModelEditor
  openstudio_modeleditor
)

if( WIN32 )
  include("${openstudio_ROOT_DIR}/lib/cmake/openstudio/FetchRubyMinGW.cmake")
  FetchRubyMinGW()
  target_link_libraries(openstudio_modeleditor_rb ${RUBY_MINGW_STUB_LIB})
endif()

install(TARGETS openstudio_modeleditor_rb DESTINATION Ruby COMPONENT "RubyAPI")
install(FILES openstudio_modeleditor.rb DESTINATION Ruby COMPONENT "RubyAPI")

if(APPLE)
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/platforms/libqcocoa.dylib DESTINATION Ruby/platforms COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtCore.framework DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtCore5Compat.framework DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtDBus.framework DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtGui.framework DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtNetwork.framework DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtPrintSupport.framework DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtSvg.framework DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtWidgets.framework DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/QtXml.framework DESTINATION Ruby COMPONENT "RubyAPI")
  # install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:openstudio::openstudiolib> DESTINATION Ruby COMPONENT "RubyAPI")
endif()

###############################################################################
#                        T E S T I N G:   C T E S T S                         #
###############################################################################

if(BUILD_TESTING)

  # find all tests
  file(GLOB RUBY_TEST_SRC "test/*.rb")

  # TODO: It doesn't work with this executable that's in build/Ruby-install/bin
  # message("RUBY_EXECUTABLE=${RUBY_EXECUTABLE}")

  # I added FindRuby.cmake from cmake 3.18.2 (after patching cmake to support versions greater than 2 and adding RVM support) in the CMake folder which is part of the CMAKE_MODULE_PATH
  set(Ruby_FIND_VIRTUALENV FIRST)
  find_package(Ruby 2.7)

  if(NOT Ruby_EXECUTABLE)
    message(WARNING "Your system ruby wasn't found, you won't be able to run the `ctest -R RubyTest` command and the tests won't be created at all.")
  else()
    message(STATUS "Found Ruby_EXECUTABLE=${Ruby_EXECUTABLE}")
  endif()

  # add a test for each unit test
  set(RUBY_TEST_REQUIRES "#include test files")
  foreach(f ${RUBY_TEST_SRC})

    file(READ "${f}" CONTENTS)
    string(REGEX MATCHALL "def +test_([A-Za-z_0-9 ]+)" FOUND_TESTS ${CONTENTS})

    foreach(HIT ${FOUND_TESTS})
      string(REGEX REPLACE "def +test_([A-Za-z_0-9]+)" "\\1" TEST_NAME ${HIT})
      string(REGEX MATCH "/?([A-Za-z_0-9 ]+)\\.rb" FILE_NAME ${f})
      string(REGEX REPLACE "/?([A-Za-z_0-9 ]+)\\.rb" "\\1" FILE_NAME ${FILE_NAME})

      # DLM: Cannot load openstudio_modeleditor.so into CLI because linked with different version of Ruby
      # CLI Test
      #add_test(NAME "OSApp-CLITest-${FILE_NAME}-${TEST_NAME}"
      #  COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_BINARY_DIR}"
      #    "${openstudio_EXECUTABLE}" "-I" "$<TARGET_FILE_DIR:openstudio_modeleditor_rb>"
      #    "${f}" "--name=test_${TEST_NAME}"
      #)

      #set_tests_properties("OSApp-CLITest-${FILE_NAME}-${TEST_NAME}" PROPERTIES TIMEOUT 660 )

      if (Ruby_EXECUTABLE)
        # Call with Ruby itself
        add_test(NAME "OSApp-RubyTest-${FILE_NAME}-${TEST_NAME}"
          COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_BINARY_DIR}"
            "${Ruby_EXECUTABLE}" "-I" "$<TARGET_FILE_DIR:openstudio::openstudio_rb>" # Or just ${openstudio_ROOT_DIR}/Ruby
            "-I" "$<TARGET_FILE_DIR:openstudio_modeleditor_rb>"
            "${f}" "--name=test_${TEST_NAME}"
        )

        set_tests_properties("OSApp-RubyTest-${FILE_NAME}-${TEST_NAME}" PROPERTIES TIMEOUT 660 )
      endif()

    endforeach()
  endforeach()
endif()
