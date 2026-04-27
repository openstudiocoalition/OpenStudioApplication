set(target_name openstudio_qt_utils)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QT_INCLUDES})

set(${target_name}_src
  Application.hpp
  Application.cpp
  OSProgressBar.hpp
  OSProgressBar.cpp
  PathWatcher.hpp
  PathWatcher.cpp
  QMetaTypes.hpp
  QMetaTypes.cpp
  Utilities.hpp
  Utilities.cpp
)

set(${target_name}_moc
  PathWatcher.hpp
)

qt6_wrap_cpp(${target_name}_mocs ${${target_name}_moc})

add_library(${target_name} STATIC ${${target_name}_src} ${${target_name}_mocs})

set(${target_name}_depends
  openstudioapp_utilities
  ${QT_LIBS}
)

target_link_libraries(${target_name} PUBLIC ${${target_name}_depends})

CREATE_SRC_GROUPS("${${target_name}_src}")

set(${target_name}_test_src
  test/QtUtilsFixture.hpp
  test/QtUtilsFixture.cpp
  test/PathWatcher_GTest.cpp
)

set(${target_name}_test_depends
  ${${target_name}_depends}
)

CREATE_TEST_TARGETS(${target_name} "${${target_name}_test_src}" "${${target_name}_test_depends}")
