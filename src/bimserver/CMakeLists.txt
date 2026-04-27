set(target_name openstudio_bimserver)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${QT_INCLUDES})

set(${target_name}_src
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

set(${target_name}_depends
  openstudio_qt_utils
)

add_library(${target_name} ${${target_name}_src})
target_link_libraries(${target_name} PUBLIC ${${target_name}_depends})
AddPCH(${target_name})

#if(NOT APPLE)
#install(TARGETS ${target_name}
#  RUNTIME DESTINATION bin
#  LIBRARY DESTINATION lib
#)
#endif()

CREATE_SRC_GROUPS("${${target_name}_src}")
CREATE_SRC_GROUPS("${${target_name}_test_src}")

CREATE_TEST_TARGETS(${target_name} "${${target_name}_test_src}" "${${target_name}_depends}")
#if(BUILD_TESTING)
#  add_dependencies(${target_name}_tests openstudio_bimserver_resources)
#endif()
