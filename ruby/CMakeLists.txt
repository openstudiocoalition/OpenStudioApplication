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

target_include_directories(openstudio_modeleditor_rb PRIVATE ${RUBY_INCLUDE_DIRS})

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

# plugin doesn't need webengine
list(REMOVE_ITEM QT_WEB_LIBS Qt5::WebEngine)
list(REMOVE_ITEM QT_WEB_LIBS Qt5::WebEngineCore)
list(REMOVE_ITEM QT_WEB_LIBS Qt5::WebEngineWidgets)

if(MSVC)
  # wd4996=no deprecated warnings ; wd5033=register
  set_target_properties(openstudio_modeleditor_rb PROPERTIES COMPILE_FLAGS "/bigobj /wd4996 /wd5033")
endif()

add_custom_command(TARGET openstudio_modeleditor_rb
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/openstudio_modeleditor.rb $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/openstudio_modeleditor.rb
)

if(APPLE)
  add_custom_command(TARGET openstudio_modeleditor_rb
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/platforms/
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt5::QCocoaIntegrationPlugin> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/platforms/.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt5::Concurrent> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt5::DBus> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt5::Network> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt5::PrintSupport> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Qt5::Xml> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openstudio::openstudiolib> $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/.
    COMMAND ruby "${PROJECT_SOURCE_DIR}/SketchUpInstallName.rb" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ruby/openstudio_modeleditor.bundle"
  )
endif()

target_link_libraries(openstudio_modeleditor_rb
  ruby_OpenStudioModelEditor
  openstudio_lib
  openstudio_bimserver
  ${QT_WEB_LIBS}
)

if( WIN32 )
  include("${openstudio_ROOT_DIR}/lib/cmake/openstudio/FetchRubyMinGW.cmake")
  FetchRubyMinGW()
  target_link_libraries(openstudio_modeleditor_rb ${RUBY_MINGW_STUB_LIB})
endif()

install(TARGETS openstudio_modeleditor_rb DESTINATION Ruby COMPONENT "RubyAPI")
install(FILES openstudio_modeleditor.rb DESTINATION Ruby COMPONENT "RubyAPI")
if(APPLE)
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/platforms/$<TARGET_FILE_NAME:Qt5::QCocoaIntegrationPlugin> DESTINATION Ruby/platforms COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:Qt5::Concurrent> DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:Qt5::Core> DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:Qt5::DBus> DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:Qt5::Gui> DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:Qt5::Network> DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:Qt5::PrintSupport> DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:Qt5::Widgets> DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:openstudio::openstudiolib> DESTINATION Ruby COMPONENT "RubyAPI")
  install(FILES $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/$<TARGET_FILE_NAME:Qt5::Xml> DESTINATION Ruby COMPONENT "RubyAPI")
endif()

if( BUILD_PAT )
  # don't need modeleditor libs in pat
endif()
