if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
  # using Clang
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  #set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --unresolved-symbols=ignore-all")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --unresolved-symbols=ignore-all")
endif()


include_directories(${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

#target_link_libraries(init_openstudio
 ##ruby_OpenStudioModelEditor # linked by openstudio_modeleditor.so only, for SketchUp plugin
#)

add_library(openstudio_modeleditor_rb MODULE
  RubyAPI.hpp
  openstudio_modeleditor_rb.cpp
)
set_target_properties(openstudio_modeleditor_rb PROPERTIES PREFIX "")
set_target_properties(openstudio_modeleditor_rb PROPERTIES OUTPUT_NAME openstudio_modeleditor)

set_target_properties(openstudio_modeleditor_rb PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/ruby/")
set_target_properties(openstudio_modeleditor_rb PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/ruby/")
set_target_properties(openstudio_modeleditor_rb PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ruby/")

if(APPLE)
  set_target_properties(openstudio_modeleditor_rb PROPERTIES SUFFIX ".bundle" )
else()
  set_target_properties(openstudio_modeleditor_rb PROPERTIES SUFFIX ".so" )
endif()

if(MSVC)
  set_target_properties(openstudio_modeleditor_rb PROPERTIES COMPILE_FLAGS "/bigobj /wd4996 /wd5033")
endif()

add_custom_command(TARGET openstudio_modeleditor_rb
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/openstudio_modeleditor.rb $<TARGET_FILE_DIR:openstudio_modeleditor_rb>/openstudio_modeleditor.rb
)

target_link_libraries(openstudio_modeleditor_rb
  ruby_OpenStudioModelEditor
  openstudio_modeleditor
  ${QT_LIBS}
)

if( UNIX AND NOT APPLE )
  target_link_libraries(openstudio_modeleditor_rb ${QT_WEB_LIBS})
endif()

if( WIN32 )
  target_link_libraries(openstudio_modeleditor_rb ${RUBY_MINGW_STUB_LIB})
endif()

install(TARGETS openstudio_modeleditor_rb DESTINATION Ruby COMPONENT "RubyAPI")
install(FILES openstudio_modeleditor.rb DESTINATION Ruby COMPONENT "RubyAPI")


# don't need modeleditor libs in pat
if( BUILD_PAT )
  if( APPLE )
    install(TARGETS openstudio_rb
      DESTINATION ParametricAnalysisTool.app/Contents/Resources/OpenStudio/Ruby/
      COMPONENT PAT
    )
    install(FILES openstudio.rb
      DESTINATION ParametricAnalysisTool.app/Contents/Resources/OpenStudio/Ruby/
      COMPONENT PAT
    )
    install(DIRECTORY openstudio
      DESTINATION ParametricAnalysisTool.app/Contents/Resources/OpenStudio/Ruby/
      COMPONENT PAT
    )
  endif()
endif()
