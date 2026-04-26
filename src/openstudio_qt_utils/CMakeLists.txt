set(target_name openstudio_qt_utils)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QT_INCLUDES})

set(${target_name}_src
  OpenStudioQtUtilsAPI.hpp
  Application.hpp
  Application.cpp
  OSProgressBar.hpp
  OSProgressBar.cpp
  QMetaTypes.hpp
  QMetaTypes.cpp
  Utilities.hpp
  Utilities.cpp
)

add_library(${target_name} ${${target_name}_src})

set(${target_name}_depends
  openstudioapp_utilities
  ${QT_LIBS}
)

add_dependencies(${target_name} ${${target_name}_depends})
target_link_libraries(${target_name} PUBLIC ${${target_name}_depends})

if(BUILD_SHARED_LIBS)
  #target_compile_definitions(${target_name} PUBLIC openstudio_qt_utils_EXPORTS)
else()
  target_compile_definitions(${target_name} PUBLIC openstudio_qt_utils_EXPORTS)
endif()

CREATE_SRC_GROUPS("${${target_name}_src}")
