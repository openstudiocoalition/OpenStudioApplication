set(target_name openstudio_bimserver)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${QT_INCLUDES})

set(${target_name}_src
  BIMserverAPI.hpp
  mainpage.hpp
  BIMserverConnection.hpp
  BIMserverConnection.cpp
  ProjectImporter.hpp
  ProjectImporter.cpp
)

set(${target_name}_test_src
  Test/BIMserverFixture.hpp
  Test/BIMserverFixture.cpp
)

set(${target_name}_swig_src
  BIMserver.i
)

set(${target_name}_depends
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS}
  openstudio_modeleditor
  openstudio::openstudiolib
  ${QT_LIBS}
)
if(WIN32)
  list(APPEND ${target_name}_depends qtwinmigrate)
endif()

add_library(${target_name} ${${target_name}_src})
target_link_libraries(${target_name} ${${target_name}_depends})
AddPCH(${target_name})

if(BUILD_MICRO_PROFILING)
  target_link_libraries(${target_name} ${MICRO_PROFILER_LIB} )
endif()

#if(NOT APPLE)
#install(TARGETS ${target_name}
#  RUNTIME DESTINATION bin
#  LIBRARY DESTINATION lib
#)
#endif()

CREATE_SRC_GROUPS("${${target_name}_src}")
CREATE_SRC_GROUPS("${${target_name}_test_src}")
CREATE_SRC_GROUPS("${${target_name}_swig_src}")

CREATE_TEST_TARGETS(${target_name} "${${target_name}_test_src}" "${${target_name}_depends}")
#if(BUILD_TESTING)
#  add_dependencies(${target_name}_tests openstudio_bimserver_resources)
#endif()

#MAKE_SWIG_TARGET_OSAPP(OpenStudioGBXML gbXML "${CMAKE_CURRENT_SOURCE_DIR}/gbXML.i" "${${target_name}_swig_src}" ${target_name} OpenStudioEnergyPlus)
