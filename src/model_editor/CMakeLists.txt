set(target_name openstudio_modeleditor)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QT_INCLUDES})

#--------------------------------------------------------------------------------
# First the library
#--------------------------------------------------------------------------------

# lib files
set(${target_name}_src
  AccessPolicyStore.hpp
  AccessPolicyStore.cpp
  mainpage.hpp
  BridgeClasses.hpp
  BridgeClasses.cpp
  GithubReleases.hpp
  GithubReleases.cpp
  InspectorGadget.hpp
  InspectorGadget.cpp
  ListWidget.hpp
  ListWidget.cpp
  ModalDialogs.hpp
  ModalDialogs.cpp
  PathWatcher.hpp
  PathWatcher.cpp
  TableView.hpp
  TableView.cpp
  TableWidget.hpp
  TableWidget.cpp
  TestButton.hpp
  TestButton.cpp
  IGLineEdit.hpp
  IGLineEdit.cpp
  IGSpinBoxes.hpp
  IGSpinBoxes.cpp
)

# lib moc files
set(${target_name}_moc
  InspectorGadget.hpp
  ListWidget.hpp
  ModalDialogs.hpp
  PathWatcher.hpp
  TableView.hpp
  TableWidget.hpp
  TestButton.hpp
  IGLineEdit.hpp
  IGSpinBoxes.hpp
  BridgeClasses.hpp
)

# lib resource files
set(${target_name}_qrc
  modeleditorlib.qrc
)

# configure the AboutBox.hpp file
# this needs to be configured to embed the build number
CONFIGURE_FILE_WITH_CHECKSUM(AboutBox.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/AboutBox.hpp")

## Qt MOC generation
qt6_wrap_cpp(${target_name}_mocs ${${target_name}_moc})

# generate rules for building source files from the resources
qt6_add_resources(${target_name}_qrcs ${${target_name}_qrc})

# make the shared library
add_library(${target_name} STATIC
  ${${target_name}_src}
  ${${target_name}_moc}
  ${${target_name}_mocs}
  ${${target_name}_qrc}
  ${${target_name}_qrcs}
)

# lib dependencies
set(${target_name}_depends
  openstudio_qt_utils
)

add_dependencies(${target_name} ${${target_name}_depends})
target_link_libraries(${target_name} PUBLIC ${${target_name}_depends})

set(${target_name}_test_src
  test/ModelEditorFixture.hpp
  test/ModelEditorFixture.cpp
  test/IGLineEdit_GTest.cpp
  test/ModalDialogs_GTest.cpp
  test/PathWatcher_GTest.cpp
  test/QMetaTypes_GTest.cpp
  test/Utilities_GTest.cpp
  test/GithubReleases_GTest.cpp
)

set(${target_name}_test_depends
  ${${target_name}_depends}
)

CREATE_TEST_TARGETS(${target_name} "${${target_name}_test_src}" "${${target_name}_test_depends}")
CREATE_SRC_GROUPS("${${target_name}_test_src}")
