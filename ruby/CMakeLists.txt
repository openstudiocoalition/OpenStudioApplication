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


  # **NOTE**: we do not want to grab the conan one, which is statically built on all platforms, and is a msvc build on windows
  # Instead, we want to grab the regularly installed one on your system

  set(_RUBY_POSSIBLE_EXECUTABLE_NAMES
    ruby
    ruby3.2
    ruby3.2.2
    ruby32
    ruby322
  )

  # TODO: this isn't great but I haven't found a better way to locate the system ruby (and avoid the conan one)
  find_program(SYSTEM_RUBY_EXECUTABLE NAMES ${_RUBY_POSSIBLE_EXECUTABLE_NAMES}
    HINTS
      "/usr/local/rvm/rubies/ruby-3.2.2/bin/"
      "/usr/local/rvm/rubies/ruby-3.2.2/bin/ruby"
      "/usr/share/rvm/rubies/ruby-3.2.2/bin/"
      "$ENV{HOME}/.rvm/rubies/ruby-3.2.2/bin/"
      "$ENV{HOME}/.rbenv/versions/3.2.2/bin/"

      "C:/Ruby32-x64/bin/"

       "/usr/local/ruby322/bin/"
       "/usr/local/ruby32/bin/"
       "/usr/bin/"
       "/usr/local/bin/"

    NO_DEFAULT_PATH)

  if(SYSTEM_RUBY_EXECUTABLE)
    # Validate the version
    execute_process (COMMAND "${SYSTEM_RUBY_EXECUTABLE}" -e "puts RUBY_VERSION"
      RESULT_VARIABLE _result
      OUTPUT_VARIABLE _system_ruby_version
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_result OR (_system_ruby_version VERSION_LESS 3.2) OR (_system_ruby_version VERSION_GREATER_EQUAL 3.3))
      if(_result)
        message(WARNING "Cannot use the interpreter \"${SYSTEM_RUBY_EXECUTABLE}\"")
      else()
        message(WARNING "Wrong version \"${_system_ruby_version}\" for the interpreter \"${SYSTEM_RUBY_EXECUTABLE}\", not >= 3.2 < 3.3")
      endif()
      set_property (CACHE SYSTEM_RUBY_EXECUTABLE PROPERTY VALUE "SYSTEM_RUBY_EXECUTABLE-NOTFOUND")
    else()
      message(STATUS "Found SYSTEM_RUBY_EXECUTABLE=${SYSTEM_RUBY_EXECUTABLE} with version ${_system_ruby_version}")
    endif()
   endif()

  if(NOT SYSTEM_RUBY_EXECUTABLE)
    message(WARNING "A valid system ruby (3.2.2 or near) wasn't found, you won't be able to run the `ctest -R RubyTest` command and the tests won't be created at all.")
  else()
    # add a test for each unit test
    set(RUBY_TEST_REQUIRES "#include test files")
    foreach(f ${RUBY_TEST_SRC})

      file(READ "${f}" CONTENTS)
      string(REGEX MATCHALL "def +test_([A-Za-z_0-9 ]+)" FOUND_TESTS ${CONTENTS})

      foreach(HIT ${FOUND_TESTS})
        string(REGEX REPLACE "def +test_([A-Za-z_0-9]+)" "\\1" TEST_NAME ${HIT})
        string(REGEX MATCH "/?([A-Za-z_0-9 ]+)\\.rb" FILE_NAME ${f})
        string(REGEX REPLACE "/?([A-Za-z_0-9 ]+)\\.rb" "\\1" FILE_NAME ${FILE_NAME})

        set(CTEST_NAME "OSApp-RubyTest-${FILE_NAME}-${TEST_NAME}")

        # DLM: Cannot load openstudio_modeleditor.so into CLI because linked with different version of Ruby
        # CLI Test
        #add_test(NAME "OSApp-CLITest-${FILE_NAME}-${TEST_NAME}"
        #  COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_BINARY_DIR}"
        #    "${openstudio_EXECUTABLE}" "-I" "$<TARGET_FILE_DIR:openstudio_modeleditor_rb>"
        #    "${f}" "--name=test_${TEST_NAME}"
        #)

        # Call with Ruby itself
        add_test(NAME "${CTEST_NAME}"
          COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_BINARY_DIR}"
          "${SYSTEM_RUBY_EXECUTABLE}" "-I" "$<TARGET_FILE_DIR:openstudio::openstudio_rb>" # Or just ${openstudio_ROOT_DIR}/Ruby
            "-I" "$<TARGET_FILE_DIR:openstudio_modeleditor_rb>"
            "${f}" "--name=test_${TEST_NAME}"
        )
        set_tests_properties("${CTEST_NAME}" PROPERTIES TIMEOUT 660 )
      endforeach()

    endforeach()

  endif()
endif()
